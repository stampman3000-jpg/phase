// CZboy — Desktop / On-Device ARMv7 CPU Benchmark (44.1 kHz, 128-frame blocks).
// Copyright (C) 2026 Stampman3000 — GNU General Public License v3.0 or later.

#include "../bridge/PhasePresets.h"
#include "../dsp/PhaseEngine.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <vector>

using namespace phase;

static void benchScenario (const char* label, const SynthParams& p, int numNotes)
{
    const double sr = 44100.0;
    const int block = 128;
    const int warmup = 64;
    const int iters  = 2000;

    PhaseEngine eng;
    eng.prepare (sr);
    const int notes[8] = { 48, 52, 55, 59, 60, 64, 67, 71 };
    for (int i = 0; i < numNotes && i < 8; ++i)
        eng.noteOn (notes[i], 0.9f, p);

    std::vector<float> L (block), R (block);
    for (int i = 0; i < warmup; ++i)
        eng.render (L.data(), R.data(), block, p, 120.0);

    std::vector<double> times (iters);
    for (int i = 0; i < iters; ++i)
    {
        const auto t0 = std::chrono::steady_clock::now();
        eng.render (L.data(), R.data(), block, p, 120.0);
        const auto t1 = std::chrono::steady_clock::now();
        times[(size_t) i] = std::chrono::duration<double, std::micro> (t1 - t0).count();
    }

    std::sort (times.begin(), times.end());
    double sum = 0.0;
    for (double t : times) sum += t;
    const double avg = sum / (double) iters;
    const double p99 = times[(size_t) (iters * 99 / 100)];
    const double budgetUs = (double) block * 1e6 / sr; // 2902.5 us
    std::printf ("%-44s %7.1f  %7.1f  %6.2f%%  %6.2f%%\n",
                 label, avg, p99, (avg / budgetUs) * 100.0, (p99 / budgetUs) * 100.0);
}

int main()
{
    int cnt = 0;
    const PhasePreset* presets = factoryPresets (cnt);

    std::printf ("=== Phase CPU Benchmark (44.1 kHz, 128-frame = 2902.5 us) ===\n");
    std::printf ("%-44s %7s  %7s  %7s  %7s\n", "Scenario", "Avg(us)", "p99(us)", "Avg%128", "p99%128");
    std::printf ("--------------------------------------------------------------------------------\n");

    SynthParams pL1Dry = presets[31].p; // Init CZ Saw, Line 1 dry
    pL1Dry.dcaSus = 0.9f; pL1Dry.chorus = 0.0f; pL1Dry.spaceSend = 0.0f;
    benchScenario ("Line 1 Only (4 voices, dry)", pL1Dry, 4);
    benchScenario ("Line 1 Only (8 voices, dry)", pL1Dry, 8);

    SynthParams pDual = presets[0].p; // Glassy Sine + Chorus
    pDual.dcaSus = 0.9f; pDual.vibDepth = 0.2f;
    benchScenario ("Dual-Line 1+2 + Chorus (8 voices)", pDual, 8);

    SynthParams pAphex = presets[5].p; // Ambient Phase Pad: 1-2 + Chorus + Space FX
    pAphex.dcaAtt = 0.02f; pAphex.dcaSus = 0.9f;
    benchScenario ("Dual-Line + Chorus + Space FX (8 voices)", pAphex, 8);

    SynthParams pRing = presets[28].p; // Sci-Fi Ring Zaps + Noise + Space FX
    pRing.dcaAtt = 0.02f; pRing.dcaSus = 0.9f; pRing.modMode = 3;
    benchScenario ("Dual-Line Ring+Noise+Chorus+Space (8v)", pRing, 8);

    return 0;
}
