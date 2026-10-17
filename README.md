<!-- Echo PS — README (midnight purple + metallic gold, animated, NOT PUBLIC YET) -->

<p align="center">
  <img src="assets/banner.svg" alt="Echo PS" width="100%"/>
</p>

<p align="center">
  <img src="assets/echopslogo.jpg" alt="Echo PS logo" width="180"/>
</p>

<p align="center">
  <b style="color:#B8860B">⛔ NOT PUBLIC YET — PRIVATE DEVELOPMENT BUILD ⛔</b><br/>
  <sub>This repository is not public. Do not share, mirror, fork, or redistribute. See <a href="LICENSE">LICENSE</a>.</sub>
</p>

<p align="center">
  <img alt="status" src="https://img.shields.io/badge/status-NOT%20PUBLIC%20YET-2A1247?style=for-the-badge&labelColor=0B0614&color=FFD700"/>
  <img alt="license" src="https://img.shields.io/badge/license-Proprietary-2A1247?style=for-the-badge&labelColor=0B0614&color=FFD700"/>
  <img alt="platform" src="https://img.shields.io/badge/platform-PS3%20CFW-2A1247?style=for-the-badge&labelColor=0B0614&color=FFD700"/>
  <img alt="sdk" src="https://img.shields.io/badge/sdk-PSL1GHT-2A1247?style=for-the-badge&labelColor=0B0614&color=FFD700"/>
</p>

<p align="center">
  <i>“Install once. Echo forever.”</i>
</p>

---

> ### ⚠️ This project is NOT public yet
> Echo PS is in **private development**. This repository is **proprietary**.
> **Forking, redistribution, modification, and derivative works are prohibited.**
> See [LICENSE](LICENSE). For permissions, contact the copyright holder.

---

## 🌌 What is Echo PS?

**Echo PS** is a polished, all-in-one homebrew hub for jailbroken **PlayStation 3** consoles.

One app. Many sections. An in-app store that delivers new apps to your console
via a **PC bridge**. The console never touches the network — the bridge handles
downloads, accounts, and updates, then writes them to USB.

<p align="center">
  <img src="assets/banner.svg" alt="" width="60%"/>
</p>

<details>
<summary><b style="color:#B8860B">✨ Features (click to expand)</b></summary>

- 🎛️ **Polished 720p UI** — midnight purple + metallic gold theme
- 🚀 **Boot splash** — animated logo + progress bar
- 🧩 **App registry** — reads `/EchoPS/index.json` from USB
- 🛒 **Store section** — browse apps, press X to install
- 🔁 **Install-request flow** — writes to `/EchoPS/requests/`
- 🌉 **Echo PS Bridge** — Node.js watcher, downloads + SHA-256 verify
- 🔒 **Offline-safe core** — no networking on the console

</details>

<details>
<summary><b style="color:#B8860B">🧱 Architecture (click to expand)</b></summary>

| Component | Runs on | Role |
|---|---|---|
| **Core** | PS3 (PSL1GHT) | UI, registry, store, install requests |
| **Bridge** | PC / phone | Downloads, auth, payments, updates |
| **Apps** | PS3 | Downloadable modules (`manifest.json`) |
| **Store index** | Hosted JSON | Catalog of available apps |

The console writes an *install request* to USB; the Bridge fulfills it and
writes the package back. **Console calls out, Bridge echoes back.**

</details>

<details>
<summary><b style="color:#B8860B">🛠️ Build (click to expand)</b></summary>

```bash
# Requirements: PSL1GHT SDK, make, gcc, xxd
cd core
make
# Output: core/build/echo_ps.self
```

</details>

<details>
<summary><b style="color:#B8860B">📦 Install on CFW PS3 (click to expand)</b></summary>

1. Copy `echo_ps.self` to `usb:/PS3/GAMEZ/ECHOPS/` or `dev_hdd0/game/ECHOPS/USRDIR/`
2. Launch via **webMAN** / **multiMAN**
3. Echo PS reads/writes `usb:/EchoPS/`

</details>


</details>

<details>
<summary><b style="color:#B8860B">🎨 Branding (click to expand)</b></summary>

- **Midnight purple:** `#0B0614` · `#1A0B2E` · `#2A1247`
- **Metallic gold:** `#B8860B` · `#FFD700` · `#FFF3B0`
- Drop `echopslogo.jpg` into `assets/` and `core/assets/` before building.

</details>

---

## 🚫 License

**Proprietary. All rights reserved.**
No forking. No redistribution. No derivative works.
See [LICENSE](LICENSE).

<p align="center">
  <sub>© 2026 Echo PS — Not public yet.</sub>
</p>

<p align="center">
  <img src="assets/banner.svg" alt="" width="100%"/>
</p>
