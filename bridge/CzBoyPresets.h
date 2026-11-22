// CZboy — 32 Handcrafted & CZ-101 Converted Presets:
//   Aphex Ambient Pads, Fat Acid/Rave PD Basses, Converted CZ-101 Classics & Synthy Noises.
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
//   Line Mode:      0=Line 1, 1=Line 2, 2=1+2, 3=1+2'
//   Mod Mode:       0=Off, 1=Ring, 2=Noise, 3=Ring+Noise
//   Voice Mode:     0=Poly 8, 1=Poly 4, 2=Mono
//   Env Time (0..1): 0.00=3ms, 0.25=20ms, 0.40=65ms, 0.50=140ms, 0.65=445ms, 0.78=1.2s, 0.88=2.6s, 0.96=4.8s

inline const CzPreset* factoryPresets (int& outCount)
{
    static const CzPreset kPresets[] = {
        // === BOARDS OF CANADA GLASSY SINES & VIBRATO + APHEX PADS (0..8) ===

        // 0: BoC Glassy Sine (warm liquid Saw-Pulse sine + crystalline Double Sine halo + silky BoC vibrato, <0.45s rel for host_sim)
        {
            "BoC Glassy Sine",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.18f; p.dcwEnv = 0.34f;
                p.dcwAtt = 0.05f; p.dcwDec = 0.46f; p.dcwSus = 0.16f; p.dcwRel = 0.38f;
                p.dcaAtt = 0.06f; p.dcaDec = 0.62f; p.dcaSus = 0.88f; p.dcaRel = 0.40f;
                p.chorus = 0.34f; p.volume = 0.88f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 7;
                p.l2Mix = 0.32f; p.l2Dcw = 0.24f; p.l2EnvOfs = 0.08f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.18f;
                p.vibRate = 0.54f; p.vibDepth = 0.30f; // ~1.6 Hz silky sine + subtle VHS tape wow
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.0f; p.spaceDecay = 0.50f;
                return p;
            }()
        },

        // 1: Olson VHS Sine (iconic MHTRTC warm wobbling hollow VHS sine-organ chord pad in Space!)
        {
            "Olson VHS Sine",
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
                p.vibRate = 0.42f; p.vibDepth = 0.38f; // ~0.95 Hz seasick BoC tape warble!
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.48f; p.spaceDecay = 0.74f;
                return p;
            }()
        },

        // 2: Roygbiv Sine Ld (warm, clean, slightly hollow BoC glassy sine mono lead with glide & vibrato)
        {
            "Roygbiv Sine Ld",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 2; p.modMode = 0;
                p.dcwTimbre = 0.30f; p.dcwEnv = 0.34f;
                p.dcwAtt = 0.02f; p.dcwDec = 0.44f; p.dcwSus = 0.32f; p.dcwRel = 0.46f;
                p.dcaAtt = 0.03f; p.dcaDec = 0.64f; p.dcaSus = 0.88f; p.dcaRel = 0.48f;
                p.chorus = 0.26f; p.volume = 0.88f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 6;
                p.l2Mix = 0.28f; p.l2Dcw = 0.22f; p.l2EnvOfs = -0.10f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.14f;
                p.vibRate = 0.66f; p.vibDepth = 0.32f; // ~2.7 Hz expressive vintage vibrato
                p.voiceMode = 2; p.glide = 0.16f; p.spaceSend = 0.36f; p.spaceDecay = 0.60f;
                return p;
            }()
        },

        // 3: Turquoise Glass (pure crystalline glassy sine bell/pad with octave Double Sine chime & BoC warble)
        {
            "Turquoise Glass",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 0; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.10f; p.dcwEnv = 0.30f;
                p.dcwAtt = 0.08f; p.dcwDec = 0.64f; p.dcwSus = 0.14f; p.dcwRel = 0.82f;
                p.dcaAtt = 0.08f; p.dcaDec = 0.84f; p.dcaSus = 0.72f; p.dcaRel = 0.84f;
                p.chorus = 0.50f; p.volume = 0.88f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 8;
                p.l2Mix = 0.34f; p.l2Dcw = 0.22f; p.l2EnvOfs = -0.32f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibRate = 0.46f; p.vibDepth = 0.40f; // ~1.1 Hz hypnotic crystalline shimmer
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.58f; p.spaceDecay = 0.80f;
                return p;
            }()
        },

        // 4: NFB Film Organ (1970s/80s National Film Board haunting glassy flute/sine organ with 16mm wow)
        {
            "NFB Film Organ",
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
                p.vibRate = 0.34f; p.vibDepth = 0.44f; // ~0.66 Hz authentic 16mm projector wow!
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.46f; p.spaceDecay = 0.72f;
                return p;
            }()
        },

        // 5: Dayvan Glass Pad (lush sweeping warm Saw-Pulse sine + Rez-Tri glassy swell + BoC vibrato)
        {
            "Dayvan Glass Pad",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.20f; p.dcwEnv = 0.46f;
                p.dcwAtt = 0.76f; p.dcwDec = 0.86f; p.dcwSus = 0.38f; p.dcwRel = 0.84f;
                p.dcaAtt = 0.68f; p.dcaDec = 0.84f; p.dcaSus = 0.90f; p.dcaRel = 0.84f;
                p.chorus = 0.62f; p.volume = 0.86f;
                p.l2Wave1 = 6; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 9;
                p.l2Mix = 0.50f; p.l2Dcw = 0.44f; p.l2EnvOfs = 0.36f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.25f;
                p.vibRate = 0.48f; p.vibDepth = 0.34f; // ~1.2 Hz dreamy cinematic pitch shimmer
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.54f; p.spaceDecay = 0.78f;
                return p;
            }()
        },

        // 6: Crystal Tine EP (authentic siney vintage CZ-101 electric piano — buttery Saw-Pulse sine body + isolated Rez-Tri glassy tine!)
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
                p.vibRate = 0.54f; p.vibDepth = 0.18f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.26f; p.spaceDecay = 0.58f;
                return p;
            }()
        },

        // 7: Aphex SAW Pad (dreamy SAW 85-92 phase-inverted Rez-Tri + Saw-Pulse cloud drenched in airy Space!)
        {
            "Aphex SAW Pad",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 6; p.l1Wave2 = 5; p.modMode = 0;
                p.dcwTimbre = 0.14f; p.dcwEnv = 0.54f;
                p.dcwAtt = 0.80f; p.dcwDec = 0.88f; p.dcwSus = 0.46f; p.dcwRel = 0.86f;
                p.dcaAtt = 0.74f; p.dcaDec = 0.84f; p.dcaSus = 0.88f; p.dcaRel = 0.86f;
                p.chorus = 0.68f; p.volume = 0.84f;
                p.l2Wave1 = 7; p.l2Wave2 = 5; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 14;
                p.l2Mix = 0.88f; p.l2Dcw = 0.62f; p.l2EnvOfs = 0.40f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.30f;
                p.vibRate = 0.40f; p.vibDepth = 0.26f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.56f; p.spaceDecay = 0.78f;
                return p;
            }()
        },

        // 8: Rhubarb Glass (fragile SAW II ambient glass pad — pure warm Saw-Pulse + slow octave Double Sine halo)
        {
            "Rhubarb Glass",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.06f; p.dcwEnv = 0.40f;
                p.dcwAtt = 0.84f; p.dcwDec = 0.90f; p.dcwSus = 0.32f; p.dcwRel = 0.90f;
                p.dcaAtt = 0.76f; p.dcaDec = 0.88f; p.dcaSus = 0.86f; p.dcaRel = 0.90f;
                p.chorus = 0.64f; p.volume = 0.86f;
                p.l2Wave1 = 3; p.l2Wave2 = 7; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 8;
                p.l2Mix = 0.50f; p.l2Dcw = 0.40f; p.l2EnvOfs = 0.54f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.30f;
                p.vibRate = 0.36f; p.vibDepth = 0.30f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.62f; p.spaceDecay = 0.84f;
                return p;
            }()
        },

        // === FAT PD BASSES & ACID LEADS (9..16) ===

        // 9: Fat Rez Bass (MASSIVE unipolar Rez Saw + sub-octave Square — fat bass fundamental + screaming filter pluck!)
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
                p.l2Mix = 0.86f; p.l2Dcw = 0.45f; p.l2EnvOfs = -0.18f;
                p.pitchEnv = 0.12f; p.pitchDec = 0.16f;
                p.vibRate = 0.30f; p.vibDepth = 0.0f;
                p.voiceMode = 2; p.glide = 0.08f; p.spaceSend = 0.0f; p.spaceDecay = 0.35f;
                return p;
            }()
        },

        // 10: Acid Rez Squelch (303-meets-CZ-101 15x resonant acid mono line with glide + tight dub echo)
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
                p.l2Mix = 0.72f; p.l2Dcw = 0.86f; p.l2EnvOfs = -0.10f;
                p.pitchEnv = 0.06f; p.pitchDec = 0.14f;
                p.vibRate = 0.34f; p.vibDepth = 0.0f;
                p.voiceMode = 2; p.glide = 0.24f; p.spaceSend = 0.22f; p.spaceDecay = 0.48f;
                return p;
            }()
        },

        // 11: Electro Slap Bass (Converted CZ-101 Elec Bass — rubbery Saw+Square + sub-octave punch)
        {
            "Electro Slap Bs",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 0; p.l1Wave2 = 2; p.modMode = 0;
                p.dcwTimbre = 0.18f; p.dcwEnv = 0.90f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.48f; p.dcwSus = 0.14f; p.dcwRel = 0.38f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.64f; p.dcaSus = 0.58f; p.dcaRel = 0.34f;
                p.chorus = 0.0f; p.volume = 0.88f;
                p.l2Wave1 = 2; p.l2Wave2 = 0; p.l2Oct = -1; p.l2Semi = 0; p.l2Fine = 4;
                p.l2Mix = 0.90f; p.l2Dcw = 0.52f; p.l2EnvOfs = -0.22f;
                p.pitchEnv = 0.18f; p.pitchDec = 0.14f;
                p.vibRate = 0.30f; p.vibDepth = 0.0f;
                p.voiceMode = 2; p.glide = 0.04f; p.spaceSend = 0.0f; p.spaceDecay = 0.35f;
                return p;
            }()
        },

        // 12: Deep Sub Pulse (weighty UK rave / Aphex sub-octave Pulse + Double Sine bass)
        {
            "Deep Sub Pulse",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 2; p.l1Wave2 = 4; p.modMode = 0;
                p.dcwTimbre = 0.12f; p.dcwEnv = 0.62f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.58f; p.dcwSus = 0.24f; p.dcwRel = 0.44f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.72f; p.dcaSus = 0.84f; p.dcaRel = 0.40f;
                p.chorus = 0.0f; p.volume = 0.90f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = -1; p.l2Semi = 0; p.l2Fine = 3;
                p.l2Mix = 0.96f; p.l2Dcw = 0.22f; p.l2EnvOfs = 0.0f;
                p.pitchEnv = 0.08f; p.pitchDec = 0.14f;
                p.vibRate = 0.26f; p.vibDepth = 0.0f;
                p.voiceMode = 2; p.glide = 0.10f; p.spaceSend = 0.0f; p.spaceDecay = 0.30f;
                return p;
            }()
        },

        // 13: Hollow House Bass (90s UK garage / house bouncy Rez Tri + octave-down punch)
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
                p.l2Mix = 0.82f; p.l2Dcw = 0.34f; p.l2EnvOfs = -0.14f;
                p.pitchEnv = 0.06f; p.pitchDec = 0.12f;
                p.vibRate = 0.30f; p.vibDepth = 0.0f;
                p.voiceMode = 2; p.glide = 0.05f; p.spaceSend = 0.10f; p.spaceDecay = 0.35f;
                return p;
            }()
        },

        // 14: Rubber Techno Arp (snappy Saw-Pulse + Rez Saw stereo ping-pong sequence bass/arp)
        {
            "Rubber Techno Arp",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 6; p.modMode = 0;
                p.dcwTimbre = 0.18f; p.dcwEnv = 0.88f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.44f; p.dcwSus = 0.10f; p.dcwRel = 0.40f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.56f; p.dcaSus = 0.18f; p.dcaRel = 0.38f;
                p.chorus = 0.18f; p.volume = 0.86f;
                p.l2Wave1 = 5; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 11;
                p.l2Mix = 0.84f; p.l2Dcw = 0.80f; p.l2EnvOfs = -0.24f;
                p.pitchEnv = 0.08f; p.pitchDec = 0.10f;
                p.vibRate = 0.32f; p.vibDepth = 0.0f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.38f; p.spaceDecay = 0.56f;
                return p;
            }()
        },

        // 15: Detuned Rave Lead (biting 80s/90s CZ mono glide lead with Chorus + Space echo)
        {
            "Detuned Rave Ld",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 2; p.modMode = 0;
                p.dcwTimbre = 0.44f; p.dcwEnv = 0.56f;
                p.dcwAtt = 0.10f; p.dcwDec = 0.58f; p.dcwSus = 0.58f; p.dcwRel = 0.48f;
                p.dcaAtt = 0.04f; p.dcaDec = 0.62f; p.dcaSus = 0.88f; p.dcaRel = 0.46f;
                p.chorus = 0.38f; p.volume = 0.84f;
                p.l2Wave1 = 5; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 18;
                p.l2Mix = 0.90f; p.l2Dcw = 0.84f; p.l2EnvOfs = 0.0f;
                p.pitchEnv = 0.05f; p.pitchDec = 0.14f;
                p.vibRate = 0.38f; p.vibDepth = 0.14f;
                p.voiceMode = 2; p.glide = 0.24f; p.spaceSend = 0.34f; p.spaceDecay = 0.58f;
                return p;
            }()
        },

        // 16: Metallic Clav (Converted Ring-blended percussive CZ clavinet — snappy funk bite)
        {
            "Metallic Clav",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 2; p.l1Wave2 = 5; p.modMode = 1;
                p.dcwTimbre = 0.28f; p.dcwEnv = 0.74f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.48f; p.dcwSus = 0.14f; p.dcwRel = 0.38f;
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

        // === CONVERTED CASIO CZ-101 KEYS, BELLS & HORNS (17..23) ===

        // 17: Velvet 1+2' Pad (classic Casio CZ-101 1+2' phase-inverted warm PWM glassy pad + chorus)
        {
            "Velvet 1+2' Pad",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 4; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.24f; p.dcwEnv = 0.48f;
                p.dcwAtt = 0.68f; p.dcwDec = 0.80f; p.dcwSus = 0.56f; p.dcwRel = 0.78f;
                p.dcaAtt = 0.62f; p.dcaDec = 0.78f; p.dcaSus = 0.90f; p.dcaRel = 0.80f;
                p.chorus = 0.58f; p.volume = 0.86f;
                p.l2Wave1 = 4; p.l2Wave2 = 4; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 8;
                p.l2Mix = 0.96f; p.l2Dcw = 0.82f; p.l2EnvOfs = 0.22f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibRate = 0.22f; p.vibDepth = 0.08f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.44f; p.spaceDecay = 0.70f;
                return p;
            }()
        },

        // 18: Tubular Chimes (authentic CZ Ring Mod metallic bell — 4.5s shimmering ring + Space!)
        {
            "Tubular Chimes",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 3; p.l1Wave2 = 7; p.modMode = 1;
                p.dcwTimbre = 0.18f; p.dcwEnv = 0.82f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.84f; p.dcwSus = 0.16f; p.dcwRel = 0.84f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.92f; p.dcaSus = 0.10f; p.dcaRel = 0.88f;
                p.chorus = 0.34f; p.volume = 0.86f;
                p.l2Wave1 = 5; p.l2Wave2 = 0; p.l2Oct = 1; p.l2Semi = 7; p.l2Fine = 14;
                p.l2Mix = 0.92f; p.l2Dcw = 0.88f; p.l2EnvOfs = -0.32f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.16f;
                p.vibRate = 0.22f; p.vibDepth = 0.0f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.48f; p.spaceDecay = 0.74f;
                return p;
            }()
        },

        // 19: Woody Marimba (Converted CZ-101 Marimba — warm resonant triangle + 2-oct wooden knock)
        {
            "Woody Marimba",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 6; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.14f; p.dcwEnv = 0.76f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.46f; p.dcwSus = 0.08f; p.dcwRel = 0.56f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.68f; p.dcaSus = 0.06f; p.dcaRel = 0.58f;
                p.chorus = 0.20f; p.volume = 0.88f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = 2; p.l2Semi = 0; p.l2Fine = 4;
                p.l2Mix = 0.74f; p.l2Dcw = 0.62f; p.l2EnvOfs = -0.66f;
                p.pitchEnv = 0.12f; p.pitchDec = 0.10f;
                p.vibRate = 0.28f; p.vibDepth = 0.0f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.28f; p.spaceDecay = 0.50f;
                return p;
            }()
        },

        // 20: Bright Harpsi (Converted CZ-101 Harpsichord — crisp plucked Saw-Pulse + octave chime)
        {
            "Bright Harpsi",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 3; p.modMode = 0;
                p.dcwTimbre = 0.34f; p.dcwEnv = 0.66f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.64f; p.dcwSus = 0.14f; p.dcwRel = 0.50f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.76f; p.dcaSus = 0.08f; p.dcaRel = 0.48f;
                p.chorus = 0.16f; p.volume = 0.84f;
                p.l2Wave1 = 0; p.l2Wave2 = 0; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 4;
                p.l2Mix = 0.72f; p.l2Dcw = 0.90f; p.l2EnvOfs = -0.38f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.14f;
                p.vibRate = 0.30f; p.vibDepth = 0.0f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.22f; p.spaceDecay = 0.46f;
                return p;
            }()
        },

        // 21: Slap Horn Funk (Converted CZ-101 Slap Horn — Pulse + Rez Trap biting brass stab)
        {
            "Slap Horn Funk",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 2; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.26f; p.dcwEnv = 0.74f;
                p.dcwAtt = 0.28f; p.dcwDec = 0.56f; p.dcwSus = 0.54f; p.dcwRel = 0.44f;
                p.dcaAtt = 0.14f; p.dcaDec = 0.60f; p.dcaSus = 0.84f; p.dcaRel = 0.42f;
                p.chorus = 0.26f; p.volume = 0.86f;
                p.l2Wave1 = 3; p.l2Wave2 = 8; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 12;
                p.l2Mix = 0.88f; p.l2Dcw = 0.84f; p.l2EnvOfs = -0.18f;
                p.pitchEnv = 0.08f; p.pitchDec = 0.16f;
                p.vibRate = 0.36f; p.vibDepth = 0.08f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.20f; p.spaceDecay = 0.44f;
                return p;
            }()
        },

        // 22: Light Harp Pluck (Converted CZ-101 Light Harp — glassy Ring Mod arpeggio harp in Space)
        {
            "Light Harp Pluck",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 4; p.l1Wave2 = 0; p.modMode = 1;
                p.dcwTimbre = 0.16f; p.dcwEnv = 0.82f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.66f; p.dcwSus = 0.20f; p.dcwRel = 0.70f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.78f; p.dcaSus = 0.14f; p.dcaRel = 0.72f;
                p.chorus = 0.42f; p.volume = 0.86f;
                p.l2Wave1 = 3; p.l2Wave2 = 0; p.l2Oct = 1; p.l2Semi = 0; p.l2Fine = 6;
                p.l2Mix = 0.64f; p.l2Dcw = 0.72f; p.l2EnvOfs = -0.35f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.16f;
                p.vibRate = 0.26f; p.vibDepth = 0.0f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.46f; p.spaceDecay = 0.68f;
                return p;
            }()
        },

        // 23: Breathy Pan Pipe (Converted CZ-101 Flute — Noise chiff + silky vibrato + ambient echo)
        {
            "Breathy Pan Pipe",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 3; p.l1Wave2 = 7; p.modMode = 2;
                p.dcwTimbre = 0.18f; p.dcwEnv = 0.52f;
                p.dcwAtt = 0.46f; p.dcwDec = 0.64f; p.dcwSus = 0.52f; p.dcwRel = 0.62f;
                p.dcaAtt = 0.48f; p.dcaDec = 0.62f; p.dcaSus = 0.84f; p.dcaRel = 0.64f;
                p.chorus = 0.40f; p.volume = 0.84f;
                p.l2Wave1 = 6; p.l2Wave2 = 0; p.l2Oct = 0; p.l2Semi = 0; p.l2Fine = 8;
                p.l2Mix = 0.74f; p.l2Dcw = 0.50f; p.l2EnvOfs = -0.42f;
                p.pitchEnv = 0.05f; p.pitchDec = 0.18f;
                p.vibRate = 0.34f; p.vibDepth = 0.18f;
                p.voiceMode = 1; p.glide = 0.14f; p.spaceSend = 0.46f; p.spaceDecay = 0.66f;
                return p;
            }()
        },

        // === SYNTHY NOISES, IDM PERCUSSION & WEIRD PD TEXTURES (24..31) ===

        // 24: Sci-Fi Ring Zaps (wild inharmonic Ring Mod + big pitch drop laser/alien bleeps in Space)
        {
            "Sci-Fi Ring Zaps",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 5; p.l1Wave2 = 3; p.modMode = 1;
                p.dcwTimbre = 0.22f; p.dcwEnv = 0.88f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.58f; p.dcwSus = 0.12f; p.dcwRel = 0.52f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.64f; p.dcaSus = 0.08f; p.dcaRel = 0.54f;
                p.chorus = 0.30f; p.volume = 0.86f;
                p.l2Wave1 = 6; p.l2Wave2 = 2; p.l2Oct = 1; p.l2Semi = 6; p.l2Fine = 35;
                p.l2Mix = 1.0f; p.l2Dcw = 0.94f; p.l2EnvOfs = 0.25f;
                p.pitchEnv = 0.92f; p.pitchDec = 0.52f;
                p.vibRate = 0.68f; p.vibDepth = 0.38f;
                p.voiceMode = 0; p.glide = 0.18f; p.spaceSend = 0.54f; p.spaceDecay = 0.72f;
                return p;
            }()
        },

        // 25: Noise Wind Sweep (Noise Mod + sweeping Rez Trap cyber storm in cavernous Space)
        {
            "Noise Wind Sweep",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 7; p.l1Wave2 = 6; p.modMode = 2;
                p.dcwTimbre = 0.10f; p.dcwEnv = 0.94f;
                p.dcwAtt = 0.86f; p.dcwDec = 0.90f; p.dcwSus = 0.36f; p.dcwRel = 0.88f;
                p.dcaAtt = 0.68f; p.dcaDec = 0.86f; p.dcaSus = 0.80f; p.dcaRel = 0.88f;
                p.chorus = 0.76f; p.volume = 0.84f;
                p.l2Wave1 = 5; p.l2Wave2 = 0; p.l2Oct = -1; p.l2Semi = 5; p.l2Fine = 28;
                p.l2Mix = 0.90f; p.l2Dcw = 0.92f; p.l2EnvOfs = 0.50f;
                p.pitchEnv = 0.18f; p.pitchDec = 0.80f;
                p.vibRate = 0.16f; p.vibDepth = 0.26f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.68f; p.spaceDecay = 0.86f;
                return p;
            }()
        },

        // 26: Broken Arcade Toy (detuned +6st Ring+Noise lo-fi glitch chime percussion)
        {
            "Broken Arcade",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 2; p.l1Wave2 = 6; p.modMode = 3;
                p.dcwTimbre = 0.36f; p.dcwEnv = 0.72f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.42f; p.dcwSus = 0.14f; p.dcwRel = 0.42f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.54f; p.dcaSus = 0.08f; p.dcaRel = 0.40f;
                p.chorus = 0.22f; p.volume = 0.86f;
                p.l2Wave1 = 5; p.l2Wave2 = 4; p.l2Oct = 1; p.l2Semi = 6; p.l2Fine = 42;
                p.l2Mix = 0.92f; p.l2Dcw = 0.96f; p.l2EnvOfs = -0.45f;
                p.pitchEnv = 0.38f; p.pitchDec = 0.22f;
                p.vibRate = 0.76f; p.vibDepth = 0.34f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.42f; p.spaceDecay = 0.60f;
                return p;
            }()
        },

        // 27: Radio Static Bell (eerie Ring+Noise shortwave metallic ping floating in deep Space)
        {
            "Radio Static Bell",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 6; p.l1Wave2 = 4; p.modMode = 3;
                p.dcwTimbre = 0.24f; p.dcwEnv = 0.78f;
                p.dcwAtt = 0.02f; p.dcwDec = 0.76f; p.dcwSus = 0.12f; p.dcwRel = 0.82f;
                p.dcaAtt = 0.02f; p.dcaDec = 0.84f; p.dcaSus = 0.06f; p.dcaRel = 0.86f;
                p.chorus = 0.54f; p.volume = 0.86f;
                p.l2Wave1 = 7; p.l2Wave2 = 0; p.l2Oct = 2; p.l2Semi = 3; p.l2Fine = 27;
                p.l2Mix = 0.95f; p.l2Dcw = 0.90f; p.l2EnvOfs = -0.20f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.20f;
                p.vibRate = 0.48f; p.vibDepth = 0.22f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.66f; p.spaceDecay = 0.86f;
                return p;
            }()
        },

        // 28: Electro Synth Tom (classic 80s CZ laser tom / pitch-drop drum)
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

        // 29: Glitch PD Blip (ultra-snappy Aphex/Autechre IDM resonant micro-percussion)
        {
            "Glitch PD Blip",
            [] {
                SynthParams p;
                p.lineMode = 2; p.l1Wave1 = 5; p.l1Wave2 = 2; p.modMode = 1;
                p.dcwTimbre = 0.18f; p.dcwEnv = 0.96f;
                p.dcwAtt = 0.01f; p.dcwDec = 0.26f; p.dcwSus = 0.02f; p.dcwRel = 0.28f;
                p.dcaAtt = 0.01f; p.dcaDec = 0.34f; p.dcaSus = 0.02f; p.dcaRel = 0.28f;
                p.chorus = 0.14f; p.volume = 0.88f;
                p.l2Wave1 = 6; p.l2Wave2 = 0; p.l2Oct = 2; p.l2Semi = 5; p.l2Fine = 19;
                p.l2Mix = 0.90f; p.l2Dcw = 0.94f; p.l2EnvOfs = -0.50f;
                p.pitchEnv = 0.48f; p.pitchDec = 0.14f;
                p.vibRate = 0.80f; p.vibDepth = 0.18f;
                p.voiceMode = 0; p.glide = 0.0f; p.spaceSend = 0.44f; p.spaceDecay = 0.58f;
                return p;
            }()
        },

        // 30: Cyber Drone FX (dark -2 oct phase-inverted Ring+Noise industrial machine rumble)
        {
            "Cyber Drone FX",
            [] {
                SynthParams p;
                p.lineMode = 3; p.l1Wave1 = 5; p.l1Wave2 = 1; p.modMode = 3;
                p.dcwTimbre = 0.34f; p.dcwEnv = 0.66f;
                p.dcwAtt = 0.86f; p.dcwDec = 0.90f; p.dcwSus = 0.62f; p.dcwRel = 0.88f;
                p.dcaAtt = 0.80f; p.dcaDec = 0.88f; p.dcaSus = 0.88f; p.dcaRel = 0.88f;
                p.chorus = 0.64f; p.volume = 0.84f;
                p.l2Wave1 = 7; p.l2Wave2 = 0; p.l2Oct = -2; p.l2Semi = 1; p.l2Fine = 32;
                p.l2Mix = 0.94f; p.l2Dcw = 0.88f; p.l2EnvOfs = 0.44f;
                p.pitchEnv = 0.0f; p.pitchDec = 0.30f;
                p.vibRate = 0.14f; p.vibDepth = 0.24f;
                p.voiceMode = 1; p.glide = 0.32f; p.spaceSend = 0.62f; p.spaceDecay = 0.84f;
                return p;
            }()
        },

        // 31: Init CZ Saw (clean bright single-line CZ Saw starting point)
        {
            "Init CZ Saw",
            [] {
                SynthParams p;
                p.lineMode = 0; p.l1Wave1 = 0; p.l1Wave2 = 0; p.modMode = 0;
                p.dcwTimbre = 0.35f; p.dcwEnv = 0.60f;
                p.dcwAtt = 0.04f; p.dcwDec = 0.56f; p.dcwSus = 0.45f; p.dcwRel = 0.46f;
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
