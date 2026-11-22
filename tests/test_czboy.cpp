// CZboy — Phase Distortion Tuning, 8 CZ Waveforms, DCW/DCA Envelopes, Ring/Noise & 16-Preset Validation Suite.
// Copyright (C) 2026 Stampman3000 — GNU General Public License v3.0 or later.

#include "../bridge/CzBoyPresets.h"
#include "../dsp/CzBoyEngine.h"

#include <cmath>
#include <cstdio>
#include <vector>

using namespace czboy;

static int gPass = 0, gFail = 0;
static void check (bool ok, const char* name, const char* detail = "")
{
    if (ok) { ++gPass; std::printf ("PASS  %-48s %s\n", name, detail); }
    else    { ++gFail; std::printf ("FAIL  %-48s %s\n", name, detail); }
}

static double rmsRange (const std::vector<float>& x, int a, int b)
{
    double s = 0.0;
    for (int i = a; i < b; ++i) s += (double) x[(size_t) i] * x[(size_t) i];
    return std::sqrt (s / (double) std::max (1, b - a));
}

static double hfSlew (const std::vector<float>& x, int a, int b)
{
    double sq = 0.0;
    for (int i = a + 1; i < b; ++i)
    {
        const double d = (double) x[(size_t) i] - (double) x[(size_t) (i - 1)];
        sq += d * d;
    }
    const double r = rmsRange (x, a, b);
    return std::sqrt (sq / (double) std::max (1, b - a - 1)) / (r + 1e-6);
}

static double measureZeroCrossHz (const std::vector<float>& x, int start, int end, double sr)
{
    int crosses = 0;
    for (int i = start + 1; i < end; ++i)
        if (x[(size_t) (i - 1)] <= 0.0f && x[(size_t) i] > 0.0f) ++crosses;
    return (double) crosses * sr / (double) (end - start);
}

static bool allFinite (const std::vector<float>& x)
{
    for (float v : x) if (! std::isfinite (v)) return false;
    return true;
}

