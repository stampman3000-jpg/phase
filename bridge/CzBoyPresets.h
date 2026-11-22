// CZboy — 16 Iconic 1984 Casio CZ-101 Phase-Distortion Factory Presets.
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
        // 0: CZ-101 Brass (signature bright 80s dual-line brass swell; crisp release for clean host_sim silence)
        {
            "CZ-101 Brass",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 2; p.l1Wave2 = 5; p.modMode = 0;
                p.dcwTimbre = 0.24f; p.dcwEnv = 0.76f;
                p.dcwAtt = 0.14f; p.dcwDec = 0.42f; p.dcwSus = 0.48f; p.dcwRel = 0.22f;
                p.dcaAtt = 0.06f; p.dcaDec = 0.40f; p.dcaSus = 0.88f; p.dcaRel = 0.20f;
                p.chorus = 0.34f; p.volume = 0.82f;
                p.l2Wave1 = 0; p.l2Wave2 = 5; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 14;
                p.l2Mix = 0.90f; p.l2Dcw = 0.88f; p.l2EnvOfs = 0.16f;
                p.pitchEnv = 0.07f; p.pitchDec = 0.11f;
                p.vibRate = 0.34f; p.vibDepth = 0.08f; p.vibDelay = 0.38f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.52f;
                return p;
            }()
        },

        // 1: Glass Tine EP (warm Double-Sine body + sparkling octave-up fast-decay bell tine on Line 2!)
        {
            "Glass Tine EP",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 3; p.l1Wave2 = 7; p.modMode = 0;
                p.dcwTimbre = 0.14f; p.dcwEnv = 0.78f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.42f; p.dcwSus = 0.14f; p.dcwRel = 0.40f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.64f; p.dcaSus = 0.24f; p.dcaRel = 0.44f;
                p.chorus = 0.44f; p.volume = 0.86f;
                p.l2Wave1 = 5; p.l2Wave2 = 0; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 8;
                p.l2Mix = 0.72f; p.l2Dcw = 0.85f; p.l2EnvOfs = -0.68f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.15f;
                p.vibRate = 0.28f; p.vibDepth = 0.06f; p.vibDelay = 0.30f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.68f;
                return p;
            }()
        },

        // 2: Rez Acid Squelch (full 15x Casio resonant sweep without a filter! Mono + Glide)
        {
            "Rez Acid Squelch",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 5; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.12f; p.dcwEnv = 0.96f;
                p.dcwAtt = 0.02f; p.dcwDec = 0.32f; p.dcwSus = 0.12f; p.dcwRel = 0.24f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.44f; p.dcaSus = 0.78f; p.dcaRel = 0.22f;
                p.chorus = 0.14f; p.volume = 0.85f;
                p.l2Wave1 = 7; p.l2Wave2 = 0; p.l2Oct = -1; p.l2Semi = 0; p.l2Fine = 6;
                p.l2Mix = 0.78f; p.l2Dcw = 0.65f; p.l2EnvOfs = -0.15f;
                p.pitchEnv = 0.06f; p.pitchDec = 0.10f;
                p.vibRate = 0.36f; p.vibDepth = 0.0f; p.vibDelay = 0.25f;
                p.voiceMode = 2; p.glide = 0.22f; p.velSens = 0.55f;
                return p;
            }()
        },

        // 3: Slap Funk Bass (snappy Saw+Square pluck + sub-octave pulse punch)
        {
            "Slap Funk Bass",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 0; p.l1Wave2 = 2; p.modMode = 0;
                p.dcwTimbre = 0.18f; p.dcwEnv = 0.90f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.26f; p.dcwSus = 0.14f; p.dcwRel = 0.20f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.42f; p.dcaSus = 0.60f; p.dcaRel = 0.18f;
                p.chorus = 0.0f; p.volume = 0.86f;
                p.l2Wave1 = 2; p.l2Wave2 = 0; p.l2Oct = -1; p.l2Semi = 0; p.l2Fine = 5;
                p.l2Mix = 0.88f; p.l2Dcw = 0.58f; p.l2EnvOfs = -0.25f;
                p.pitchEnv = 0.16f; p.pitchDec = 0.08f;
                p.vibRate = 0.30f; p.vibDepth = 0.0f; p.vibDelay = 0.20f;
                p.voiceMode = 2; p.glide = 0.06f; p.velSens = 0.58f;
                return p;
            }()
        },

        // 4: Tubular Bells (post-ring DCA metallic bell — pitched body + shimmering inharmonic ring)
        {
            "Tubular Bells",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 3; p.l1Wave2 = 7; p.modMode = 1;
                p.dcwTimbre = 0.20f; p.dcwEnv = 0.82f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.56f; p.dcwSus = 0.18f; p.dcwRel = 0.58f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.72f; p.dcaSus = 0.12f; p.dcaRel = 0.62f;
                p.chorus = 0.32f; p.volume = 0.88f;
                p.l2Wave1 = 5; p.l2Wave2 = 0; p.l2Oct = 1; p.l2Semi = 7; p.l2Fine = 14;
                p.l2Mix = 0.92f; p.l2Dcw = 0.88f; p.l2EnvOfs = -0.35f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.16f;
                p.vibRate = 0.24f; p.vibDepth = 0.0f; p.vibDelay = 0.30f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.62f;
                return p;
            }()
        },

        // 5: 80s CZ Strings (lush dual-line detuned strings with delayed Line 2 bloom + wide chorus)
        {
            "80s CZ Strings",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 0; p.l1Wave2 = 5; p.modMode = 0;
                p.dcwTimbre = 0.34f; p.dcwEnv = 0.60f;
                p.dcwAtt = 0.42f; p.dcwDec = 0.55f; p.dcwSus = 0.70f; p.dcwRel = 0.56f;
                p.dcaAtt = 0.44f; p.dcaDec = 0.50f; p.dcaSus = 0.92f; p.dcaRel = 0.58f;
                p.chorus = 0.68f; p.volume = 0.80f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 18;
                p.l2Mix = 0.92f; p.l2Dcw = 0.82f; p.l2EnvOfs = 0.38f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibRate = 0.30f; p.vibDepth = 0.12f; p.vibDelay = 0.42f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.40f;
                return p;
            }()
        },

        // 6: Hollow Rez Pad (phase-inverted 1+2' sweeping Rez Trapezoid + slow Line 2 shimmer)
        {
            "Hollow Rez Pad",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 7; p.l1Wave2 = 7; p.modMode = 0;
                p.dcwTimbre = 0.16f; p.dcwEnv = 0.84f;
                p.dcwAtt = 0.52f; p.dcwDec = 0.64f; p.dcwSus = 0.56f; p.dcwRel = 0.62f;
                p.dcaAtt = 0.48f; p.dcaDec = 0.58f; p.dcaSus = 0.88f; p.dcaRel = 0.64f;
                p.chorus = 0.74f; p.volume = 0.84f;
                p.l2Wave1 = 5; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 22;
                p.l2Mix = 0.88f; p.l2Dcw = 0.78f; p.l2EnvOfs = 0.48f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibRate = 0.24f; p.vibDepth = 0.14f; p.vibDelay = 0.46f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.42f;
                return p;
            }()
        },

        // 7: Jazz Drawbar Org (Double-Sine + Pulse organ + fast percussive 2nd-harmonic key-click on L2)
        {
            "Jazz Drawbar Org",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 3; p.l1Wave2 = 3; p.modMode = 0;
                p.dcwTimbre = 0.52f; p.dcwEnv = 0.34f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.18f; p.dcwSus = 0.70f; p.dcwRel = 0.14f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.18f; p.dcaSus = 0.94f; p.dcaRel = 0.14f;
                p.chorus = 0.52f; p.volume = 0.82f;
                p.l2Wave1 = 1; p.l2Wave2 = 0; p.l2Oct = 1; p.l2Semi = 7; p.l2Fine = 4;
                p.l2Mix = 0.62f; p.l2Dcw = 0.70f; p.l2EnvOfs = -0.82f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.12f;
                p.vibRate = 0.44f; p.vibDepth = 0.10f; p.vibDelay = 0.12f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.20f;
                return p;
            }()
        },

        // 8: CZ Harpsichord (razor-bright Saw-Pulse + upper octave sparkling pluck)
        {
            "CZ Harpsichord",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 3; p.modMode = 0;
                p.dcwTimbre = 0.32f; p.dcwEnv = 0.68f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.34f; p.dcwSus = 0.12f; p.dcwRel = 0.24f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.48f; p.dcaSus = 0.08f; p.dcaRel = 0.24f;
                p.chorus = 0.16f; p.volume = 0.84f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 4;
                p.l2Mix = 0.74f; p.l2Dcw = 0.92f; p.l2EnvOfs = -0.45f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.12f;
                p.vibRate = 0.30f; p.vibDepth = 0.0f; p.vibDelay = 0.20f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.56f;
                return p;
            }()
        },

        // 9: Woody Marimba (classic CZ-101 resonant triangle mallet + 2-octave-up wooden knock on L2)
        {
            "Woody Marimba",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 6; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.14f; p.dcwEnv = 0.78f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.26f; p.dcwSus = 0.10f; p.dcwRel = 0.28f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.52f; p.dcaSus = 0.12f; p.dcaRel = 0.30f;
                p.chorus = 0.24f; p.volume = 0.90f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = 2; p.l2Semi = 0; p.l2Fine = 4;
                p.l2Mix = 0.78f; p.l2Dcw = 0.65f; p.l2EnvOfs = -0.68f;
                p.pitchEnv = 0.14f; p.pitchDec = 0.06f;
                p.vibRate = 0.30f; p.vibDepth = 0.0f; p.vibDelay = 0.20f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.72f;
                return p;
            }()
        },

        // 10: Cosmic Ring Pad (glassy sci-fi fifth-tuned Ring Mod pad with slow stereo movement)
        {
            "Cosmic Ring Pad",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 2; p.l1Wave2 = 4; p.modMode = 1;
                p.dcwTimbre = 0.22f; p.dcwEnv = 0.74f;
                p.dcwAtt = 0.48f; p.dcwDec = 0.62f; p.dcwSus = 0.62f; p.dcwRel = 0.60f;
                p.dcaAtt = 0.42f; p.dcaDec = 0.56f; p.dcaSus = 0.86f; p.dcaRel = 0.62f;
                p.chorus = 0.72f; p.volume = 0.84f;
                p.l2Wave1 = 6; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 7; p.l2Fine = 10;
                p.l2Mix = 0.82f; p.l2Dcw = 0.84f; p.l2EnvOfs = 0.42f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibRate = 0.22f; p.vibDepth = 0.16f; p.vibDelay = 0.40f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.48f;
                return p;
            }()
        },

        // 11: Funky CZ Clav (biting Ring-blended percussive clavinet — Prince/80s electro funk)
        {
            "Funky CZ Clav",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 2; p.l1Wave2 = 5; p.modMode = 1;
                p.dcwTimbre = 0.28f; p.dcwEnv = 0.72f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.28f; p.dcwSus = 0.14f; p.dcwRel = 0.20f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.36f; p.dcaSus = 0.18f; p.dcaRel = 0.18f;
                p.chorus = 0.14f; p.volume = 0.88f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 6;
                p.l2Mix = 0.76f; p.l2Dcw = 0.90f; p.l2EnvOfs = -0.35f;
                p.pitchEnv = 0.05f; p.pitchDec = 0.08f;
                p.vibRate = 0.32f; p.vibDepth = 0.0f; p.vibDelay = 0.20f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.68f;
                return p;
            }()
        },

        // 12: Breathy Pan Pipe (Noise-modulated Double Sine + Rez Tri breath chiff + delayed vibrato)
        {
            "Breathy Pan Pipe",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 3; p.l1Wave2 = 7; p.modMode = 2;
                p.dcwTimbre = 0.18f; p.dcwEnv = 0.56f;
                p.dcwAtt = 0.16f; p.dcwDec = 0.36f; p.dcwSus = 0.52f; p.dcwRel = 0.34f;
                p.dcaAtt = 0.18f; p.dcaDec = 0.34f; p.dcaSus = 0.84f; p.dcaRel = 0.34f;
                p.chorus = 0.38f; p.volume = 0.84f;
                p.l2Wave1 = 6; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 8;
                p.l2Mix = 0.74f; p.l2Dcw = 0.52f; p.l2EnvOfs = -0.40f;
                p.pitchEnv = 0.05f; p.pitchDec = 0.10f;
                p.vibRate = 0.36f; p.vibDepth = 0.18f; p.vibDelay = 0.34f;
                p.voiceMode = 1; p.glide = 0.12f; p.velSens = 0.56f;
                return p;
            }()
        },

        // 13: Detuned Synth Lead (biting 80s CZ mono glide lead with Saw-Pulse + Rez Saw edge)
        {
            "Detuned CZ Lead",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 2; p.modMode = 0;
                p.dcwTimbre = 0.44f; p.dcwEnv = 0.56f;
                p.dcwAtt = 0.02f; p.dcwDec = 0.34f; p.dcwSus = 0.58f; p.dcwRel = 0.24f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.36f; p.dcaSus = 0.88f; p.dcaRel = 0.24f;
                p.chorus = 0.28f; p.volume = 0.84f;
                p.l2Wave1 = 5; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 16;
                p.l2Mix = 0.88f; p.l2Dcw = 0.82f; p.l2EnvOfs = 0.0f;
                p.pitchEnv = 0.05f; p.pitchDec = 0.09f;
                p.vibRate = 0.38f; p.vibDepth = 0.14f; p.vibDelay = 0.28f;
                p.voiceMode = 2; p.glide = 0.26f; p.velSens = 0.50f;
                return p;
            }()
        },

        // 14: 80s Electro Tom (classic CZ laser/synth drum with big pitch drop + resonant sweep)
        {
            "80s Electro Tom",
            [] {
                SynthParams p;
                p.lineMode = 0; p.l1Wave1 = 5; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.06f; p.dcwEnv = 0.94f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.28f; p.dcwSus = 0.04f; p.dcwRel = 0.22f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.36f; p.dcaSus = 0.02f; p.dcaRel = 0.24f;
                p.chorus = 0.0f; p.volume = 0.88f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 0;
                p.l2Mix = 0.80f; p.l2Dcw = 0.80f; p.l2EnvOfs = 0.0f;
                p.pitchEnv = 0.82f; p.pitchDec = 0.24f;
                p.vibRate = 0.30f; p.vibDepth = 0.0f; p.vibDelay = 0.20f;
                p.voiceMode = 2; p.glide = 0.0f; p.velSens = 0.68f;
                return p;
            }()
        },

        // 15: Init CZ Saw (clean bright single-line CZ Saw starting patch)
        {
            "Init CZ Saw",
            [] {
                SynthParams p;
                p.lineMode = 0; p.l1Wave1 = 0; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.35f; p.dcwEnv = 0.60f;
                p.dcwAtt = 0.02f; p.dcwDec = 0.36f; p.dcwSus = 0.45f; p.dcwRel = 0.24f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.35f; p.dcaSus = 0.82f; p.dcaRel = 0.22f;
                p.chorus = 0.0f; p.volume = 0.84f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 10;
                p.l2Mix = 0.85f; p.l2Dcw = 0.90f; p.l2EnvOfs = 0.0f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.18f;
                p.vibRate = 0.32f; p.vibDepth = 0.0f; p.vibDelay = 0.25f;
                p.voiceMode = 0; p.glide = 0.0f; p.velSens = 0.45f;
                return p;
            }()
        },
    };

    outCount = (int) (sizeof (kPresets) / sizeof (kPresets[0]));
    return kPresets;
}

} // namespace czboy
