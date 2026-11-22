// CZboy — 16 Handcrafted Casio CZ Phase-Distortion Factory Presets.
// Copyright (C) 2026 Stampman3000 — GNU General Public License v3.0 or later.
#pragma once

#include "../dsp/CzBoyEngine.h"

namespace czboy {

struct CzPreset
{
    const char* name;
    SynthParams p;
};

// Waveform indices:
//   Wave 1 (0..7):  0=Saw, 1=Square, 2=Pulse, 3=DblSin, 4=SawPls, 5=RezSaw, 6=RezTri, 7=RezTrp
//   Wave 2 (0..8):  0=Off, 1=Saw, 2=Square, 3=Pulse, 4=DblSin, 5=SawPls, 6=RezSaw, 7=RezTri, 8=RezTrp
//   Line Mode:      0=Line 1, 1=Line 2, 2=1+2, 3=1+2'
//   Mod Mode:       0=Off, 1=Ring, 2=Noise, 3=Ring+Noise
//   Voice Mode:     0=Poly 8, 1=Poly 4, 2=Mono

inline const CzPreset* factoryPresets (int& outCount)
{
    static const CzPreset kPresets[] = {
        // 0: Init CZ Saw (fast crisp release for instant playability & clean host_sim silence)
        {
            "Init CZ Saw",
            [] {
                SynthParams p;
                p.lineMode = 0; p.l1Wave1 = 0; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwDepth = 0.72f; p.dcwAtt = 0.02f; p.dcwDec = 0.36f; p.dcwSus = 0.45f; p.dcwRel = 0.25f;
                p.dcaAtt   = 0.01f; p.dcaDec = 0.35f; p.dcaSus = 0.80f; p.dcaRel = 0.22f;
                p.chorus = 0.0f; p.drive = 0.0f; p.volume = 0.82f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Dcw = 0.90f; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 8;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibWave = 0; p.vibRate = 0.32f; p.vibDepth = 0.0f; p.vibDelay = 0.20f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.45f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 1: CZ Brass Ens (classic CZ-101 brass swell: Pulse + Saw-Pulse, 1+2 detuned)
        {
            "CZ Brass Ens",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 2; p.l1Wave2 = 5; p.modMode = 0;
                p.dcwDepth = 0.92f; p.dcwAtt = 0.16f; p.dcwDec = 0.48f; p.dcwSus = 0.54f; p.dcwRel = 0.34f;
                p.dcaAtt   = 0.10f; p.dcaDec = 0.42f; p.dcaSus = 0.88f; p.dcaRel = 0.36f;
                p.chorus = 0.30f; p.drive = 0.12f; p.volume = 0.80f;
                p.l2Wave1 = 0; p.l2Wave2 = 5; p.l2Dcw = 0.85f; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 14;
                p.pitchEnv = 0.08f; p.pitchDec = 0.14f;
                p.vibWave = 0; p.vibRate = 0.34f; p.vibDepth = 0.08f; p.vibDelay = 0.35f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.55f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 2: Hollow Glass EP (CZ resonant triangle + double sine bell tine electric piano)
        {
            "Hollow Glass EP",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 6; p.l1Wave2 = 4; p.modMode = 0;
                p.dcwDepth = 0.78f; p.dcwAtt = 0.01f; p.dcwDec = 0.42f; p.dcwSus = 0.16f; p.dcwRel = 0.44f;
                p.dcaAtt   = 0.01f; p.dcaDec = 0.62f; p.dcaSus = 0.22f; p.dcaRel = 0.48f;
                p.chorus = 0.38f; p.drive = 0.05f; p.volume = 0.84f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Dcw = 0.65f; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 6;
                p.pitchEnv = 0.0f; p.pitchDec = 0.18f;
                p.vibWave = 0; p.vibRate = 0.28f; p.vibDepth = 0.06f; p.vibDelay = 0.30f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.65f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 3: Solid PD Bass (tight mono 80s phase-distortion bass)
        {
            "Solid PD Bass",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 0; p.l1Wave2 = 2; p.modMode = 0;
                p.dcwDepth = 0.94f; p.dcwAtt = 0.01f; p.dcwDec = 0.30f; p.dcwSus = 0.20f; p.dcwRel = 0.24f;
                p.dcaAtt   = 0.01f; p.dcaDec = 0.45f; p.dcaSus = 0.62f; p.dcaRel = 0.20f;
                p.chorus = 0.0f; p.drive = 0.28f; p.volume = 0.84f;
                p.l2Wave1 = 1; p.l2Wave2 = 0; p.l2Dcw = 0.72f; p.l2Oct = -1; p.l2Semi = 0; p.l2Fine = 5;
                p.pitchEnv = 0.14f; p.pitchDec = 0.10f;
                p.vibWave = 0; p.vibRate = 0.30f; p.vibDepth = 0.0f; p.vibDelay = 0.20f;
                p.voiceMode = 2; p.glide = 0.08f; p.velSens = 0.50f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 4: Metallic Bell (signature CZ ring-modulated tubular chime)
        {
            "Metallic Bell",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 3; p.l1Wave2 = 6; p.modMode = 1;
                p.dcwDepth = 0.82f; p.dcwAtt = 0.01f; p.dcwDec = 0.58f; p.dcwSus = 0.18f; p.dcwRel = 0.58f;
                p.dcaAtt   = 0.01f; p.dcaDec = 0.68f; p.dcaSus = 0.10f; p.dcaRel = 0.60f;
                p.chorus = 0.25f; p.drive = 0.0f; p.volume = 0.85f;
                p.l2Wave1 = 5; p.l2Wave2 = 0; p.l2Dcw = 0.80f; p.l2Oct = 1; p.l2Semi = 7; p.l2Fine = 11;
                p.pitchEnv = 0.0f; p.pitchDec = 0.18f;
                p.vibWave = 0; p.vibRate = 0.25f; p.vibDepth = 0.0f; p.vibDelay = 0.25f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.60f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 5: 80s Synth Strings (lush detuned Saw + Saw-Pulse with stereo BBD chorus)
        {
            "80s Synth Str",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 0; p.l1Wave2 = 5; p.modMode = 0;
                p.dcwDepth = 0.76f; p.dcwAtt = 0.44f; p.dcwDec = 0.55f; p.dcwSus = 0.68f; p.dcwRel = 0.56f;
                p.dcaAtt   = 0.48f; p.dcaDec = 0.50f; p.dcaSus = 0.90f; p.dcaRel = 0.58f;
                p.chorus = 0.62f; p.drive = 0.06f; p.volume = 0.78f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Dcw = 0.88f; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 18;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibWave = 0; p.vibRate = 0.30f; p.vibDepth = 0.10f; p.vibDelay = 0.40f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.40f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 6: Resonant Acid Lead (CZ REZ SAW filter-like sweep, mono glide)
        {
            "Rez Acid Lead",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 5; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwDepth = 0.96f; p.dcwAtt = 0.03f; p.dcwDec = 0.34f; p.dcwSus = 0.32f; p.dcwRel = 0.28f;
                p.dcaAtt   = 0.01f; p.dcaDec = 0.38f; p.dcaSus = 0.84f; p.dcaRel = 0.24f;
                p.chorus = 0.16f; p.drive = 0.36f; p.volume = 0.82f;
                p.l2Wave1 = 5; p.l2Wave2 = 1; p.l2Dcw = 0.92f; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 12;
                p.pitchEnv = 0.05f; p.pitchDec = 0.12f;
                p.vibWave = 0; p.vibRate = 0.38f; p.vibDepth = 0.12f; p.vibDelay = 0.28f;
                p.voiceMode = 2; p.glide = 0.26f; p.velSens = 0.50f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 7: Drawbar Organ (Double Sine + Pulse octave harmonic organ)
        {
            "Drawbar Organ",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 3; p.l1Wave2 = 3; p.modMode = 0;
                p.dcwDepth = 0.66f; p.dcwAtt = 0.01f; p.dcwDec = 0.18f; p.dcwSus = 0.85f; p.dcwRel = 0.14f;
                p.dcaAtt   = 0.01f; p.dcaDec = 0.18f; p.dcaSus = 0.94f; p.dcaRel = 0.14f;
                p.chorus = 0.42f; p.drive = 0.18f; p.volume = 0.80f;
                p.l2Wave1 = 2; p.l2Wave2 = 0; p.l2Dcw = 0.60f; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 5;
                p.pitchEnv = 0.0f; p.pitchDec = 0.15f;
                p.vibWave = 0; p.vibRate = 0.42f; p.vibDepth = 0.10f; p.vibDelay = 0.10f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.20f; p.dcaCurve = 0;
                return p;
            }()
        },

        // 8: Crisp Harpsichord (bright plucked Saw-Pulse + upper octave)
        {
            "CZ Harpsichord",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 3; p.modMode = 0;
                p.dcwDepth = 0.88f; p.dcwAtt = 0.01f; p.dcwDec = 0.34f; p.dcwSus = 0.12f; p.dcwRel = 0.24f;
                p.dcaAtt   = 0.01f; p.dcaDec = 0.46f; p.dcaSus = 0.06f; p.dcaRel = 0.22f;
                p.chorus = 0.12f; p.drive = 0.08f; p.volume = 0.82f;
                p.l2Wave1 = 2; p.l2Wave2 = 0; p.l2Dcw = 0.78f; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 3;
                p.pitchEnv = 0.0f; p.pitchDec = 0.15f;
                p.vibWave = 0; p.vibRate = 0.30f; p.vibDepth = 0.0f; p.vibDelay = 0.20f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.55f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 9: Velvet PD Pad (phase-inverted 1+2' Rez Trapezoid sweep + wide chorus)
        {
            "Velvet PD Pad",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 7; p.l1Wave2 = 1; p.modMode = 0;
                p.dcwDepth = 0.80f; p.dcwAtt = 0.56f; p.dcwDec = 0.64f; p.dcwSus = 0.58f; p.dcwRel = 0.62f;
                p.dcaAtt   = 0.52f; p.dcaDec = 0.60f; p.dcaSus = 0.86f; p.dcaRel = 0.64f;
                p.chorus = 0.70f; p.drive = 0.0f; p.volume = 0.82f;
                p.l2Wave1 = 6; p.l2Wave2 = 0; p.l2Dcw = 0.74f; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 22;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibWave = 0; p.vibRate = 0.22f; p.vibDepth = 0.14f; p.vibDelay = 0.45f;
                p.voiceMode = 1; p.glide = 0.0f; p.velSens = 0.45f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 10: Square Sub Bass (deep CZ square + sub octave, clean punch)
        {
            "Square Sub Bass",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 1; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwDepth = 0.68f; p.dcwAtt = 0.01f; p.dcwDec = 0.34f; p.dcwSus = 0.26f; p.dcwRel = 0.22f;
                p.dcaAtt   = 0.01f; p.dcaDec = 0.42f; p.dcaSus = 0.78f; p.dcaRel = 0.22f;
                p.chorus = 0.0f; p.drive = 0.22f; p.volume = 0.84f;
                p.l2Wave1 = 1; p.l2Wave2 = 0; p.l2Dcw = 0.42f; p.l2Oct = -1; p.l2Semi = 0; p.l2Fine = 4;
                p.pitchEnv = 0.0f; p.pitchDec = 0.14f;
                p.vibWave = 0; p.vibRate = 0.25f; p.vibDepth = 0.0f; p.vibDelay = 0.20f;
                p.voiceMode = 2; p.glide = 0.06f; p.velSens = 0.40f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 11: Cyber Ring Arp (short, snappy fifth-tuned ring-mod arp pluck)
        {
            "Cyber Ring Arp",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 2; p.l1Wave2 = 7; p.modMode = 1;
                p.dcwDepth = 0.88f; p.dcwAtt = 0.01f; p.dcwDec = 0.34f; p.dcwSus = 0.22f; p.dcwRel = 0.26f;
                p.dcaAtt   = 0.01f; p.dcaDec = 0.44f; p.dcaSus = 0.30f; p.dcaRel = 0.26f;
                p.chorus = 0.28f; p.drive = 0.24f; p.volume = 0.88f;
                p.l2Wave1 = 6; p.l2Wave2 = 0; p.l2Dcw = 0.85f; p.l2Oct = 0; p.l2Semi = 5; p.l2Fine = 8;
                p.pitchEnv = 0.06f; p.pitchDec = 0.10f;
                p.vibWave = 0; p.vibRate = 0.35f; p.vibDepth = 0.0f; p.vibDelay = 0.20f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.60f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 12: Noise Flute (breathy CZ noise-modulated double-sine wind instrument)
        {
            "Noise Flute",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 3; p.l1Wave2 = 7; p.modMode = 2;
                p.dcwDepth = 0.56f; p.dcwAtt = 0.20f; p.dcwDec = 0.38f; p.dcwSus = 0.55f; p.dcwRel = 0.36f;
                p.dcaAtt   = 0.22f; p.dcaDec = 0.35f; p.dcaSus = 0.82f; p.dcaRel = 0.34f;
                p.chorus = 0.32f; p.drive = 0.0f; p.volume = 0.82f;
                p.l2Wave1 = 6; p.l2Wave2 = 0; p.l2Dcw = 0.50f; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 7;
                p.pitchEnv = 0.04f; p.pitchDec = 0.12f;
                p.vibWave = 0; p.vibRate = 0.36f; p.vibDepth = 0.16f; p.vibDelay = 0.36f;
                p.voiceMode = 1; p.glide = 0.10f; p.velSens = 0.55f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 13: Marimba Pluck (woody resonant triangle mallet with quick pitch transient)
        {
            "Marimba Pluck",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 6; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwDepth = 0.70f; p.dcwAtt = 0.01f; p.dcwDec = 0.20f; p.dcwSus = 0.08f; p.dcwRel = 0.26f;
                p.dcaAtt   = 0.01f; p.dcaDec = 0.36f; p.dcaSus = 0.04f; p.dcaRel = 0.28f;
                p.chorus = 0.18f; p.drive = 0.05f; p.volume = 0.85f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Dcw = 0.52f; p.l2Oct = 2; p.l2Semi = 0; p.l2Fine = 4;
                p.pitchEnv = 0.12f; p.pitchDec = 0.07f;
                p.vibWave = 0; p.vibRate = 0.30f; p.vibDepth = 0.0f; p.vibDelay = 0.20f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.70f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 14: Poly Rez Choir (formant-like Rez Trap + Rez Tri vocal ensemble)
        {
            "Poly Rez Choir",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 7; p.l1Wave2 = 7; p.modMode = 0;
                p.dcwDepth = 0.74f; p.dcwAtt = 0.38f; p.dcwDec = 0.52f; p.dcwSus = 0.64f; p.dcwRel = 0.54f;
                p.dcaAtt   = 0.36f; p.dcaDec = 0.48f; p.dcaSus = 0.88f; p.dcaRel = 0.56f;
                p.chorus = 0.58f; p.drive = 0.04f; p.volume = 0.82f;
                p.l2Wave1 = 6; p.l2Wave2 = 0; p.l2Dcw = 0.66f; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 16;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibWave = 0; p.vibRate = 0.32f; p.vibDepth = 0.14f; p.vibDelay = 0.42f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.45f; p.dcaCurve = 1;
                return p;
            }()
        },

        // 15: Zap Synth Tom (80s electronic drum / laser tom using DCO pitch sweep)
        {
            "Zap Synth Tom",
            [] {
                SynthParams p;
                p.lineMode = 0; p.l1Wave1 = 5; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwDepth = 0.90f; p.dcwAtt = 0.01f; p.dcwDec = 0.28f; p.dcwSus = 0.05f; p.dcwRel = 0.22f;
                p.dcaAtt   = 0.01f; p.dcaDec = 0.34f; p.dcaSus = 0.02f; p.dcaRel = 0.24f;
                p.chorus = 0.0f; p.drive = 0.30f; p.volume = 0.86f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Dcw = 0.80f; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 0;
                p.pitchEnv = 0.78f; p.pitchDec = 0.24f;
                p.vibWave = 0; p.vibRate = 0.30f; p.vibDepth = 0.0f; p.vibDelay = 0.20f;
                p.voiceMode = 2; p.glide = 0.0f; p.velSens = 0.65f; p.dcaCurve = 1;
                return p;
            }()
        },
    };

    outCount = (int) (sizeof (kPresets) / sizeof (kPresets[0]));
    return kPresets;
}

} // namespace czboy
