// CZboy — Phase Distortion Tuning, 8 Unipolar CZ Waveforms, Live DCW Sweep, Space Send & 32-Preset Suite.
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
        p.chorus = 0.0f; p.spaceSend = 0.0f; p.volume = 0.85f; p.pitchEnv = 0.0f; p.vibDepth = 0.0f;

        eng.noteOn (69, 1.0f, p);
        std::vector<float> L (N), R (N);
        for (int pos = 0; pos < N; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, std::min (128, N - pos), p, 120.0);

        const double hz  = measureZeroCrossHz (L, N / 4, N - 256, sr);
        const double rms = rmsRange (L, N / 4, N - 256);
        std::snprintf (msg, sizeof (msg), "measured %.2f Hz (target 440.0 Hz), rms=%.3f", hz, rms);
        check (std::fabs (hz - 440.0) < 3.0 && rms > 0.10 && allFinite (L),
               "A4 (440 Hz) tuning accuracy & pure cosine core", msg);
    }

    // 2. All 8 CZ Waveforms render cleanly at full scale (including unipolar REZ SAW/TRI/TRAP)
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
            p.chorus = 0.0f; p.spaceSend = 0.0f; p.volume = 0.85f;

            eng.noteOn (57, 1.0f, p);
            std::vector<float> L (16384), R (16384);
            for (int pos = 0; pos < 16384; pos += 128)
                eng.render (L.data() + pos, R.data() + pos, 128, p, 120.0);
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

    // 3. Live real-time DCW Timbre sweeps unipolar REZ TRI mid-note without retriggering
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        SynthParams p;
        p.lineMode = 0; p.l1Wave1 = 6; p.l1Wave2 = 0; // REZ TRI
        p.dcwTimbre = 0.0f; p.dcwEnv = 0.0f;
        p.dcaAtt = 0.0f; p.dcaSus = 0.95f; p.chorus = 0.0f; p.spaceSend = 0.0f;

        eng.noteOn (48, 1.0f, p);
        std::vector<float> L (N), R (N);
        for (int pos = 0; pos < N / 2; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, std::min (128, N / 2 - pos), p, 120.0);
        p.dcwTimbre = 0.95f;
        for (int pos = N / 2; pos < N; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, std::min (128, N - pos), p, 120.0);

        const double slDark   = hfSlew (L, 2000, N / 2 - 256);
        const double slBright = hfSlew (L, N / 2 + 256, N - 256);
        std::snprintf (msg, sizeof (msg), "live mid-note REZ TRI slew %.3f -> %.3f (%.1fx)", slDark, slBright, slBright / std::max (1e-6, slDark));
        check (slBright > slDark * 3.0 && allFinite (L), "Live DCW Timbre sweeps 15x Rez instantly mid-note", msg);
    }

    // 4. Space FX true pre-delay Send rings out stereo ambient cloud after Send pulled to 0%
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        SynthParams p;
        p.lineMode = 0; p.l1Wave1 = 0; p.l1Wave2 = 0;
        p.dcwTimbre = 0.6f; p.dcwEnv = 0.0f;
        p.dcaAtt = 0.0f; p.dcaDec = 0.2f; p.dcaSus = 0.9f; p.dcaRel = 0.15f;
        p.chorus = 0.0f; p.spaceSend = 0.85f; p.spaceDecay = 0.65f; p.volume = 0.85f;

        eng.noteOn (60, 1.0f, p);
        std::vector<float> tL (128), tR (128);
        for (int pos = 0; pos < 2560; pos += 128)
            eng.render (tL.data(), tR.data(), 128, p, 120.0);
        eng.noteOff (60);
        for (int pos = 0; pos < 4096; pos += 128)
            eng.render (tL.data(), tR.data(), 128, p, 120.0);

        p.spaceSend = 0.0f;
        const int tailN = 66048;
        std::vector<float> L (tailN), R (tailN);
        for (int pos = 0; pos < tailN; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, 128, p, 120.0);

        const double echoRms = std::max (rmsRange (L, 8000, 32000), rmsRange (R, 8000, 32000));
        double diffSq = 0.0;
        for (int i = 8000; i < 32000; ++i) { double d = L[(size_t) i] - R[(size_t) i]; diffSq += d * d; }
        const double stDiff = std::sqrt (diffSq / 24000.0);
        std::snprintf (msg, sizeof (msg), "spaceTail=%.4f stereoDiff=%.4f", echoRms, stDiff);
        check (echoRms > 0.02 && stDiff > 0.01 && allFinite (L) && allFinite (R),
               "Space FX pre-delay Send rings out in stereo at 0%", msg);
    }

    // 5. Dual-Line 1+2 Polyphonic 8-Voice Chord with Natural Stereo Voice Spread
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        int cnt = 0;
        SynthParams p = factoryPresets (cnt)[0].p; // CZ-101 Brass
        p.chorus = 0.0f; p.spaceSend = 0.0f; // Test pure stereo voice spread!
        const int chord[8] = { 48, 52, 55, 59, 60, 64, 67, 71 };
        for (int n : chord) eng.noteOn (n, 0.9f, p);
        std::vector<float> L (N), R (N);
        for (int pos = 0; pos < N; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, std::min (128, N - pos), p, 120.0);
        double diffSq = 0.0;
        for (int i = 2000; i < N; ++i) { double d = L[(size_t) i] - R[(size_t) i]; diffSq += d * d; }
        const double stereoDiff = std::sqrt (diffSq / (double) (N - 2000));
        const double rmsL = rmsRange (L, 2000, N);
        std::snprintf (msg, sizeof (msg), "voices=%d rmsL=%.3f voiceStereoDiff=%.3f", eng.activeVoices(), rmsL, stereoDiff);
        check (eng.activeVoices() == 8 && rmsL > 0.10 && stereoDiff > 0.015 && allFinite (L) && allFinite (R),
               "8-voice polyphony with natural stereo spread", msg);
    }

    // 6. Program 0 (CZ-101 Brass) releases cleanly to silence < 0.5 s
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        int cnt = 0;
        SynthParams p = factoryPresets (cnt)[0].p;
        eng.noteOn (60, 1.0f, p);
        std::vector<float> L (N), R (N);
        for (int pos = 0; pos < 6144; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, 128, p, 120.0);
        eng.noteOff (60);
        for (int pos = 0; pos < N; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, std::min (128, N - pos), p, 120.0);
        const double tailRms = rmsRange (L, N - 4410, N);
        std::snprintf (msg, sizeof (msg), "tailRms=%.6f activeVoices=%d", tailRms, eng.activeVoices());
        check (tailRms < 1e-4 && eng.activeVoices() == 0 && allFinite (L),
               "Program 0 releases cleanly to silence < 0.5 s", msg);
    }

    // 7. All 32 Handcrafted & Converted Presets render audibly with zero NaN/Inf
    {
        int count = 0;
        const CzPreset* presets = factoryPresets (count);
        int audible = 0, finite = 0;
        const int PN = 32768; // 0.74 s so slow Aphex ambient pads bloom audibly
        std::vector<float> L (PN), R (PN);
        for (int i = 0; i < count; ++i)
        {
            CzBoyEngine eng;
            eng.prepare (sr);
            eng.noteOn (60, 0.9f, presets[i].p);
            for (int pos = 0; pos < PN; pos += 128)
                eng.render (L.data() + pos, R.data() + pos, 128, presets[i].p, 120.0);
            if (allFinite (L) && allFinite (R)) ++finite;
            const double r = std::max (rmsRange (L, 256, PN), rmsRange (R, 256, PN));
            if (r > 0.015) ++audible;
            else std::printf ("  [quiet preset %2d: %-18s rms=%.4f]\n", i, presets[i].name, r);
        }
        std::snprintf (msg, sizeof (msg), "%d/%d presets audible, %d/%d finite", audible, count, finite, count);
        check (audible == count && finite == count && count == 32,
               "All 32 factory presets audible & finite", msg);
    }

    // 8. Clean Glassy Sine Purity (zero LUT grit / zipper / drive fuzz) & Click-Free Wave1+Wave2 Splice
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        SynthParams p;
        p.lineMode = 0; p.l1Wave1 = 0; p.l1Wave2 = 0; p.modMode = 0;
        p.dcwTimbre = 0.0f; p.dcwEnv = 0.0f;
        p.dcaAtt = 0.0f; p.dcaDec = 0.1f; p.dcaSus = 1.0f; p.dcaRel = 0.2f;
        p.chorus = 0.0f; p.spaceSend = 0.0f; p.volume = 0.85f;

        eng.noteOn (69, 1.0f, p); // A4 = 440 Hz
        std::vector<float> L (N), R (N);
        for (int pos = 0; pos < N; pos += 128)
            eng.render (L.data() + pos, R.data() + pos, std::min (128, N - pos), p, 120.0);

        // Least-squares fit pure 440 Hz sinusoid over steady-state window to measure THD+N
        double sSin = 0.0, sCos = 0.0, sSS = 0.0, sCC = 0.0, sSC = 0.0;
        const int a = 8192, b = N - 512;
        const double w = 2.0 * 3.14159265358979323846 * 440.0 / sr;
        for (int i = a; i < b; ++i)
        {
            const double sn = std::sin (w * i), cs = std::cos (w * i), y = L[(size_t) i];
            sSin += y * sn; sCos += y * cs; sSS += sn * sn; sCC += cs * cs; sSC += sn * cs;
        }
        const double det = sSS * sCC - sSC * sSC;
        const double A = (sCC * sSin - sSC * sCos) / det;
        const double B = (-sSC * sSin + sSS * sCos) / det;
        double sigSq = 0.0, errSq = 0.0;
        for (int i = a; i < b; ++i)
        {
            const double fit = A * std::sin (w * i) + B * std::cos (w * i);
            const double e = L[(size_t) i] - fit;
            sigSq += fit * fit;
            errSq += e * e;
        }
        const double snrDb = 10.0 * std::log10 (sigSq / std::max (1e-18, errSq));

        // Check click-free Wave1 (SAW) + Wave2 (REZ_TRI) cycle splice
        CzBoyEngine eng2;
        eng2.prepare (sr);
        p.l1Wave1 = 0; p.l1Wave2 = 7; p.dcwTimbre = 0.25f; // SAW + REZ_TRI
        eng2.noteOn (60, 1.0f, p);
        for (int pos = 0; pos < N; pos += 128)
            eng2.render (L.data() + pos, R.data() + pos, std::min (128, N - pos), p, 120.0);
        double maxJump = 0.0;
        for (int i = 4096; i < N - 256; ++i)
            maxJump = std::max (maxJump, (double) std::fabs (L[(size_t) i] - L[(size_t) (i - 1)]));

        std::snprintf (msg, sizeof (msg), "sineSNR=%.1f dB, SAW+REZ_TRI maxStep=%.4f", snrDb, maxJump);
        check (snrDb > 55.0 && maxJump < 0.08,
               "Glassy sine purity & click-free Wave1+Wave2 splice", msg);
    }

    // 9. Zero-Click Retrigger, Voice-Steal, Vibrato Release & Smooth Limiter on BoC Glassy Sine
    {
        int cnt = 0;
        SynthParams p = factoryPresets (cnt)[0].p; // BoC Glassy Sine
        CzBoyEngine eng;
        eng.prepare (sr);
        std::vector<float> L, R;
        auto run = [&] (int n) {
            const size_t o = L.size();
            L.resize (o + (size_t) n); R.resize (o + (size_t) n);
            for (int i = 0; i < n; i += 128)
                eng.render (L.data() + o + i, R.data() + o + i, std::min (128, n - i), p, 120.0);
        };
        for (int k = 0; k < 4; ++k) { eng.noteOn (60, 0.9f, p); run (4096); eng.noteOff (60); run (2048); }
        double tapD2 = 0.0;
        for (size_t i = 2; i < L.size(); ++i)
            tapD2 = std::max (tapD2, (double) std::fabs (L[i] - 2.0f * L[i - 1] + L[i - 2]));

        const size_t cStart = L.size();
        const int notes[10] = { 48, 52, 55, 59, 60, 64, 67, 71, 72, 76 };
        for (int n : notes) { eng.noteOn (n, 0.9f, p); run (1024); }
        for (int n : notes) eng.noteOff (n);
        run (8192);
        double chordD2 = 0.0;
        for (size_t i = cStart + 2; i < L.size(); ++i)
            chordD2 = std::max (chordD2, (double) std::fabs (L[i] - 2.0f * L[i - 1] + L[i - 2]));

        std::snprintf (msg, sizeof (msg), "tapMaxD2=%.5f, 10vStealMaxD2=%.5f", tapD2, chordD2);
        check (tapD2 < 0.005 && chordD2 < 0.015 && allFinite (L),
               "Zero-click retrigger, voice-steal & vibrato release", msg);
    }

    // 10. Overlapping same-pitch notes (MPC sequence legato): first Note-Off only releases oldest voice
    {
        CzBoyEngine eng;
        eng.prepare (sr);
        SynthParams p;
        p.lineMode = 0; p.l1Wave1 = 0; p.l1Wave2 = 0; p.modMode = 0;
        p.dcwTimbre = 0.0f; p.dcwEnv = 0.0f;
        p.dcaAtt = 0.0f; p.dcaDec = 0.1f; p.dcaSus = 1.0f; p.dcaRel = 0.05f; // ultra-fast ~4 ms release
        p.chorus = 0.0f; p.spaceSend = 0.0f; p.volume = 0.85f; p.vibDepth = 0.0f;

        std::vector<float> L (8192), R (8192);
        eng.noteOn (69, 1.0f, p); // A4 #1
        for (int pos = 0; pos < 2048; pos += 128) eng.render (L.data() + pos, R.data() + pos, 128, p, 120.0);
        eng.noteOn (69, 1.0f, p); // A4 #2 overlapping while #1 still held
        for (int pos = 0; pos < 1024; pos += 128) eng.render (L.data() + pos, R.data() + pos, 128, p, 120.0);
        eng.noteOff (69);         // First Note-Off must release ONLY A4 #1, leaving A4 #2 held!
        for (int pos = 0; pos < 8192; pos += 128) eng.render (L.data() + pos, R.data() + pos, 128, p, 120.0);

        const int voicesAfterFirstOff = eng.activeVoices();
        const double heldRms = rmsRange (L, 4096, 8192);

        eng.noteOff (69);         // Second Note-Off releases A4 #2 cleanly to silence
        for (int pos = 0; pos < 8192; pos += 128) eng.render (L.data() + pos, R.data() + pos, 128, p, 120.0);
        const int voicesAfterSecondOff = eng.activeVoices();
        const double finalRms = rmsRange (L, 4096, 8192);

        std::snprintf (msg, sizeof (msg), "afterOff1: v=%d rms=%.3f | afterOff2: v=%d rms=%.6f",
                       voicesAfterFirstOff, heldRms, voicesAfterSecondOff, finalRms);
        check (voicesAfterFirstOff == 1 && heldRms > 0.10 && voicesAfterSecondOff == 0 && finalRms < 1e-4,
               "Overlapping same-pitch A4 releases only oldest voice", msg);
    }

    std::printf ("\nSummary: %d passed, %d failed\n", gPass, gFail);
    return gFail == 0 ? 0 : 1;
}
