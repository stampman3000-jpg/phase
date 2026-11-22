# CZboy — Phase Distortion Synthesizer for Akai MPC

![CZboy](screenshots/czboy.png)

**CZboy** (`UID: CZBY`, vendor `Stampman3000`) is a standalone 1980s CZ-style Phase Distortion synthesizer for Akai MPC OS 3.10.1+ (32-bit ARM32 Cortex-A17 & 64-bit ARM64), based on the fixed-point C++ [**CrispyZebra**](https://github.com/hugelton/CrispyZebra) Phase Distortion engine by Leo Kuroshita (`@kurogedelic`).

Licensed under the **GNU General Public License v3.0 or later (GPL-3.0-or-later)** — see [LICENSE](LICENSE) and [NOTICE.md](NOTICE.md).

## Highlights
- **CrispyZebra Fixed-Point Phase Distortion Core (`dsp/CrispyZebra.h`, `dsp/CzBoyEngine.h`)**:
  - All 8 classic CZ waveforms (**Saw**, **Square**, **Pulse**, **Double Sine**, **Saw-Pulse**, **Rez Saw**, **Rez Tri**, **Rez Trap**) with authentic per-cycle **Wave 1 + Wave 2** alternation on both lines.
  - Dual-line architecture (`Line 1`, `Line 2`, `1 + 2`, `1 + 2'` phase-inverted) with **Ring Modulation**, **Noise Modulation**, and independent **Line 2 Octave / Semitone / Fine Detune** + **L2 DCW** trim.
  - Musical **DCW** (timbre/phase-distortion depth + ADSR) and **DCA** (amplitude ADSR + selectable **Linear / Exp** curve) envelopes mapped onto CrispyZebra's Q16 8-stage envelope core, plus a **DCO Pitch Env** blip/sweep for classic CZ brass, bass, and electronic percussion.
  - **Vibrato LFO** (`Tri / Square / Saw Up / Saw Dn` with Rate, Depth, Delay, plus MIDI Mod Wheel CC1), **Portamento / Glide**, **Velocity Sensitivity**, Sustain Pedal (CC64), **Master Drive**, and a lightweight CZ-style **Stereo Chorus**.
  - **CPU-Safe Polyphony**: Selectable **`Poly 8`**, **`Poly 4`**, and **`Mono`** voice modes.
- **16 Factory Presets (`bridge/CzBoyPresets.h`)**:
  - *Init CZ Saw*, *CZ Brass Ens*, *Hollow Glass EP*, *Solid PD Bass*, *Metallic Bell*, *80s Synth Str*, *Rez Acid Lead*, *Drawbar Organ*, *CZ Harpsichord*, *Velvet PD Pad*, *Square Sub Bass*, *Cyber Ring Arp*, *Noise Flute*, *Marimba Pluck*, *Poly Rez Choir*, and *Zap Synth Tom*.
- **1984 Casio CZ Hardware Console UI (`1280 × 628`, `tools/gen_ui_assets.py`)**:
  - 2 clean tabs (**`CZBOY`** and **`LINE 2/MOD`**) covering all 32 automatable parameters 1:1 across 4×4 hardware Q-Link banks, backlit olive-mint dot-matrix LCD with a live 8-waveform Phase Distortion vector scope, stereo LCD peak meters, fluted rotary knobs, and vertical DCW/DCA ADSR faders.
- **Roadmap (post-v1)**:
  - CZ `.syx` SysEx patch loading (`pdsynth`-style) and full 8-stage envelope point editing are planned for a later release, not v1.

## Desktop CPU Reference (44.1 kHz, 128-frame = 2902.5 µs)
*(On-device Gen1 MPC One ARMv7 benchmarks pending hardware test; fixed-point CrispyZebra math with per-block hoisting and single-line skipping is built to stay well within Gen1 Cortex-A17 headroom even at 8 voices.)*

| Scenario | Avg (µs) | p99 (µs) | Desktop % of 128-frame |
| :--- | ---: | ---: | ---: |
| Line 1 Only (4 voices, dry) | 3.1 µs | 3.4 µs | **0.11%** |
| Line 1 Only (8 voices, dry) | 5.0 µs | 6.8 µs | **0.17%** |
| Dual-Line 1+2 + Chorus (4 voices) | 6.1 µs | 7.8 µs | **0.21%** |
| Dual-Line 1+2 + Chorus (8 voices) | 10.7 µs | 19.2 µs | **0.37%** |
| Dual-Line Ring+Noise+Chorus (8 voices) | 10.9 µs | 21.0 µs | **0.37%** |

## Build & Verify
Set `MPC_SDK` to a checkout of the official [Akai MPC Plugin Builder SDK](https://github.com/inmusic-software/mpc-plugin-builder-sdk), then:

```bash
bash tools/build_all.sh
bash tools/run_tests.sh
```

Deployable archive: `dist/czboy-1.0.0-mpc.zip` (extract `"Stampman3000 - VST - CZboy"` into `VST2 Plugins/` on your MPC drive).

## Credits
- **Synthesis Engine**: [CrispyZebra](https://github.com/hugelton/CrispyZebra) by Leo Kuroshita (`@kurogedelic`, GPL-3.0).
- **MPC Port, UI & Presets**: Stampman3000.

☕ If this saves you time, you can [buy me a coffee](https://paypal.me/johnnybarter1).
