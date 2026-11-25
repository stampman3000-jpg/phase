# AGENTS.md — Build & Maintenance Guide for Phase

## Project Identity
- **Plugin Name:** Phase (`UID: PHSE`, vendor `Stampman3000`, kind `instrument`)
- **Repository:** `stampman3000-jpg/phase`
- **License:** `GPL-3.0-or-later` (derived from [CrispyZebra](https://github.com/hugelton/CrispyZebra) by Leo Kuroshita `@kurogedelic`, pinned upstream commit `db6af5c`)
- **Targets:** Akai MPC OS 3.9.1 & 3.10.1+ — 32-bit ARM (`armv7`, Cortex-A17) and 64-bit ARM (`aarch64`, Cortex-A55)

## Repository Layout
- `dsp/CrispyZebra.h` — Modified CrispyZebra phase-distortion core (interpolated unipolar cosine LUT, convex DCW curve, exponential resonant windows, pitched LFSR phase-noise, zero-click voice stealing, oldest-held overlapping Note-Off, per-voice aged drift, 11.8 kHz silk LPF).
- `dsp/PhaseEngine.h` — Host-independent stereo wrapper adding dual-tap sine-LFO stereo BBD chorus, tempo-synced diffused stereo Space FX (true pre-delay send), and C1/C2-smooth output peak protector.
- `bridge/PhasePresets.h` — 32 curated factory presets (Glassy Sines & Ambient Pads, 80s/90s Saws/Brass/Strings, Fat PD Basses, CZ Keys/Bells, FX & Init).
- `plugins/phase/plugin.json` & `plugins/phase/src/PhasePlugin.cpp` — Official Akai MPC Plugin Builder SDK VST2 entrypoint (32 automatable params + 4 UI telemetry handles, 100% 4x4 Q-Link mapped across 2 tabs).
- `tools/gen_ui_assets.py`, `tools/gen_skins.py`, `tools/render_preview.py` — Procedural 1280x628 faceplate, 4x-supersampled dials/sliders/LCD strips, `TUI.json` / `Q-Links.json`, and `screenshots/phase.png`.
- `tests/test_phase.cpp` & `tests/bench_phase.cpp` — 10-check DSP validation suite and desktop CPU benchmark.

## Build & Test Commands
Requires:
- `MPC_SDK`: checkout of `https://github.com/inmusic-software/mpc-plugin-builder-sdk` (uses Zig cross-compiler automatically, or `--toolchain gcc` / `--toolchain docker`)
- `OPEN_MPC` (optional, for catalog release zips): checkout of `https://github.com/open-mpc/mpc-vst-plugins`
- Python 3 + Pillow (`PIL`) for procedural UI artwork generation

```bash
# Build skins, ARM32, ARM64, native dylib, run Akai validate, and package release zips
MPC_SDK=/path/to/mpc-plugin-builder-sdk OPEN_MPC=/path/to/mpc-vst-plugins bash tools/build_all.sh

# Run all 10 DSP tests, desktop CPU benchmark, and official Akai host_sim
MPC_SDK=/path/to/mpc-plugin-builder-sdk bash tools/run_tests.sh
```

## Compiler Flags (via Akai MPC SDK `tools/build.py`)
- **Common flags:** `-std=c++17 -O2 -fPIC -fvisibility=hidden -ffunction-sections -fdata-sections -Wall -Wextra`
- **ARMv7 (`file32: VST/phase-arm32.so`):** Zig `-target arm-linux-gnueabihf.2.31 -mcpu=cortex_a17` (GCC equivalent: `-march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard`)
- **AArch64 (`file64: VST/phase-arm64.so`):** Zig `-target aarch64-linux-gnu.2.31 -mcpu=cortex_a55` (GCC equivalent: `-march=armv8-a`)

## Release & Repo Rules
- Commit as `Stampman3000 <stampman3000@gmail.com>`.
- Never commit `build/`, `dist/`, `build_tests/`, `.so`, `.dylib`, `.zip`, or generated PNGs other than `screenshots/phase.png`.
- Never embed third-party manufacturer names or logos in the plugin name, skin artwork, or screenshot.
- Never delete, replace, or re-upload published release zips — cut a new version tag for fixes.
- Always validate release zips with `python3 "$OPEN_MPC/tools/catalog_check.py" <zip> --catalog` against latest `open-mpc/mpc-vst-plugins` `main`.
