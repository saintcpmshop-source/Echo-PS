#!/usr/bin/env bash
# Echo PS — publish an app release to GitHub.
# Usage: tools/publish-app.sh <app_repo_dir> <version>
# Example: tools/publish-app.sh ../echo-ps-saves 0.1.0
set -euo pipefail

SRC="${1:?usage: publish-app.sh <app_repo_dir> <version>}"
VER="${2:?usage: publish-app.sh <app_repo_dir> <version>}"

cd "$SRC"

if [ ! -f manifest.json ]; then
  echo "manifest.json missing in $SRC" >&2
  exit 1
fi

ID="$(node -e "console.log(require('./manifest.json').id)")"
ZIP="${ID}-${VER}.zip"

mkdir -p dist
rm -f "dist/${ZIP}" "dist/${ZIP}.sha256"

# Package: manifest + built binary (adjust build path if needed)
( cd "$SRC" && zip -r "dist/${ZIP}" manifest.json build/ ) >/dev/null

sha256sum "dist/${ZIP}" | awk '{print $1}' > "dist/${ZIP}.sha256"

echo "Built:    dist/${ZIP}"
echo "SHA256:   $(cat dist/${ZIP}.sha256)"

if command -v gh >/dev/null 2>&1; then
  gh release create "v${VER}" \
    "dist/${ZIP}" "dist/${ZIP}.sha256" \
    --title "${ID} v${VER}" \
    --notes "${ID} v${VER}

sha256: $(cat dist/${ZIP}.sha256)"
  echo "Published release v${VER}"
else
  echo "gh CLI not found — create the release manually with these assets:"
  echo "  dist/${ZIP}"
  echo "  dist/${ZIP}.sha256"
fi
