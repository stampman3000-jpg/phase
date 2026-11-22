# CZboy — Phase Distortion Synthesizer for Akai MPC

![CZboy](screenshots/czboy.png)

**CZboy** (`UID: CZBY`, vendor `Stampman3000`) is a standalone 1984 CZ-101-style Phase Distortion synthesizer for Akai MPC OS 3.10.1+ (32-bit ARM32 Cortex-A17 & 64-bit ARM64), based on the fixed-point C++ [**CrispyZebra**](https://github.com/hugelton/CrispyZebra) Phase Distortion engine by Leo Kuroshita (`@kurogedelic`).

Licensed under the **GNU General Public License v3.0 or later (GPL-3.0-or-later)** — see [LICENSE](LICENSE) and [NOTICE.md](NOTICE.md).

## Highlights
- **Unthrottled CrispyZebra Fixed-Point Phase Distortion Core (`dsp/CrispyZebra.h`, `dsp/CzBoyEngine.h`)**:
  - All 8 classic CZ waveforms (**Saw**, **Square**, **Pulse**, **Double Sine**, **Saw-Pulse**, **Rez Saw**, **Rez Tri**, **Rez Trap**) with authentic 2048-entry inverted-cosine LUT, full phase-distortion bend, screaming **1×–15×** resonant harmonic sweeps (with per-note Nyquist guard), and per-cycle **Wave 1 + Wave 2** alternation on both lines.
  - **Live Real-Time Timbre Control:** Dedicated **`DCW Timbre`** (instant live phase-distortion floor/cutoff) + **`DCW Env`** (live DCW envelope modulation depth) alongside full **DCW** and **DCA** ADSR envelopes.
  - **Dual-Line Layering & Post-Ring Modulation:** `Line 1`, `Line 2`, `1 + 2`, `1 + 2'` (phase-inverted), **Ring Mod** (with post-ring DCA staging so bells and clavs ring out cleanly), **Noise Mod**, **L2 Octave / Semi / Fine**, **`L2 Mix`**, **`L2 DCW`**, and bipolar **`L2 Env Ofs`** (turn negative for glassy bell/tine transients on Line 2, or positive for slow blooming pad layers).
  - **DCO Pitch Env**, **Triangle Vibrato LFO** (Rate / Depth / Delay + MIDI Mod Wheel CC1), **Portamento**, **Velocity Sensitivity**, Sustain Pedal (CC64), lush CZ-5000-style **Stereo BBD Chorus**, and CPU-safe **`Poly 8` / `Poly 4` / `Mono`** voice modes.
- **16 Classic CZ-101 Factory Presets (`bridge/CzBoyPresets.h`)**:
  - *CZ-101 Brass*, *Glass Tine EP*, *Rez Acid Squelch*, *Slap Funk Bass*, *Tubular Bells*, *80s CZ Strings*, *Hollow Rez Pad*, *Jazz Drawbar Org*, *CZ Harpsichord*, *Woody Marimba*, *Cosmic Ring Pad*, *Funky CZ Clav*, *Breathy Pan Pipe*, *Detuned CZ Lead*, *80s Electro Tom*, and *Init CZ Saw*.
- **1984 Casio CZ-101 Hardware Faceplate UI (`1280 × 628`, `tools/gen_ui_assets.py`)**:
  - Matte jet-charcoal ABS faceplate with hairline `DCO → DCW → DCA` schematic framing, brushed-silver chamfered green-grey LCD (2-line readout + live 8-waveform PD scope + stereo LCD meters), printed 8-waveform legend chart, flat matte-black CZ-style rotary dials, and compact Casio mini-sliders across 2 tabs (**`CZ-101`** and **`LINE 2/MOD`**) with 100% 4×4 hardware Q-Link coverage.
- **Roadmap (post-v1)**:
  - CZ `.syx` SysEx patch loading (`pdsynth`-style) and full 8-stage envelope point editing are planned for a later release, not v1.

## Desktop CPU Reference (44.1 kHz, 128-frame = 2902.5 µs)
*(On-device Gen1 MPC One ARMv7 benchmarks pending hardware capture.)*

| Scenario | Avg (µs) | p99 (µs) | Desktop % of 128-frame |
| :--- | ---: | ---: | ---: |
| Line 1 Only (4 voices, dry) | 5.0 µs | 6.3 µs | **0.17%** |
| Line 1 Only (8 voices, dry) | 7.9 µs | 13.9 µs | **0.27%** |
| Dual-Line 1+2 + Chorus (4 voices) | 9.9 µs | 17.9 µs | **0.34%** |
| Dual-Line 1+2 + Chorus (8 voices) | 14.5 µs | 25.2 µs | **0.50%** |
| Dual-Line Ring+Noise+Chorus (8 voices) | 11.0 µs | 18.8 µs | **0.38%** |

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
