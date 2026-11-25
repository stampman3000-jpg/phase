// CZboy — 32 Curated Presets:
//   Glassy Sines & Ambient Pads, 80s/90s Saws & Classics, Fat PD Basses,
//   CZ Keys/Bells & Synthy FX.
// Copyright (C) 2026 Stampman3000 — GNU General Public License v3.0 or later.
#pragma once

#include "../dsp/CzBoyEngine.h"

namespace czboy {

struct CzPreset
{
    const char* name;
    SynthParams p;
};

// Reference scales:
//   Wave 1 (0..7):  0=Saw, 1=Square, 2=Pulse, 3=DblSin, 4=SawPls, 5=RezSaw, 6=RezTri, 7=RezTrp
//   Wave 2 (0..8):  0=Off, 1=Saw, 2=Square, 3=Pulse, 4=DblSin, 5=SawPls, 6=RezSaw, 7=RezTri, 8=RezTrp
//   Line Mode:      0=Line 1, 1=Line 2, 2=1+2, 3=1-2
//   Mod Mode:       0=Off, 1=Ring, 2=Noise, 3=Ring+Noise
//   Voice Mode:     0=Poly 8, 1=Poly 4, 2=Mono
//   Env Time (0..1): 0.00=3ms, 0.25=20ms, 0.40=65ms, 0.50=140ms, 0.65=445ms, 0.78=1.2s, 0.88=2.6s, 0.96=4.8s

inline const CzPreset* factoryPresets (int& outCount)
{
    static const CzPreset kPresets[] = {
        // === GLASSY SINES & AMBIENT PADS (0..7) ===

        // 0: Glassy Sine (warm liquid Saw-Pulse sine + crystalline Double Sine halo + subtle vibrato, <0.45s rel for host_sim)
        {
            "Glassy Sine",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.18f; p.dcwEnv = 0.34f;
                p.dcwAtt = 0.05f; p.dcwDec = 0.46f; p.dcwSus = 0.16f; p.dcwRel = 0.38f;
                p.dcaAtt = 0.11f; p.dcaDec = 0.62f; p.dcaSus = 0.88f; p.dcaRel = 0.40f;
                p.chorus = 0.34f; p.volume = 0.88f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 7;
                p.l2Mix = 0.32f; p.l2Dcw = 0.24f; p.l2EnvOfs = 0.08f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.18f;
                p.vibRate = 0.52f; p.vibDepth = 0.16f; // Subtle silky sine + tape shimmer
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.0f; p.spaceDecay = 0.50f;
                return p;
            }()
        },

        // 1: Warm Tape Sine (warm hollow vintage sine-organ chord pad with gentle tape wow + Space)
        {
            "Warm Tape Sine",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 1; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.24f; p.dcwEnv = 0.28f;
                p.dcwAtt = 0.48f; p.dcwDec = 0.74f; p.dcwSus = 0.30f; p.dcwRel = 0.76f;
                p.dcaAtt = 0.44f; p.dcaDec = 0.76f; p.dcaSus = 0.90f; p.dcaRel = 0.78f;
                p.chorus = 0.56f; p.volume = 0.88f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 10;
                p.l2Mix = 0.38f; p.l2Dcw = 0.28f; p.l2EnvOfs = 0.28f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.22f;
                p.vibRate = 0.40f; p.vibDepth = 0.22f; // Tasteful slow tape pitch drift
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.48f; p.spaceDecay = 0.74f;
                return p;
            }()
        },

