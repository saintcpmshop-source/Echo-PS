#!/usr/bin/env node
/**
 * Echo PS Bridge — fulfills PS3 install requests from GitHub Releases.
 * Proprietary — see ../LICENSE. No fork, no redistribution.
 */

const fs = require("fs");
const path = require("path");
const crypto = require("crypto");
const axios = require("axios");

const CONFIG_PATH = path.join(__dirname, "config.json");

function loadConfig() {
  if (!fs.existsSync(CONFIG_PATH)) {
    console.error("Missing config.json (copy config.example.json).");
    process.exit(1);
  }
  return JSON.parse(fs.readFileSync(CONFIG_PATH, "utf8"));
}

function ghHeaders(token) {
  const h = {
    "User-Agent": "echo-ps-bridge",
    Accept: "application/vnd.github+json",
    "X-GitHub-Api-Version": "2022-11-28",
  };
  if (token) h.Authorization = `Bearer ${token}`;
  return h;
}

async function fetchJson(url, token) {
  const r = await axios.get(url, { headers: ghHeaders(token), timeout: 30000 });
  return r.data;
}

async function fetchText(url, token) {
  const r = await axios.get(url, {
    headers: { ...ghHeaders(token), Accept: "*/*" },
    timeout: 30000,
    responseType: "text",
    transformResponse: (x) => x,
  });
  return r.data;
}

async function download(url, dest, token) {
  dest = path.resolve(dest);
  fs.mkdirSync(path.dirname(dest), { recursive: true });
  const tmp = dest + ".part";
  const r = await axios.get(url, {
    headers: { ...ghHeaders(token), Accept: "application/octet-stream" },
    responseType: "stream",
    timeout: 0,
    maxRedirects: 5,
  });
  await new Promise((res, rej) => {
    const w = fs.createWriteStream(tmp);
    r.data.pipe(w);
    r.data.on("error", rej);
    w.on("finish", res);
    w.on("error", rej);
  });
  fs.renameSync(tmp, dest);
}

function sha256File(p) {
  const h = crypto.createHash("sha256");
  h.update(fs.readFileSync(p));
  return h.digest("hex");
}

function pickAsset(assets, preferName) {
  if (preferName) {
    const exact = assets.find((a) => a.name === preferName);
    if (exact) return exact;
  }
  const zip = assets.find((a) => a.name.endsWith(".zip"));
  if (zip) return zip;
  return assets[0];
}

async function resolveApp(repo, token) {
  const [owner, name] = repo.split("/");
  if (!owner || !name) throw new Error(`bad repo: ${repo}`);

  const rel = await fetchJson(
    `https://api.github.com/repos/${owner}/${name}/releases/latest`,
    token
  );

  const version = (rel.tag_name || "").replace(/^v/, "");
  const assets = rel.assets || [];

  let manifest = null;
  const manifestAsset = assets.find((a) => a.name === "manifest.json");
  if (manifestAsset) {
    const txt = await fetchText(manifestAsset.browser_download_url, token);
    manifest = JSON.parse(txt);
  } else {
    try {
      const raw = await fetchText(
        `https://raw.githubusercontent.com/${owner}/${name}/HEAD/manifest.json`,
        token
      );
      manifest = JSON.parse(raw);
    } catch (e) {
      throw new Error(`no manifest.json found in ${repo}`);
    }
  }

  if (!manifest.id) throw new Error(`manifest missing id in ${repo}`);

  const zipAsset = pickAsset(assets, `${manifest.id}-${version}.zip`);
  if (!zipAsset) throw new Error(`no zip asset in release for ${repo}`);

  let sha256 = null;
  const shaAsset = assets.find((a) => a.name.endsWith(".sha256"));
  if (shaAsset) {
    const txt = await fetchText(shaAsset.browser_download_url, token);
    const m = txt.match(/[a-f0-9]{64}/i);
    if (m) sha256 = m[0].toLowerCase();
  }
  if (!sha256 && rel.body) {
    const m = rel.body.match(/sha256\s*[:=]\s*([a-f0-9]{64})/i);
    if (m) sha256 = m[1].toLowerCase();
  }

  return {
    version: version || manifest.version,
    manifest,
    zipAsset,
    sha256,
    downloadUrl: zipAsset.browser_download_url,
  };
}

