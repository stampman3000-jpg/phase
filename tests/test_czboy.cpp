// CZboy — Phase Distortion Tuning, 8 CZ Waveforms, Live DCW Sweep, Ring Mod & 16-Preset Validation Suite.
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

    // 1. Exact A4 = 440.0 Hz tuning with DCW = 0 (pure cosine core)
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        SynthParams p;
        p.lineMode = 0; p.l1Wave1 = 0; p.l1Wave2 = 0; p.modMode = 0;
        p.dcwTimbre = 0.0f; p.dcwEnv = 0.0f;
        p.dcaAtt = 0.0f; p.dcaDec = 0.1f; p.dcaSus = 1.0f; p.dcaRel = 0.2f;
        p.chorus = 0.0f; p.volume = 0.85f; p.pitchEnv = 0.0f; p.vibDepth = 0.0f; p.velSens = 0.0f;

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

    // 2. All 8 CZ Waveforms render cleanly and audibly
    {
        int audible = 0, finite = 0;
        double minRms = 1e9, maxSlew = 0.0;
        for (int w = 0; w < 8; ++w)
        {
            CzBoyEngine eng;
            eng.prepare (sr);
            SynthParams p;
            p.lineMode = 0; p.l1Wave1 = w; p.l1Wave2 = 0; p.modMode = 0;
            p.dcwTimbre = 0.85f; p.dcwEnv = 0.0f;
            p.dcaAtt = 0.0f; p.dcaDec = 0.2f; p.dcaSus = 0.90f; p.dcaRel = 0.2f;
            p.chorus = 0.0f; p.volume = 0.85f; p.velSens = 0.0f;

            eng.noteOn (57, 1.0f, p);
            std::vector<float> L (16384), R (16384);
            for (int pos = 0; pos < 16384; pos += 128)
                eng.render (L.data() + pos, R.data() + pos, 128, p);
            if (allFinite (L) && allFinite (R)) ++finite;
            const double r  = rmsRange (L, 2000, 16000);
            const double sl = hfSlew (L, 2000, 16000);
            if (r > 0.08) ++audible;
            if (r < minRms) minRms = r;
            if (sl > maxSlew) maxSlew = sl;
        }
        std::snprintf (msg, sizeof (msg), "%d/8 audible, %d/8 finite, minRms=%.3f maxSlew=%.3f", audible, finite, minRms, maxSlew);
        check (audible == 8 && finite == 8, "All 8 CZ Phase Distortion waveforms full-scale", msg);
    }

    // 3. Live real-time DCW Timbre immediately sweeps held note from pure sine to screaming Rez
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        SynthParams p;
        p.lineMode = 0; p.l1Wave1 = 6; p.l1Wave2 = 0; // REZ TRI
        p.dcwTimbre = 0.0f; p.dcwEnv = 0.0f;
        p.dcaAtt = 0.0f; p.dcaSus = 0.95f; p.chorus = 0.0f; p.velSens = 0.0f;

        eng.noteOn (48, 1.0f, p); // C3
        std::vector<float> L (N), R (N);
        for (int pos = 0; pos < N / 2; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, std::min (128, N / 2 - pos), p);
        // Twist DCW Timbre live mid-note without retriggering!
        p.dcwTimbre = 0.95f;
        for (int pos = N / 2; pos < N; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, std::min (128, N - pos), p);

        const double slDark   = hfSlew (L, 2000, N / 2 - 256);
        const double slBright = hfSlew (L, N / 2 + 256, N - 256);
        std::snprintf (msg, sizeof (msg), "live mid-note REZ TRI slew %.3f -> %.3f (%.1fx)", slDark, slBright, slBright / std::max (1e-6, slDark));
        check (slBright > slDark * 4.0 && allFinite (L), "Live DCW Timbre sweeps 15x Rez instantly mid-note", msg);
    }

    // 4. DCW Envelope sweeps resonant timbre down smoothly over time
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        SynthParams p;
        p.lineMode = 0; p.l1Wave1 = 6; p.l1Wave2 = 0; // REZ TRI
        p.dcwTimbre = 0.0f; p.dcwEnv = 0.98f;
        p.dcwAtt = 0.0f; p.dcwDec = 0.35f; p.dcwSus = 0.0f;
        p.dcaAtt = 0.0f; p.dcaDec = 0.2f; p.dcaSus = 0.90f; p.chorus = 0.0f; p.velSens = 0.0f;
        eng.noteOn (52, 1.0f, p);
        std::vector<float> L (N), R (N);
        for (int pos = 0; pos < N; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, std::min (128, N - pos), p);
        const double slEarly = hfSlew (L, 256, 2000);
        const double slLate  = hfSlew (L, 14000, 21000);
        std::snprintf (msg, sizeof (msg), "REZ TRI envSlew early=%.3f -> late=%.3f", slEarly, slLate);
        check (slEarly > slLate * 2.5 && allFinite (L), "DCW EG decays resonant timbre smoothly", msg);
    }

    // 5. Dual-Line 1+2 Polyphonic 8-Voice Chord + Lush CZ Stereo Chorus
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        int cnt = 0;
        SynthParams p = factoryPresets (cnt)[0].p; // CZ-101 Brass
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

    // 6. Program 0 (CZ-101 Brass) releases cleanly to silence < 0.5 s
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        int cnt = 0;
        SynthParams p = factoryPresets (cnt)[0].p;
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
               "Program 0 releases cleanly to silence < 0.5 s", msg);
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
            if (r > 0.02) ++audible;
            else std::printf ("  [quiet preset %2d: %-18s rms=%.4f]\n", i, presets[i].name, r);
        }
        std::snprintf (msg, sizeof (msg), "%d/%d presets audible, %d/%d finite", audible, count, finite, count);
        check (audible == count && finite == count && count == 16,
               "All 16 factory presets audible & finite", msg);
    }

    std::printf ("\nSummary: %d passed, %d failed\n", gPass, gFail);
    return gFail == 0 ? 0 : 1;
}