        // 2: Soft Sine Lead (warm, clean, slightly hollow glassy sine mono lead with glide & subtle vibrato)
        {
            "Soft Sine Lead",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 2; p.modMode = 0;
                p.dcwTimbre = 0.30f; p.dcwEnv = 0.34f;
                p.dcwAtt = 0.02f; p.dcwDec = 0.44f; p.dcwSus = 0.32f; p.dcwRel = 0.46f;
                p.dcaAtt = 0.04f; p.dcaDec = 0.64f; p.dcaSus = 0.88f; p.dcaRel = 0.48f;
                p.chorus = 0.26f; p.volume = 0.88f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 6;
                p.l2Mix = 0.28f; p.l2Dcw = 0.22f; p.l2EnvOfs = -0.10f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.14f;
                p.vibRate = 0.64f; p.vibDepth = 0.18f; // Gentle vintage synth vibrato
                p.voiceMode = 2; p.glide = 0.16f; p.spaceSend = 0.36f; p.spaceDecay = 0.60f;
                return p;
            }()
        },

        // 3: Crystal Sine Pad (pure crystalline glassy sine bell/pad with octave Double Sine chime & gentle drift)
        {
            "Crystal Sine Pad",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 0; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.10f; p.dcwEnv = 0.30f;
                p.dcwAtt = 0.08f; p.dcwDec = 0.64f; p.dcwSus = 0.14f; p.dcwRel = 0.82f;
                p.dcaAtt = 0.10f; p.dcaDec = 0.84f; p.dcaSus = 0.72f; p.dcaRel = 0.84f;
                p.chorus = 0.50f; p.volume = 0.88f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 8;
                p.l2Mix = 0.34f; p.l2Dcw = 0.22f; p.l2EnvOfs = -0.32f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibRate = 0.44f; p.vibDepth = 0.22f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.58f; p.spaceDecay = 0.80f;
                return p;
            }()
        },

        // 4: Hollow Tape Organ (nostalgic 70s/80s glassy flute/sine organ with gentle 16mm wow)
        {
            "Hollow Tape Organ",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 1; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.34f; p.dcwEnv = 0.24f;
                p.dcwAtt = 0.22f; p.dcwDec = 0.62f; p.dcwSus = 0.46f; p.dcwRel = 0.66f;
                p.dcaAtt = 0.20f; p.dcaDec = 0.64f; p.dcaSus = 0.90f; p.dcaRel = 0.68f;
                p.chorus = 0.48f; p.volume = 0.86f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 11;
                p.l2Mix = 0.46f; p.l2Dcw = 0.32f; p.l2EnvOfs = 0.16f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibRate = 0.34f; p.vibDepth = 0.24f; // Subtle 16mm tape drift
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.46f; p.spaceDecay = 0.72f;
                return p;
            }()
        },

        // 5: Ambient Phase Pad (dreamy phase-subtracted 1-2 Rez-Tri + Saw-Pulse cloud drenched in Space)
        {
            "Ambient Phase Pad",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 6; p.l1Wave2 = 5; p.modMode = 0;
                p.dcwTimbre = 0.18f; p.dcwEnv = 0.56f;
                p.dcwAtt = 0.80f; p.dcwDec = 0.88f; p.dcwSus = 0.48f; p.dcwRel = 0.86f;
                p.dcaAtt = 0.74f; p.dcaDec = 0.84f; p.dcaSus = 0.88f; p.dcaRel = 0.86f;
                p.chorus = 0.68f; p.volume = 0.84f;
                p.l2Wave1 = 7; p.l2Wave2 = 5; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 14;
                p.l2Mix = 0.88f; p.l2Dcw = 0.64f; p.l2EnvOfs = 0.40f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.30f;
                p.vibRate = 0.38f; p.vibDepth = 0.16f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.56f; p.spaceDecay = 0.78f;
                return p;
            }()
        },

        // 6: Fragile Glass Pad (warm Saw-Pulse bed + slow octave Double Sine shimmer in Space)
        {
            "Fragile Glass Pad",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.12f; p.dcwEnv = 0.42f;
                p.dcwAtt = 0.84f; p.dcwDec = 0.90f; p.dcwSus = 0.34f; p.dcwRel = 0.90f;
                p.dcaAtt = 0.76f; p.dcaDec = 0.88f; p.dcaSus = 0.86f; p.dcaRel = 0.90f;
                p.chorus = 0.64f; p.volume = 0.86f;
                p.l2Wave1 = 3; p.l2Wave2 = 7; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 8;
                p.l2Mix = 0.50f; p.l2Dcw = 0.40f; p.l2EnvOfs = 0.54f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.30f;
                p.vibRate = 0.34f; p.vibDepth = 0.18f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.62f; p.spaceDecay = 0.84f;
                return p;
            }()
        },

        // 7: Deep Drift Drone (deep hypnotic slow-breathing sub-octave 1-2 ambient drone pad in cavernous Space)
        {
            "Deep Drift Drone",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 7; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.14f; p.dcwEnv = 0.44f;
                p.dcwAtt = 0.88f; p.dcwDec = 0.94f; p.dcwSus = 0.46f; p.dcwRel = 0.92f;
                p.dcaAtt = 0.72f; p.dcaDec = 0.92f; p.dcaSus = 0.90f; p.dcaRel = 0.92f;
                p.chorus = 0.72f; p.volume = 0.86f;
                p.l2Wave1 = 4; p.l2Wave2 = 7; p.l2Oct = -1; p.l2Semi = 0; p.l2Fine = 10;
                p.l2Mix = 0.90f; p.l2Dcw = 0.42f; p.l2EnvOfs = 0.42f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.30f;
                p.vibRate = 0.30f; p.vibDepth = 0.14f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.68f; p.spaceDecay = 0.88f;
                return p;
            }()
        },

        // === 80s / 90s SAWS, BRASS, STRINGS & PADS (8..14) ===

        // 8: 80s CZ Poly Saw (lush detuned dual-Saw 1984 polysynth — warm, wide, glossy 80s chord machine)
        {
            "80s CZ Poly Saw",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 0; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.46f; p.dcwEnv = 0.48f;
                p.dcwAtt = 0.14f; p.dcwDec = 0.68f; p.dcwSus = 0.56f; p.dcwRel = 0.58f;
                p.dcaAtt = 0.06f; p.dcaDec = 0.72f; p.dcaSus = 0.86f; p.dcaRel = 0.58f;
                p.chorus = 0.54f; p.volume = 0.86f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 14;
                p.l2Mix = 0.92f; p.l2Dcw = 0.90f; p.l2EnvOfs = 0.12f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.18f;
                p.vibRate = 0.54f; p.vibDepth = 0.10f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.24f; p.spaceDecay = 0.52f;
                return p;
            }()
        },

        // 9: 90s Rave Saw Stab (punchy early-90s UK rave / house detuned Saw+Pulse chord stab with tight echo)
        {
            "90s Rave Saw Stab",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 0; p.l1Wave2 = 3; p.modMode = 0;
                p.dcwTimbre = 0.44f; p.dcwEnv = 0.56f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.46f; p.dcwSus = 0.22f; p.dcwRel = 0.42f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.54f; p.dcaSus = 0.34f; p.dcaRel = 0.42f;
                p.chorus = 0.46f; p.volume = 0.88f;
                p.l2Wave1 = 0; p.l2Wave2 = 5; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 22;
                p.l2Mix = 0.94f; p.l2Dcw = 0.94f; p.l2EnvOfs = -0.14f;
                p.pitchEnv = 0.06f; p.pitchDec = 0.10f;
                p.vibRate = 0.48f; p.vibDepth = 0.0f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.32f; p.spaceDecay = 0.52f;
                return p;
            }()
        },

        // 10: 80s Synthwave Saw (neon sunset detuned Saw + Saw-Pulse dreamy 80s pad/keys hybrid)
        {
            "80s Synthwave Saw",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 0; p.l1Wave2 = 5; p.modMode = 0;
                p.dcwTimbre = 0.36f; p.dcwEnv = 0.54f;
                p.dcwAtt = 0.32f; p.dcwDec = 0.74f; p.dcwSus = 0.54f; p.dcwRel = 0.70f;
                p.dcaAtt = 0.24f; p.dcaDec = 0.76f; p.dcaSus = 0.88f; p.dcaRel = 0.72f;
                p.chorus = 0.60f; p.volume = 0.86f;
                p.l2Wave1 = 4; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 13;
                p.l2Mix = 0.90f; p.l2Dcw = 0.84f; p.l2EnvOfs = 0.20f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibRate = 0.48f; p.vibDepth = 0.12f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.38f; p.spaceDecay = 0.64f;
                return p;
            }()
        },

        // 11: 90s Detuned Lead (biting 90s techno/rave mono glide Saw + Rez Saw lead with Chorus + Space)
        {
            "90s Detuned Lead",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 0; p.l1Wave2 = 5; p.modMode = 0;
                p.dcwTimbre = 0.56f; p.dcwEnv = 0.44f;
                p.dcwAtt = 0.08f; p.dcwDec = 0.58f; p.dcwSus = 0.66f; p.dcwRel = 0.48f;
                p.dcaAtt = 0.04f; p.dcaDec = 0.62f; p.dcaSus = 0.88f; p.dcaRel = 0.46f;
                p.chorus = 0.38f; p.volume = 0.86f;
                p.l2Wave1 = 5; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 18;
                p.l2Mix = 0.90f; p.l2Dcw = 0.86f; p.l2EnvOfs = 0.0f;
                p.pitchEnv = 0.05f; p.pitchDec = 0.14f;
                p.vibRate = 0.60f; p.vibDepth = 0.14f;
                p.voiceMode = 2; p.glide = 0.22f; p.spaceSend = 0.32f; p.spaceDecay = 0.56f;
                return p;
            }()
        },

        // 12: CZ-101 Brass Ens (rich, luminous 1984 Casio CZ-101 Brass Ensemble — Pulse + Saw-Pulse)
        {
            "CZ-101 Brass Ens",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 2; p.l1Wave2 = 5; p.modMode = 0;
                p.dcwTimbre = 0.28f; p.dcwEnv = 0.68f;
                p.dcwAtt = 0.42f; p.dcwDec = 0.68f; p.dcwSus = 0.54f; p.dcwRel = 0.50f;
                p.dcaAtt = 0.30f; p.dcaDec = 0.66f; p.dcaSus = 0.88f; p.dcaRel = 0.50f;
                p.chorus = 0.44f; p.volume = 0.86f;
                p.l2Wave1 = 0; p.l2Wave2 = 5; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 12;
                p.l2Mix = 0.90f; p.l2Dcw = 0.86f; p.l2EnvOfs = 0.16f;
                p.pitchEnv = 0.05f; p.pitchDec = 0.20f;
                p.vibRate = 0.52f; p.vibDepth = 0.08f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.22f; p.spaceDecay = 0.50f;
                return p;
            }()
        },

        // 13: Warm CZ Strings (silky 1984 Casio CZ-101 String Ensemble — clean detuned Saw + Saw-Pulse)
        {
            "Warm CZ Strings",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 0; p.l1Wave2 = 5; p.modMode = 0;
                p.dcwTimbre = 0.38f; p.dcwEnv = 0.48f;
                p.dcwAtt = 0.70f; p.dcwDec = 0.78f; p.dcwSus = 0.66f; p.dcwRel = 0.76f;
                p.dcaAtt = 0.68f; p.dcaDec = 0.76f; p.dcaSus = 0.92f; p.dcaRel = 0.78f;
                p.chorus = 0.66f; p.volume = 0.84f;
                p.l2Wave1 = 4; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 13;
                p.l2Mix = 0.92f; p.l2Dcw = 0.82f; p.l2EnvOfs = 0.28f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.25f;
                p.vibRate = 0.50f; p.vibDepth = 0.10f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.36f; p.spaceDecay = 0.62f;
                return p;
            }()
        },

        // 14: Velvet 1-2 Pad (phase-subtracted 1-2 warm hollow PWM glassy pad + chorus)
        {
            "Velvet 1-2 Pad",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 4; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.32f; p.dcwEnv = 0.48f;
                p.dcwAtt = 0.68f; p.dcwDec = 0.80f; p.dcwSus = 0.58f; p.dcwRel = 0.78f;
                p.dcaAtt = 0.62f; p.dcaDec = 0.78f; p.dcaSus = 0.90f; p.dcaRel = 0.80f;
                p.chorus = 0.58f; p.volume = 0.86f;
                p.l2Wave1 = 4; p.l2Wave2 = 4; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 8;
                p.l2Mix = 0.96f; p.l2Dcw = 0.84f; p.l2EnvOfs = 0.22f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibRate = 0.44f; p.vibDepth = 0.12f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.44f; p.spaceDecay = 0.70f;
                return p;
            }()
        },

        // === FAT PD BASSES, ACID & ARPS (15..20) ===

        // 15: Fat Rez Bass (massive unipolar Rez Saw + sub-octave Square — fat bass fundamental + resonant pluck)
        {
            "Fat Rez Bass",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 5; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.16f; p.dcwEnv = 0.92f;
                p.dcwAtt = 0.02f; p.dcwDec = 0.56f; p.dcwSus = 0.16f; p.dcwRel = 0.44f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.70f; p.dcaSus = 0.72f; p.dcaRel = 0.38f;
                p.chorus = 0.0f; p.volume = 0.88f;
                p.l2Wave1 = 1; p.l2Wave2 = 0; p.l2Oct = -1; p.l2Semi = 0; p.l2Fine = 5;
                p.l2Mix = 0.86f; p.l2Dcw = 0.56f; p.l2EnvOfs = -0.18f;
                p.pitchEnv = 0.12f; p.pitchDec = 0.16f;
                p.vibRate = 0.30f; p.vibDepth = 0.0f;
                p.voiceMode = 2; p.glide = 0.08f; p.spaceSend = 0.0f; p.spaceDecay = 0.35f;
                return p;
            }()
        },

        // 16: Acid Rez Squelch (303-meets-CZ-101 exponential resonant acid mono line with glide + tight echo)
        {
            "Acid Rez Squelch",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 5; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.10f; p.dcwEnv = 0.98f;
                p.dcwAtt = 0.05f; p.dcwDec = 0.54f; p.dcwSus = 0.08f; p.dcwRel = 0.42f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.66f; p.dcaSus = 0.78f; p.dcaRel = 0.38f;
                p.chorus = 0.08f; p.volume = 0.88f;
                p.l2Wave1 = 7; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 9;
                p.l2Mix = 0.72f; p.l2Dcw = 0.88f; p.l2EnvOfs = -0.10f;
                p.pitchEnv = 0.06f; p.pitchDec = 0.14f;
                p.vibRate = 0.34f; p.vibDepth = 0.0f;
                p.voiceMode = 2; p.glide = 0.24f; p.spaceSend = 0.22f; p.spaceDecay = 0.48f;
                return p;
            }()
        },

        // 17: Electro Slap Bs (80s electro rubbery Saw+Square + sub-octave funk punch)
        {
            "Electro Slap Bs",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 0; p.l1Wave2 = 2; p.modMode = 0;
                p.dcwTimbre = 0.26f; p.dcwEnv = 0.84f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.48f; p.dcwSus = 0.20f; p.dcwRel = 0.38f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.64f; p.dcaSus = 0.58f; p.dcaRel = 0.34f;
                p.chorus = 0.0f; p.volume = 0.88f;
                p.l2Wave1 = 2; p.l2Wave2 = 0; p.l2Oct = -1; p.l2Semi = 0; p.l2Fine = 4;
                p.l2Mix = 0.90f; p.l2Dcw = 0.64f; p.l2EnvOfs = -0.22f;
                p.pitchEnv = 0.18f; p.pitchDec = 0.14f;
                p.vibRate = 0.30f; p.vibDepth = 0.0f;
                p.voiceMode = 2; p.glide = 0.04f; p.spaceSend = 0.0f; p.spaceDecay = 0.35f;
                return p;
            }()
        },

        // 18: Deep Sub Pulse (weighty UK rave / BoC sub-octave Pulse + Double Sine warm sub bass)
        {
            "Deep Sub Pulse",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 2; p.l1Wave2 = 4; p.modMode = 0;
                p.dcwTimbre = 0.20f; p.dcwEnv = 0.64f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.58f; p.dcwSus = 0.30f; p.dcwRel = 0.44f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.72f; p.dcaSus = 0.84f; p.dcaRel = 0.40f;
                p.chorus = 0.0f; p.volume = 0.90f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Oct = -1; p.l2Semi = 0; p.l2Fine = 3;
                p.l2Mix = 0.96f; p.l2Dcw = 0.24f; p.l2EnvOfs = 0.0f;
                p.pitchEnv = 0.08f; p.pitchDec = 0.14f;
                p.vibRate = 0.26f; p.vibDepth = 0.0f;
                p.voiceMode = 2; p.glide = 0.10f; p.spaceSend = 0.0f; p.spaceDecay = 0.30f;
                return p;
            }()
        },

        // 19: Hollow House Bs (90s UK garage / house bouncy Rez Tri + octave-down punch)
        {
            "Hollow House Bs",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 6; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.14f; p.dcwEnv = 0.84f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.50f; p.dcwSus = 0.10f; p.dcwRel = 0.40f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.64f; p.dcaSus = 0.34f; p.dcaRel = 0.36f;
                p.chorus = 0.12f; p.volume = 0.88f;
                p.l2Wave1 = 1; p.l2Wave2 = 0; p.l2Oct = -1; p.l2Semi = 0; p.l2Fine = 6;
                p.l2Mix = 0.82f; p.l2Dcw = 0.44f; p.l2EnvOfs = -0.14f;
                p.pitchEnv = 0.06f; p.pitchDec = 0.12f;
                p.vibRate = 0.30f; p.vibDepth = 0.0f;
                p.voiceMode = 2; p.glide = 0.05f; p.spaceSend = 0.10f; p.spaceDecay = 0.35f;
                return p;
            }()
        },

        // 20: Rubber Techno Arp (snappy Saw-Pulse + Rez Saw stereo ping-pong sequence bass/arp)
        {
            "Rubber Techno Arp",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 6; p.modMode = 0;
                p.dcwTimbre = 0.24f; p.dcwEnv = 0.84f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.44f; p.dcwSus = 0.14f; p.dcwRel = 0.40f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.56f; p.dcaSus = 0.18f; p.dcaRel = 0.38f;
                p.chorus = 0.18f; p.volume = 0.86f;
                p.l2Wave1 = 5; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 11;
                p.l2Mix = 0.84f; p.l2Dcw = 0.82f; p.l2EnvOfs = -0.24f;
                p.pitchEnv = 0.08f; p.pitchDec = 0.10f;
                p.vibRate = 0.32f; p.vibDepth = 0.0f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.38f; p.spaceDecay = 0.56f;
                return p;
            }()
        },

        // === CLASSIC CZ KEYS, BELLS & PLUCKED (21..26) ===

        // 21: Crystal Tine EP (authentic siney vintage CZ electric piano — buttery Saw-Pulse body + isolated Rez-Tri tine)
        {
            "Crystal Tine EP",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.14f; p.dcwEnv = 0.44f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.56f; p.dcwSus = 0.12f; p.dcwRel = 0.66f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.86f; p.dcaSus = 0.26f; p.dcaRel = 0.70f;
                p.chorus = 0.34f; p.volume = 0.88f;
                p.l2Wave1 = 6; p.l2Wave2 = 0; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 6;
                p.l2Mix = 0.36f; p.l2Dcw = 0.48f; p.l2EnvOfs = -0.62f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.16f;
                p.vibRate = 0.52f; p.vibDepth = 0.12f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.26f; p.spaceDecay = 0.58f;
                return p;
            }()
        },

        // 22: Metallic Clav (Ring-blended percussive CZ clavinet — snappy funk bite)
        {
            "Metallic Clav",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 2; p.l1Wave2 = 5; p.modMode = 1;
                p.dcwTimbre = 0.34f; p.dcwEnv = 0.70f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.48f; p.dcwSus = 0.18f; p.dcwRel = 0.38f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.58f; p.dcaSus = 0.16f; p.dcaRel = 0.34f;
                p.chorus = 0.16f; p.volume = 0.88f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 7;
                p.l2Mix = 0.78f; p.l2Dcw = 0.90f; p.l2EnvOfs = -0.30f;
                p.pitchEnv = 0.04f; p.pitchDec = 0.10f;
                p.vibRate = 0.32f; p.vibDepth = 0.0f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.14f; p.spaceDecay = 0.36f;
                return p;
            }()
        },

        // 23: Tubular Chimes (CZ Ring Mod metallic bell — long shimmering ring in Space)
        {
            "Tubular Chimes",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 3; p.l1Wave2 = 7; p.modMode = 1;
                p.dcwTimbre = 0.22f; p.dcwEnv = 0.74f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.84f; p.dcwSus = 0.18f; p.dcwRel = 0.84f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.92f; p.dcaSus = 0.10f; p.dcaRel = 0.88f;
                p.chorus = 0.34f; p.volume = 0.86f;
                p.l2Wave1 = 5; p.l2Wave2 = 0; p.l2Oct = 1; p.l2Semi = 7; p.l2Fine = 14;
                p.l2Mix = 0.92f; p.l2Dcw = 0.86f; p.l2EnvOfs = -0.32f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.16f;
                p.vibRate = 0.22f; p.vibDepth = 0.0f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.48f; p.spaceDecay = 0.74f;
                return p;
            }()
        },

        // 24: Woody Marimba (warm resonant triangle + 2-octave wooden mallet knock)
        {
            "Woody Marimba",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 6; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.14f; p.dcwEnv = 0.74f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.46f; p.dcwSus = 0.08f; p.dcwRel = 0.56f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.68f; p.dcaSus = 0.06f; p.dcaRel = 0.58f;
                p.chorus = 0.20f; p.volume = 0.88f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = 2; p.l2Semi = 0; p.l2Fine = 4;
                p.l2Mix = 0.68f; p.l2Dcw = 0.58f; p.l2EnvOfs = -0.66f;
                p.pitchEnv = 0.12f; p.pitchDec = 0.10f;
                p.vibRate = 0.28f; p.vibDepth = 0.0f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.28f; p.spaceDecay = 0.50f;
                return p;
            }()
        },

        // 25: Bright Harpsi (crisp plucked Saw-Pulse + octave chime harpsichord)
        {
            "Bright Harpsi",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 3; p.modMode = 0;
                p.dcwTimbre = 0.46f; p.dcwEnv = 0.54f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.64f; p.dcwSus = 0.22f; p.dcwRel = 0.50f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.76f; p.dcaSus = 0.08f; p.dcaRel = 0.48f;
                p.chorus = 0.16f; p.volume = 0.84f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 4;
                p.l2Mix = 0.72f; p.l2Dcw = 0.92f; p.l2EnvOfs = -0.38f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.14f;
                p.vibRate = 0.30f; p.vibDepth = 0.0f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.22f; p.spaceDecay = 0.46f;
                return p;
            }()
        },

        // 26: Light Harp Pluck (glassy Ring Mod arpeggio harp floating in Space)
        {
            "Light Harp Pluck",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 0; p.modMode = 1;
                p.dcwTimbre = 0.20f; p.dcwEnv = 0.70f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.66f; p.dcwSus = 0.20f; p.dcwRel = 0.70f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.78f; p.dcaSus = 0.14f; p.dcaRel = 0.72f;
                p.chorus = 0.42f; p.volume = 0.86f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 6;
                p.l2Mix = 0.60f; p.l2Dcw = 0.64f; p.l2EnvOfs = -0.35f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.16f;
                p.vibRate = 0.26f; p.vibDepth = 0.0f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.46f; p.spaceDecay = 0.68f;
                return p;
            }()
        },

        // === WIND, SYNTHY FX, DRUMS & INIT (27..31) ===

        // 27: Breathy Pan Pipe (pitched LFSR Noise chiff flute + gentle vibrato + ambient echo)
        {
            "Breathy Pan Pipe",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 3; p.l1Wave2 = 7; p.modMode = 2;
                p.dcwTimbre = 0.22f; p.dcwEnv = 0.46f;
                p.dcwAtt = 0.46f; p.dcwDec = 0.64f; p.dcwSus = 0.48f; p.dcwRel = 0.62f;
                p.dcaAtt = 0.48f; p.dcaDec = 0.62f; p.dcaSus = 0.84f; p.dcaRel = 0.64f;
                p.chorus = 0.40f; p.volume = 0.84f;
                p.l2Wave1 = 6; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 8;
                p.l2Mix = 0.74f; p.l2Dcw = 0.46f; p.l2EnvOfs = -0.42f;
                p.pitchEnv = 0.05f; p.pitchDec = 0.18f;
                p.vibRate = 0.56f; p.vibDepth = 0.14f;
                p.voiceMode = 1; p.glide = 0.14f; p.spaceSend = 0.46f; p.spaceDecay = 0.66f;
                return p;
            }()
        },

        // 28: Sci-Fi Ring Zaps (inharmonic Ring Mod + big pitch drop laser/alien bleeps in Space)
        {
            "Sci-Fi Ring Zaps",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 5; p.l1Wave2 = 3; p.modMode = 1;
                p.dcwTimbre = 0.24f; p.dcwEnv = 0.86f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.58f; p.dcwSus = 0.12f; p.dcwRel = 0.52f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.64f; p.dcaSus = 0.08f; p.dcaRel = 0.54f;
                p.chorus = 0.30f; p.volume = 0.86f;
                p.l2Wave1 = 6; p.l2Wave2 = 2; p.l2Oct = 1; p.l2Semi = 6; p.l2Fine = 35;
                p.l2Mix = 1.0f; p.l2Dcw = 0.94f; p.l2EnvOfs = 0.25f;
                p.pitchEnv = 0.92f; p.pitchDec = 0.52f;
                p.vibRate = 0.72f; p.vibDepth = 0.26f;
                p.voiceMode = 0; p.glide = 0.18f; p.spaceSend = 0.54f; p.spaceDecay = 0.72f;
                return p;
            }()
        },

        // 29: Noise Wind Sweep (pitched LFSR Noise Mod + sweeping Rez Trap ambient storm in Space)
        {
            "Noise Wind Sweep",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 7; p.l1Wave2 = 6; p.modMode = 2;
                p.dcwTimbre = 0.12f; p.dcwEnv = 0.92f;
                p.dcwAtt = 0.86f; p.dcwDec = 0.90f; p.dcwSus = 0.36f; p.dcwRel = 0.88f;
                p.dcaAtt = 0.68f; p.dcaDec = 0.86f; p.dcaSus = 0.80f; p.dcaRel = 0.88f;
                p.chorus = 0.74f; p.volume = 0.84f;
                p.l2Wave1 = 5; p.l2Wave2 = 0; p.l2Oct = -1; p.l2Semi = 5; p.l2Fine = 28;
                p.l2Mix = 0.90f; p.l2Dcw = 0.92f; p.l2EnvOfs = 0.50f;
                p.pitchEnv = 0.18f; p.pitchDec = 0.80f;
                p.vibRate = 0.28f; p.vibDepth = 0.18f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.68f; p.spaceDecay = 0.86f;
                return p;
            }()
        },

        // 30: Electro Synth Tom (classic 80s CZ laser tom / pitch-drop drum)
        {
            "Electro Synth Tom",
            [] {
                SynthParams p;
                p.lineMode = 0; p.l1Wave1 = 5; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.06f; p.dcwEnv = 0.94f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.48f; p.dcwSus = 0.04f; p.dcwRel = 0.44f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.58f; p.dcaSus = 0.02f; p.dcaRel = 0.44f;
                p.chorus = 0.0f; p.volume = 0.88f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 0;
                p.l2Mix = 0.80f; p.l2Dcw = 0.80f; p.l2EnvOfs = 0.0f;
                p.pitchEnv = 0.84f; p.pitchDec = 0.44f;
                p.vibRate = 0.30f; p.vibDepth = 0.0f;
                p.voiceMode = 2; p.glide = 0.0f; p.spaceSend = 0.20f; p.spaceDecay = 0.44f;
                return p;
            }()
        },

        // 31: Init CZ Saw (clean bright single-line CZ Saw starting point)
        {
            "Init CZ Saw",
            [] {
                SynthParams p;
                p.lineMode = 0; p.l1Wave1 = 0; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.50f; p.dcwEnv = 0.45f;
                p.dcwAtt = 0.04f; p.dcwDec = 0.56f; p.dcwSus = 0.52f; p.dcwRel = 0.46f;
                p.dcaAtt = 0.02f; p.dcaDec = 0.56f; p.dcaSus = 0.84f; p.dcaRel = 0.42f;
                p.chorus = 0.0f; p.volume = 0.84f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 10;
                p.l2Mix = 0.85f; p.l2Dcw = 0.90f; p.l2EnvOfs = 0.0f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.25f;
                p.vibRate = 0.32f; p.vibDepth = 0.0f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.0f; p.spaceDecay = 0.50f;
                return p;
            }()
        },
    };

    outCount = (int) (sizeof (kPresets) / sizeof (kPresets[0]));
    return kPresets;
}

} // namespace czboy