function mergeRegistry(pkgDir, manifest, version) {
  const regPath = path.join(pkgDir, "..", "apps", "registry.json");
  fs.mkdirSync(path.dirname(regPath), { recursive: true });
  let reg = { schemaVersion: 1, apps: [] };
  if (fs.existsSync(regPath)) {
    try { reg = JSON.parse(fs.readFileSync(regPath, "utf8")); } catch {}
    if (!Array.isArray(reg.apps)) reg.apps = [];
  }
  const entry = {
    id: manifest.id,
    name: manifest.name,
    version: version,
    type: manifest.type || "section",
    free: manifest.free !== false,
    description: manifest.description || "",
    repo: manifest.repo || "",
  };
  const i = reg.apps.findIndex((a) => a.id === entry.id);
  if (i >= 0) reg.apps[i] = entry; else reg.apps.push(entry);
  reg.updated = new Date().toISOString();
  fs.writeFileSync(regPath, JSON.stringify(reg, null, 2));
}

async function fulfill(reqPath, store, cfg) {
  const req = JSON.parse(fs.readFileSync(reqPath, "utf8"));
  const appId = req.app || req.id;
  if (!appId) { console.warn(`[!] request without app id: ${reqPath}`); return; }

  const entry = (store.apps || []).find(
    (a) => a.id === appId || (a.repo || "").endsWith(`/${appId}`)
  );
  if (!entry || !entry.repo) {
    console.warn(`[!] ${appId} not found in store/index.json`);
    return;
  }

  const usb = path.resolve(cfg.usb_path);
  const pkgDir = path.join(usb, "EchoPS", "pkgs");
  fs.mkdirSync(pkgDir, { recursive: true });

  console.log(`[+] Resolving ${entry.repo} ...`);
  const info = await resolveApp(entry.repo, cfg.github_token);
  console.log(`[+] Latest: ${info.manifest.id} v${info.version}`);

  const dest = path.join(pkgDir, `${info.manifest.id}-${info.version}.zip`);
  if (!fs.existsSync(dest)) {
    console.log(`[+] Downloading ${info.zipAsset.name} ...`);
    await download(info.downloadUrl, dest, cfg.github_token);
  } else {
    console.log(`[=] Already downloaded: ${dest}`);
  }

  const digest = sha256File(dest);
  if (info.sha256 && info.sha256 !== digest) {
    console.error(`[!] SHA256 mismatch for ${info.manifest.id}`);
    try { fs.unlinkSync(dest); } catch {}
    return;
  }

  fs.writeFileSync(
    path.join(pkgDir, `${info.manifest.id}.manifest.json`),
    JSON.stringify(info.manifest, null, 2)
  );

  mergeRegistry(pkgDir, info.manifest, info.version);

  const done = reqPath.replace(/\.json$/, ".done");
  fs.writeFileSync(done, JSON.stringify({
    app: info.manifest.id,
    version: info.version,
    sha256: digest,
    repo: entry.repo,
    ok: true,
  }, null, 2));
  console.log(`[✓] Installed ${info.manifest.id} v${info.version}`);
}

async function fetchStore(url, token) {
  const r = await axios.get(url, {
    headers: { "User-Agent": "echo-ps-bridge", Accept: "*/*" },
    timeout: 30000,
    transformResponse: (x) => x,
  });
  return JSON.parse(r.data);
}

async function main() {
  const cfg = loadConfig();
  const usb = path.resolve(cfg.usb_path);
  const reqDir = path.join(usb, "EchoPS", "requests");
  fs.mkdirSync(reqDir, { recursive: true });

  console.log(`Echo PS Bridge v0.2.0`);
  console.log(`USB:   ${usb}`);
  console.log(`Store: ${cfg.store_index_url}`);
  console.log(`Watch: ${reqDir}`);

  let store = null;
  let storeAge = 0;

  while (true) {
    try {
      if (!store || Date.now() - storeAge > 60000) {
        store = await fetchStore(cfg.store_index_url, cfg.github_token);
        storeAge = Date.now();
        console.log(`[=] Store index loaded: ${(store.apps || []).length} apps`);
      }

      const files = fs.readdirSync(reqDir).filter(
        (f) => f.startsWith("install-") && f.endsWith(".json")
      );
      for (const f of files) {
        const reqPath = path.join(reqDir, f);
        const donePath = reqPath.replace(/\.json$/, ".done");
        if (fs.existsSync(donePath)) continue;
        try { await fulfill(reqPath, store, cfg); }
        catch (e) { console.error(`[!] Error fulfilling ${f}: ${e.message}`); }
      }
    } catch (e) {
      console.error(`[!] Loop error: ${e.message}`);
      store = null;
    }
    await new Promise((r) => setTimeout(r, (cfg.poll_seconds || 2) * 1000));
  }
}

main().catch((e) => { console.error(e); process.exit(1); });