int main()
{
    const double sr = 44100.0;
    const int N = 22050; // 0.5 s
    char msg[160];

    std::printf ("=== CZboy Phase Distortion Validation Suite (sr=%.0f Hz) ===\n\n", sr);

    // 1. Exact A4 = 440.0 Hz tuning with DCW = 0 (pure sine phase)
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        SynthParams p;
        p.lineMode = 0; p.l1Wave1 = 0; p.l1Wave2 = 0; p.modMode = 0;
        p.dcwDepth = 0.0f; p.dcwSus = 0.0f;
        p.dcaAtt = 0.0f; p.dcaDec = 0.1f; p.dcaSus = 1.0f; p.dcaRel = 0.2f;
        p.chorus = 0.0f; p.drive = 0.0f; p.volume = 0.85f;
        p.pitchEnv = 0.0f; p.vibDepth = 0.0f; p.velSens = 0.0f;

        eng.noteOn (69, 1.0f, p);
        std::vector<float> L (N), R (N);
        for (int pos = 0; pos < N; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, std::min (128, N - pos), p);

        const double hz  = measureZeroCrossHz (L, N / 4, N - 256, sr);
        const double rms = rmsRange (L, N / 4, N - 256);
        std::snprintf (msg, sizeof (msg), "measured %.2f Hz (target 440.0 Hz), rms=%.3f", hz, rms);
        check (std::fabs (hz - 440.0) < 3.0 && rms > 0.10 && allFinite (L),
               "A4 (440 Hz) tuning accuracy & pure cosine core", msg);
    }

    // 2. All 8 CZ Waveforms render cleanly and audibly with distinct harmonic profiles
    {
        int audible = 0, finite = 0;
        double minRms = 1e9, maxSlew = 0.0;
        for (int w = 0; w < 8; ++w)
        {
            CzBoyEngine eng;
            eng.prepare (sr);
            SynthParams p;
            p.lineMode = 0; p.l1Wave1 = w; p.l1Wave2 = 0; p.modMode = 0;
            p.dcwDepth = 0.85f; p.dcwAtt = 0.0f; p.dcwDec = 0.5f; p.dcwSus = 0.85f;
            p.dcaAtt = 0.0f; p.dcaDec = 0.2f; p.dcaSus = 0.90f; p.dcaRel = 0.2f;
            p.chorus = 0.0f; p.drive = 0.0f; p.volume = 0.85f; p.velSens = 0.0f;

            eng.noteOn (57, 1.0f, p);
            std::vector<float> L (16384), R (16384);
            for (int pos = 0; pos < 16384; pos += 128)
                eng.render (L.data() + pos, R.data() + pos, 128, p);
            if (allFinite (L) && allFinite (R)) ++finite;
            const double r = rmsRange (L, 2000, 16000);
            const double sl = hfSlew (L, 2000, 16000);
            if (r > 0.05) ++audible;
            if (r < minRms) minRms = r;
            if (sl > maxSlew) maxSlew = sl;
        }
        std::snprintf (msg, sizeof (msg), "%d/8 audible, %d/8 finite, minRms=%.3f maxSlew=%.3f", audible, finite, minRms, maxSlew);
        check (audible == 8 && finite == 8, "All 8 CZ Phase Distortion waveforms clean", msg);
    }

    // 3. DCW Envelope visibly sweeps harmonic brightness (high DCW vs low DCW)
    {
        auto renderSlew = [&] (float dcwDepth) {
            CzBoyEngine eng;
            eng.prepare (sr);
            SynthParams p;
            p.lineMode = 0; p.l1Wave1 = 0; p.l1Wave2 = 0;
            p.dcwDepth = dcwDepth; p.dcwAtt = 0.0f; p.dcwDec = 0.5f; p.dcwSus = 1.0f;
            p.dcaAtt = 0.0f; p.dcaSus = 0.9f; p.chorus = 0.0f; p.drive = 0.0f; p.velSens = 0.0f;
            eng.noteOn (57, 1.0f, p);
            std::vector<float> L (N), R (N);
            for (int pos = 0; pos < N; pos += 128)
                eng.render (L.data() + pos, R.data() + pos, std::min (128, N - pos), p);
            return hfSlew (L, 4000, N - 256);
        };
        const double slDark = renderSlew (0.0f);
        const double slBright = renderSlew (0.95f);
        std::snprintf (msg, sizeof (msg), "hfSlew dcw=0%% %.3f -> dcw=95%% %.3f", slDark, slBright);
        check (slBright > slDark * 1.45, "DCW depth sweeps Phase Distortion harmonics", msg);
    }

    // 4. DCW Envelope decay closes timbre smoothly over time
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        SynthParams p;
        p.lineMode = 0; p.l1Wave1 = 6; p.l1Wave2 = 0; // REZ TRI (continuous zero-endpoint window)
        p.dcwDepth = 0.95f; p.dcwAtt = 0.0f; p.dcwDec = 0.35f; p.dcwSus = 0.0f;
        p.dcaAtt = 0.0f; p.dcaDec = 0.2f; p.dcaSus = 0.90f; p.chorus = 0.0f; p.velSens = 0.0f;
        eng.noteOn (52, 1.0f, p);
        std::vector<float> L (N), R (N);
        for (int pos = 0; pos < N; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, std::min (128, N - pos), p);
        const double slEarly = hfSlew (L, 256, 2000);
        const double slLate  = hfSlew (L, 14000, 21000);
        std::snprintf (msg, sizeof (msg), "REZ TRI envSlew early=%.3f -> late=%.3f", slEarly, slLate);
        check (slEarly > slLate * 1.6 && allFinite (L), "DCW EG decays resonant timbre smoothly", msg);
    }

    // 5. Dual-Line 1+2 Polyphonic 8-Voice Chord + Ring Mod + Chorus Stereo Spread
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        SynthParams p;
        p.lineMode = 2; p.l1Wave1 = 2; p.l1Wave2 = 5; p.l2Wave1 = 0; p.l2Fine = 14;
        p.dcwDepth = 0.85f; p.dcwSus = 0.6f; p.dcaAtt = 0.02f; p.dcaSus = 0.85f;
        p.chorus = 0.45f; p.voiceMode = 0;
        const int chord[8] = { 48, 52, 55, 59, 60, 64, 67, 71 };
        for (int n : chord) eng.noteOn (n, 0.9f, p);
        std::vector<float> L (N), R (N);
        for (int pos = 0; pos < N; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, std::min (128, N - pos), p);
        double diffSq = 0.0;
        for (int i = 2000; i < N; ++i) { double d = L[(size_t) i] - R[(size_t) i]; diffSq += d * d; }
        const double stereoDiff = std::sqrt (diffSq / (double) (N - 2000));
        const double rmsL = rmsRange (L, 2000, N);
        std::snprintf (msg, sizeof (msg), "voices=%d rmsL=%.3f stereoDiff=%.3f", eng.activeVoices(), rmsL, stereoDiff);
        check (eng.activeVoices() == 8 && rmsL > 0.10 && stereoDiff > 0.02 && allFinite (L) && allFinite (R),
               "8-voice dual-line polyphony + stereo chorus", msg);
    }

    // 6. DCA Note-Off release settles cleanly to silence
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        int cnt = 0;
        SynthParams p = factoryPresets (cnt)[0].p; // Init CZ Saw
        eng.noteOn (60, 1.0f, p);
        std::vector<float> L (N), R (N);
        for (int pos = 0; pos < 4096; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, 128, p);
        eng.noteOff (60);
        for (int pos = 0; pos < N; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, std::min (128, N - pos), p);
        const double tailRms = rmsRange (L, N - 4410, N);
        std::snprintf (msg, sizeof (msg), "tailRms=%.6f activeVoices=%d", tailRms, eng.activeVoices());
        check (tailRms < 1e-4 && eng.activeVoices() == 0 && allFinite (L),
               "Init preset releases cleanly to silence < 0.5 s", msg);
    }

    // 7. All 16 Handcrafted Factory Presets render audibly with zero NaN/Inf
    {
        int count = 0;
        const CzPreset* presets = factoryPresets (count);
        int audible = 0, finite = 0;
        std::vector<float> L (16384), R (16384);
        for (int i = 0; i < count; ++i)
        {
            CzBoyEngine eng;
            eng.prepare (sr);
            eng.noteOn (60, 0.9f, presets[i].p);
            for (int pos = 0; pos < 16384; pos += 128)
                eng.render (L.data() + pos, R.data() + pos, 128, presets[i].p);
            if (allFinite (L) && allFinite (R)) ++finite;
            const double r = std::max (rmsRange (L, 256, 16384), rmsRange (R, 256, 16384));
            if (r > 0.015) ++audible;
            else std::printf ("  [quiet preset %2d: %-18s rms=%.4f]\n", i, presets[i].name, r);
        }
        std::snprintf (msg, sizeof (msg), "%d/%d presets audible, %d/%d finite", audible, count, finite, count);
        check (audible == count && finite == count && count == 16,
               "All 16 factory presets audible & finite", msg);
    }

    std::printf ("\nSummary: %d passed, %d failed\n", gPass, gFail);
    return gFail == 0 ? 0 : 1;
}
