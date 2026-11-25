# Attribution, Licensing & Upstream Source Record — Phase

## Licence

**Phase** (`UID: PHSE`, vendor `Stampman3000`) is distributed under the **GNU General Public License v3.0 or later** (`GPL-3.0-or-later`), matching the upstream licence of **CrispyZebra**. See [LICENSE](LICENSE) for the full licence text.

## Upstream Engine Attribution

- **Original project:** **CrispyZebra** (C++ CZ-style Phase Distortion synthesizer engine)
- **Author:** Leo Kuroshita (`@kurogedelic`)
- **Upstream repository:** https://github.com/hugelton/CrispyZebra
- **Pinned upstream commit:** `db6af5c` (v1.0.0, GPL-3.0)
- **MPC Adaptation Author / Port Packager:** `Stampman3000` (`Phase`)

## Modifications Made to Upstream `CrispyZebra.h` (`dsp/CrispyZebra.h`)

The core 8-waveform Phase Distortion architecture (`SAW`, `SQUARE`, `PULSE`, `DOUBLE_SINE`, `SAW_PULSE`, `REZ_SAW`, `REZ_TRI`, `REZ_TRAP`), Q16 8-stage `Envelope`, `LFO`, and dual-line `Voice` / `Engine` architecture are derived from upstream CrispyZebra (`db6af5c`), with adaptations for standalone Akai MPC ARMv7/ARM64 playability:

1. **Interpolated Unipolar Cosine Core & Convex DCW Transfer (`Tables`, `Oscillator::nextRawSampleFloat`)**: Added a 2048-point linear-interpolated unipolar cosine LUT (`> 100 dB` sine purity), convex Casio-style DCW knee mapping (`0–55%` warm sine/EP sweet spot through bright sawtooth), exponential 3.6-octave musical resonant sweeps (`1×–12×` with frequency-aware anti-aliasing floor), continuous `0.0` cycle endpoints for click-free `Wave 1 + Wave 2` alternation, per-sample smoothed DCW/DCA envelopes, and pitched 16-bit Galois LFSR phase-noise modulation.
2. **Voice Management, Zero-Click Stealing & Polyphonic Output (`Voice`, `Engine`)**: Added free-voice-first allocation, oldest-held-voice release on overlapping same-pitch Note-Offs, ~3 ms exponential anti-click voice-steal crossfade, post-ring DCA staging, free-running sine + slow tape-wow vibrato LFO that glides through release tails, subtle aged-hardware per-voice calibration tolerance and thermal drift, per-voice stereo panning, and a linear high-headroom stereo bus with an `11.8 kHz` reconstruction silk filter and gentle `240 Hz` low-mid warmth.
3. **MPC Host Adapter, Presets & UI (`dsp/PhaseEngine.h`, `bridge/PhasePresets.h`, `plugins/phase/*`, `tools/*`)**: Newly authored GPL-3.0-or-later MPC VST2 plugin wrapper, dual-tap stereo BBD chorus, diffused tempo-synced stereo Space FX, 32 factory presets, procedural faceplate UI generator, and MPC `TUI.json` / `Q-Links.json` skin generator.

## SDK & Community Dependencies

- **Official Akai MPC Plugin Builder SDK** (`inmusic-software/mpc-plugin-builder-sdk`, MIT License):
  - Provides `framework/mpc_plugin.h`, `framework/mpc_vst2_abi.h`, `tools/build.py`, `tools/validate.py`, `tools/package.py`, and `host_sim`.
- **Open MPC Community Toolchain** (`open-mpc/mpc-vst-plugins`, MIT License):
  - Used for per-architecture release packaging (`tools/sdk_release.py`) and catalog validation (`tools/catalog_check.py`).

## Trademark Disclaimer

Casio, CZ-101, CZ-1000, CZ-5000, Akai, MPC, and VST are registered trademarks of their respective owners. Phase is an independent open-source project by Stampman3000 and is not affiliated with or endorsed by any trademark holder.
