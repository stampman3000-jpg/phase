# Attribution, Licensing & Upstream Source Record — CZboy

## Licence

**CZboy** (`UID: CZBY`, vendor `Stampman3000`) is distributed under the **GNU General Public License v3.0 or later** (`GPL-3.0-or-later`), matching the upstream licence of **CrispyZebra**. See [LICENSE](LICENSE) for the full licence text.

## Upstream Engine Attribution

- **Original project:** **CrispyZebra** (fixed-point C++ CZ-series Phase Distortion synthesizer engine)
- **Author:** Leo Kuroshita (`@kurogedelic`)
- **Upstream repository:** https://github.com/hugelton/CrispyZebra
- **Pinned upstream commit:** `db6af5c` (v1.0.0, GPL-3.0)
- **MPC Adaptation Author / Port Packager:** `Stampman3000` (`CZboy`)

## Modifications Made to Upstream `CrispyZebra.h` (`dsp/CrispyZebra.h`)

Core Phase Distortion oscillator waveforms (`SAW`, `SQUARE`, `PULSE`, `DOUBLE_SINE`, `SAW_PULSE`, `REZ_SAW`, `REZ_TRI`, `REZ_TRAP`), Q16 8-stage `Envelope`, `LFO`, `Voice` detune/ring-mod/noise-mod routing, and soft clipper are preserved from upstream CrispyZebra (`db6af5c`), with targeted adaptations for Akai MPC ARMv7/ARM64 standalone playability and real-time performance:

1. **Authentic Unipolar Resonant Waveforms & Full CZ Phase Distortion (`Oscillator::nextRawSample`)**: Opened DCW scaling from `56000` to `64200`, switched resonant windows (`REZ_SAW`, `REZ_TRI`, `REZ_TRAP`, `REZ_PULSE`, `REZ_DBL_SAW`) to the real Casio uPD933 unipolar `(1 - cos(k*phi)) * window(phi)` formula across `1×..15×` (preserving full fundamental body alongside the resonant sweep, with per-note Nyquist guard `max_rez_dcw` and gentle upper-register key-follow), and added instant real-time `dcw_floor` and `dcw_env_scale` modulation.
2. **Post-Ring DCA, Stereo Voice Spread & Logarithmic Envelopes (`Voice::process`, `Engine::processBlockStereo`, `setEgRateDirect`)**: Separated raw oscillator evaluation (`nextRawSample`) from DCA multiplication so Ring Modulation preserves full sustain without `DCA²` attenuation, added subtle per-voice stereo panning in Poly modes (center in Mono), added direct Q16 envelope rate setting for smooth `3 ms – 6.6 s` ADSR sweeps (plus 12 s release cap for long ambient pads), and skipped inactive lines in single-line modes.
3. **Per-block hoisting & Zero-bias DC blocker (`Engine::processBlock`)**: Hoisted `pitch_bend_factor` (`__builtin_powf`), drive, volume, pan, and vibrato depth checks out of the per-sample loop; replaced integer-shift DC tracking with a denormal-flushed float one-pole DC blocker so idle output settles cleanly to `-200 dBFS` (`0.000000` RMS).
4. **64-bit pitch-envelope intermediate & voice polish**: Used 64-bit intermediate multiplication for DCO pitch-envelope modulation, added `dco_eg.keyOff()` in `Voice::noteOff()`, block-accurate `shouldRelease(numSamples, mode, ring_mod)`, zero-mean DCA-gated noise modulation, live waveform/detune updates for held notes, runtime polyphony cap (`Poly 8 / Poly 4 / Mono`), MIDI velocity scaling, sustain pedal (`CC 64`), mod-wheel vibrato (`CC 1`), and `allNotesOff()` / `forceSilence()`.
5. **MPC Host Adapter, Presets & UI (`dsp/CzBoyEngine.h`, `bridge/*`, `plugins/*`, `tools/*`)**: Newly authored GPL-3.0-or-later MPC VST2 wrapper, dual-tap CZ-5000 style stereo BBD chorus, 16 factory presets, procedural 1984 Casio CZ-101 hardware UI generator, and MPC `TUI.json` / `Q-Links.json` skin generator.

## SDK & Community Dependencies

- **Official Akai MPC Plugin Builder SDK** (`inmusic-software/mpc-plugin-builder-sdk`, MIT License):
  - Provides `framework/mpc_plugin.h`, `framework/mpc_vst2_abi.h`, `tools/build.py`, `tools/validate.py`, `tools/package.py`, and `host_sim`.
- **Open MPC Community Documentation** (`open-mpc/mpc-vst-plugins`, MIT License):
  - Referenced for MPC OS 3.10.1 hardware UI/TUI constraints and ARMv7 Cortex-A17 runtime guidelines.

## Trademark Disclaimer

Casio, CZ-101, CZ-1000, CZ-5000, Akai, MPC, and VST are registered trademarks of their respective owners. CZboy is an independent open-source project by Stampman3000 and is not affiliated with or endorsed by any trademark holder.
