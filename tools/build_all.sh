#!/bin/bash
# Build & package Phase (UID PHSE) via official Akai MPC Plugin Builder SDK
# and (optionally) Open MPC sdk_release.py for per-architecture catalog zips.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
MPC_SDK="${MPC_SDK:-}"
OPEN_MPC="${OPEN_MPC:-}"
VERSION="1.0.1"

if [ ! -f "$MPC_SDK/tools/build.py" ]; then
  echo "ERROR: Set MPC_SDK to a checkout of https://github.com/inmusic-software/mpc-plugin-builder-sdk" >&2
  exit 1
fi

echo "== 1/3: Generating Phase artwork & MPC skins =="
python3 "$ROOT/tools/gen_ui_assets.py"
python3 "$ROOT/tools/gen_skins.py"
python3 "$ROOT/tools/render_preview.py"

echo "== 2/3: Building Phase (native + ARM32 + ARM64 + validate) =="
python3 "$MPC_SDK/tools/build.py" "$ROOT/plugins/phase"

echo "== 3/3: Packaging deployable ZIPs =="
mkdir -p "$ROOT/dist"
if [ -n "$OPEN_MPC" ] && [ -f "$OPEN_MPC/tools/sdk_release.py" ]; then
  python3 "$OPEN_MPC/tools/sdk_release.py" \
    "$ROOT/plugins/phase/build/Stampman3000 - VST - Phase" \
    --version "$VERSION" \
    --id phase \
    --repo stampman3000-jpg/phase \
    --license GPL-3.0-or-later \
    -o "$ROOT/dist" \
    -- --about "CZ-derived phase-distortion polysynth based on CrispyZebra with 8 PD waveforms, dual-line layering, vibrato, stereo chorus and diffused Space FX."
  for z in "$ROOT/dist/Phase-${VERSION}-mpc-armv7.zip" "$ROOT/dist/Phase-${VERSION}-mpc-aarch64.zip"; do
    python3 "$OPEN_MPC/tools/catalog_check.py" "$z" --catalog
  done
else
  python3 "$MPC_SDK/tools/package.py" "$ROOT/plugins/phase" --out "$ROOT/dist"
fi
ls -lh "$ROOT/dist"/*.zip
