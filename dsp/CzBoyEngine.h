// CZboy — Casio CZ-Series Phase Distortion Synthesizer Engine for Akai MPC.
// Built on CrispyZebra fixed-point C++ PD engine (Copyright (C) 2026 Leo Kuroshita @kurogedelic, GPL-3.0).
// Copyright (C) 2026 Stampman3000 — GNU General Public License v3.0 or later.
#pragma once

#include "CrispyZebra.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace czboy {

static constexpr float kPi    = 3.14159265358979323846f;
static constexpr float kTwoPi = 6.28318530717958647692f;

inline float clampf (float x, float lo, float hi) noexcept { return x < lo ? lo : (x > hi ? hi : x); }
inline int   clampi (int x, int lo, int hi) noexcept       { return x < lo ? lo : (x > hi ? hi : x); }

struct SynthParams
{
    // Master / Output
    float volume     = 0.84f; // 0..1 (Param 0)

    // Page 1: Core CZ Voice, Waveforms, Live DCW Timbre & ADSR Envelopes
    int   lineMode   = 2;     // 0=Line1, 1=Line2, 2=1+2, 3=1+2'
    int   l1Wave1    = 2;     // 0..7 -> CrispyZebra::Waveform 1..8
    int   l1Wave2    = 5;     // 0=Off, 1..8 -> Waveform 1..8
    int   modMode    = 0;     // 0=Off, 1=Ring, 2=Noise, 3=Ring+Noise

    float dcwTimbre  = 0.24f; // 0..1 live real-time Phase Distortion floor/cutoff
    float dcwAtt     = 0.42f; // 0..1 -> 3 ms .. 6.6 s
    float dcwDec     = 0.62f; // 0..1 -> 3 ms .. 6.6 s
    float dcwSus     = 0.50f; // 0..1 sustain level
    float dcwRel     = 0.48f; // 0..1 -> 3 ms .. 6.6 s

    float dcaAtt     = 0.28f; // 0..1 -> 3 ms .. 6.6 s
    float dcaDec     = 0.60f; // 0..1 -> 3 ms .. 6.6 s
    float dcaSus     = 0.88f; // 0..1 sustain level
    float dcaRel     = 0.46f; // 0..1 -> 3 ms .. 6.6 s

    float chorus     = 0.36f; // 0..1 CZ-5000 dual-BBD stereo chorus
    float dcwEnv     = 0.76f; // 0..1 live real-time DCW EG depth

    // Page 2: Line 2 Layer, Detune, Pitch EG, Vibrato & Ambient Space FX
    int   l2Wave1    = 0;     // 0..7 -> Waveform 1..8
    int   l2Wave2    = 5;     // 0=Off, 1..8 -> Waveform 1..8
    int   l2Oct      = 0;     // -2..+2
    int   l2Semi     = 0;     // -12..+12
    int   l2Fine     = 14;    // -60..+60 cents
    float l2Mix      = 0.90f; // 0..1 Line 2 level / Ring blend
    float l2Dcw      = 0.88f; // 0..1 Line 2 DCW trim relative to Line 1
    float l2EnvOfs   = 0.16f; // -1..+1 bipolar Line 2 envelope offset (faster pluck <-> slower swell)

    float pitchEnv   = 0.0f;  // 0..1 DCO pitch blip depth
    float pitchDec   = 0.30f; // 0..1 DCO pitch blip decay time
    float vibRate    = 0.34f; // 0..1
    float vibDepth   = 0.0f;  // 0..1
    int   voiceMode  = 0;     // 0=Poly 8, 1=Poly 4, 2=Mono
    float glide      = 0.0f;  // 0..1 portamento
    float spaceSend  = 0.0f;  // 0..1 true pre-delay Send into warm diffused stereo Space FX
    float spaceDecay = 0.55f; // 0..1 Space echo feedback & ambient wash length
};

// Lush 1985 Casio CZ-5000 style dual-tap stereo BBD ensemble chorus (zero CPU when mix <= 0.005)
class CzStereoChorus
{
public:
    static constexpr int kBuf  = 2048;
    static constexpr int kMask = kBuf - 1;

    void prepare (float sr)
    {
        sr_ = sr > 8000.0f ? sr : 44100.0f;
        lpCoef_ = 1.0f - std::exp (-kTwoPi * 11000.0f / sr_);
        reset();
    }

    void reset()
    {
        bufL_.fill (0.0f); bufR_.fill (0.0f);
        pos_ = 0; ph1_ = 0.0f; ph2_ = 0.37f;
        lpL_ = lpR_ = 0.0f;
    }

    inline void process (float* outL, float* outR, int n, float mix)
    {
        if (mix <= 0.005f)
        {
            for (int i = 0; i < n; ++i)
            {
                bufL_[(size_t) pos_] = outL[i];
                bufR_[(size_t) pos_] = outR[i];
                pos_ = (pos_ + 1) & kMask;
            }
            return;
        }

        const float ms   = sr_ * 0.001f;
        const float inc1 = 0.52f / sr_;
        const float inc2 = 1.14f / sr_;
        const float wet  = 0.72f * mix;
        const float dry  = 1.0f - 0.16f * mix;

        for (int i = 0; i < n; ++i)
        {
            const float xL = outL[i];
            const float xR = outR[i];
            bufL_[(size_t) pos_] = xL;
            bufR_[(size_t) pos_] = xR;

            ph1_ += inc1; if (ph1_ >= 1.0f) ph1_ -= 1.0f;
            ph2_ += inc2; if (ph2_ >= 1.0f) ph2_ -= 1.0f;

            auto fastSin = [] (float ph01) -> float {
                const float u = ph01 * 2.0f - 1.0f; // -1..+1
                return 4.0f * u * (1.0f - std::fabs (u)); // smooth sine-like curve, zero corners
            };
            const float t1L = fastSin (ph1_);
            float p1R = ph1_ + 0.333f; if (p1R >= 1.0f) p1R -= 1.0f;
            const float t1R = fastSin (p1R);
            const float t2  = fastSin (ph2_);

            const float tapL = 0.65f * readDelay (bufL_, (7.2f + 2.8f * t1L) * ms)
                             + 0.35f * readDelay (bufR_, (13.4f + 1.6f * t2) * ms);
            const float tapR = 0.65f * readDelay (bufR_, (8.8f + 2.8f * t1R) * ms)
                             - 0.35f * readDelay (bufL_, (12.2f - 1.6f * t2) * ms);

            lpL_ += (tapL - lpL_) * lpCoef_;
            lpR_ += (tapR - lpR_) * lpCoef_;
            if (std::fabs (lpL_) < 1e-24f) lpL_ = 0.0f;
            if (std::fabs (lpR_) < 1e-24f) lpR_ = 0.0f;

            outL[i] = xL * dry + lpL_ * wet;
            outR[i] = xR * dry + lpR_ * wet;
            pos_ = (pos_ + 1) & kMask;
        }
    }

private:
    inline float readDelay (const std::array<float, kBuf>& b, float d) const
    {
        const float dc = clampf (d, 2.0f, (float) (kBuf - 4));
        const int di = (int) dc;
        const float frac = dc - (float) di;
        const int i0 = (pos_ - di) & kMask;
        const int i1 = (i0 - 1) & kMask;
        return b[(size_t) i0] + frac * (b[(size_t) i1] - b[(size_t) i0]);
    }

    float sr_ = 44100.0f, lpCoef_ = 0.68f;
    std::array<float, kBuf> bufL_ {}, bufR_ {};
    int   pos_ = 0;
    float ph1_ = 0.0f, ph2_ = 0.37f;
    float lpL_ = 0.0f, lpR_ = 0.0f;
};

// Lightweight warm 80s/Aphex ambient diffused stereo Space FX (true pre-delay Send, zero CPU when idle)
class CzSpaceFx
{
public:
    static constexpr int kMaxDly = 49152; // >1.1s at 44.1 kHz
    static constexpr int kAp1    = 1024;
    static constexpr int kAp2    = 1536;

    void prepare (float sr)
    {
        sr_ = sr > 8000.0f ? sr : 44100.0f;
        lpCoef_ = 1.0f - std::exp (-kTwoPi * 7800.0f / sr_);
        hpCoef_ = 1.0f - std::exp (-kTwoPi * 110.0f / sr_);
        apLen1_ = clampi ((int) (0.0173f * sr_), 128, kAp1 - 2);
        apLen2_ = clampi ((int) (0.0289f * sr_), 128, kAp2 - 2);
        reset();
    }

    void reset()
    {
        dlyL_.fill (0.0f); dlyR_.fill (0.0f);
        ap1L_.fill (0.0f); ap1R_.fill (0.0f);
        ap2L_.fill (0.0f); ap2R_.fill (0.0f);
        pos_ = ap1Pos_ = ap2Pos_ = tail_ = 0;
        lpL_ = lpR_ = hpL_ = hpR_ = 0.0f;
    }

    inline void process (float* bufL, float* bufR, int n, float sendNorm, float decayNorm, double bpm)
    {
        const float send = clampf (sendNorm, 0.0f, 1.0f);
        const float dec  = clampf (decayNorm, 0.0f, 1.0f);

        // Tempo-synced asymmetric stereo ping-pong (3/8 beat L, 1/2 beat R for lush wide ambient wash)
        const float beatSec = (float) (60.0 / std::max (50.0, std::min (240.0, bpm)));
        const int dL = clampi ((int) (beatSec * 0.75f * sr_), 512, kMaxDly - 2);
        const int dR = clampi ((int) (beatSec * 1.00f * sr_), 512, kMaxDly - 2);
        const int dMax = std::max (dL, dR);

        if (send > 0.002f)
            tail_ = std::max (tail_, dMax + 4096);

        const bool active = (send > 0.002f || tail_ > 0);
        if (! active)
        {
            for (int i = 0; i < n; ++i)
            {
                dlyL_[(size_t) pos_] = 0.0f;
                dlyR_[(size_t) pos_] = 0.0f;
                if (++pos_ >= kMaxDly) pos_ = 0;
            }
            return;
        }

        // Smooth feedback curve: 0.24 (tight room/slap) -> 0.84 (lush floating Aphex cloud, always stable)
        const float fb   = 0.24f + 0.60f * std::pow (dec, 0.85f);
        const float diff = 0.35f + 0.25f * dec;
        float peak = 0.0f;

        for (int i = 0; i < n; ++i)
        {
            const float inL = bufL[i];
            const float inR = bufR[i];

            // Pre-delay stereo diffuser (2 short prime allpasses soften echoes into hazy ambient space)
            const float sL = inL * send;
            const float sR = inR * send;

            const float a1L = ap1L_[(size_t) ap1Pos_];
            const float v1L = sL + a1L * diff;
            ap1L_[(size_t) ap1Pos_] = v1L;
            const float d1L = a1L - v1L * diff;

            const float a1R = ap1R_[(size_t) ap1Pos_];
            const float v1R = sR + a1R * diff;
            ap1R_[(size_t) ap1Pos_] = v1R;
            const float d1R = a1R - v1R * diff;

            if (++ap1Pos_ >= apLen1_) ap1Pos_ = 0;

            const float a2L = ap2L_[(size_t) ap2Pos_];
            const float v2L = d1L + a2L * diff;
            ap2L_[(size_t) ap2Pos_] = v2L;
            const float difL = a2L - v2L * diff;

            const float a2R = ap2R_[(size_t) ap2Pos_];
            const float v2R = d1R + a2R * diff;
            ap2R_[(size_t) ap2Pos_] = v2R;
            const float difR = a2R - v2R * diff;

            if (++ap2Pos_ >= apLen2_) ap2Pos_ = 0;

            int rL = pos_ - dL; if (rL < 0) rL += kMaxDly;
            int rR = pos_ - dR; if (rR < 0) rR += kMaxDly;
            const float tapL = dlyL_[(size_t) rL];
            const float tapR = dlyR_[(size_t) rR];

            // Warm tape/BBD low-pass + gentle mud-cutting HPF inside feedback loop
            lpL_ += (tapL - lpL_) * lpCoef_;
            lpR_ += (tapR - lpR_) * lpCoef_;
            hpL_ += (lpL_ - hpL_) * hpCoef_;
            hpR_ += (lpR_ - hpR_) * hpCoef_;
            float fL = lpL_ - hpL_;
            float fR = lpR_ - hpR_;
            if (std::fabs (fL) < 1e-24f) fL = 0.0f;
            if (std::fabs (fR) < 1e-24f) fR = 0.0f;

            // Cross-coupled stereo ping-pong feedback
            const float wL = difL + (0.72f * fR + 0.28f * fL) * fb;
            const float wR = difR + (0.72f * fL + 0.28f * fR) * fb;
            dlyL_[(size_t) pos_] = wL;
            dlyR_[(size_t) pos_] = wR;
            if (++pos_ >= kMaxDly) pos_ = 0;

            const float e = std::fabs (fL) + std::fabs (fR) + std::fabs (wL) + std::fabs (wR);
            if (e > peak) peak = e;

            // Blend diffused early reflection + lush cross-coupled ambient echoes
            bufL[i] = inL + (fL * 0.78f + difL * 0.22f);
            bufR[i] = inR + (fR * 0.78f + difR * 0.22f);
        }

        if (send <= 0.002f)
        {
            if (peak > 1e-5f)
                tail_ = std::max (tail_, dMax + 4096);
            else
            {
                tail_ = std::max (0, tail_ - n);
                if (tail_ == 0)
                    lpL_ = lpR_ = hpL_ = hpR_ = 0.0f;
            }
        }
    }

private:
    float sr_ = 44100.0f, lpCoef_ = 0.42f, hpCoef_ = 0.015f;
    std::array<float, kMaxDly> dlyL_ {}, dlyR_ {};
    std::array<float, kAp1>    ap1L_ {}, ap1R_ {};
    std::array<float, kAp2>    ap2L_ {}, ap2R_ {};
    int pos_ = 0, ap1Pos_ = 0, ap2Pos_ = 0, apLen1_ = 761, apLen2_ = 1277, tail_ = 0;
    float lpL_ = 0.0f, lpR_ = 0.0f, hpL_ = 0.0f, hpR_ = 0.0f;
};

class CzBoyEngine
{
public:
    static constexpr uint16_t kLutSize = 2048;

    CzBoyEngine()
    {
        for (uint16_t i = 0; i < kLutSize; ++i)
        {
            const float phase = (kTwoPi * (float) i) / (float) kLutSize;
            sinLut_[i] = static_cast<int16_t> (-std::cos (phase) * 32767.0f);
        }
    }

    void prepare (double sampleRate)
    {
        sr_ = sampleRate > 8000.0 ? sampleRate : 44100.0;
        core_.setup (sinLut_.data(), kLutSize, static_cast<uint32_t> (std::lround (sr_)));
        core_.setMasterPan (50);
        core_.setMasterOctave (1);
        core_.setMasterNote (0);
        core_.setMasterFine (30);
        core_.setMasterDrive (0);  // Studio-clean linear CZ-101 polyphonic bus (zero chord fuzz)
        core_.setPitchBendRangeUp (2);
        core_.setPitchBendRangeDown (2);
        core_.setVibratoWaveform (CrispyZebra::LfoWaveform::TRIANGLE);
        core_.setVibratoDelay (28); // Expressive ~0.28s vibrato ramp-in
        core_.setVelocitySensitivity (52);
        core_.setDcaCurveMode (CrispyZebra::DcaCurveMode::PSEUDO_EXP);
        chorus_.prepare ((float) sr_);
        space_.prepare ((float) sr_);
        paramsApplied_ = false;
        outPeakL_ = outPeakR_ = 0.0f;
    }

    void reset()
    {
        core_.forceSilence();
        chorus_.reset();
        space_.reset();
        outPeakL_ = outPeakR_ = 0.0f;
    }

    void allNotesOff()  { core_.allNotesOff(); }
    void forceSilence() { core_.forceSilence(); chorus_.reset(); space_.reset(); }

    void setPitchBend (float normMinus1To1)
    {
        const float c = clampf (normMinus1To1, -1.0f, 1.0f);
        const int16_t v = static_cast<int16_t> (c >= 0.0f ? c * 8191.0f : c * 8192.0f);
        core_.setPitchBend (v);
    }

    void setModWheel (float norm0To1)
    {
        core_.setModWheel (static_cast<uint8_t> (clampf (norm0To1, 0.0f, 1.0f) * 127.0f));
    }

    void setSustain (bool down) { core_.setSustainPedal (down); }

    void noteOn (int midiNote, float velocity, const SynthParams& p)
    {
        syncParams (p);
        const uint8_t n = static_cast<uint8_t> (clampi (midiNote, 0, 127));
        const uint8_t v = static_cast<uint8_t> (clampi ((int) std::lround (clampf (velocity, 0.05f, 1.0f) * 127.0f), 1, 127));
        core_.midiNoteOn (n, v);
    }

    void noteOff (int midiNote)
    {
        core_.midiNoteOff (static_cast<uint8_t> (clampi (midiNote, 0, 127)));
    }

    void render (float* outL, float* outR, int numSamples, const SynthParams& p, double bpm = 120.0)
    {
        if (numSamples <= 0) return;
        syncParams (p);

        core_.processBlockStereo (outL, outR, static_cast<uint32_t> (numSamples));
        chorus_.process (outL, outR, numSamples, clampf (p.chorus, 0.0f, 1.0f));
        space_.process (outL, outR, numSamples, clampf (p.spaceSend, 0.0f, 1.0f), clampf (p.spaceDecay, 0.0f, 1.0f), bpm);

        float pkL = 0.0f, pkR = 0.0f;
        for (int i = 0; i < numSamples; ++i)
        {
            float l = std::isfinite (outL[i]) ? outL[i] : 0.0f;
            float r = std::isfinite (outR[i]) ? outR[i] : 0.0f;
            // Smooth output limiter above 0.90 so big ambient chords + Space never clip harshly
            if (std::fabs (l) > 0.90f)
                l = (l > 0.0f ? 1.0f : -1.0f) * (0.90f + 0.09f * std::tanh ((std::fabs (l) - 0.90f) * 4.0f));
            if (std::fabs (r) > 0.90f)
                r = (r > 0.0f ? 1.0f : -1.0f) * (0.90f + 0.09f * std::tanh ((std::fabs (r) - 0.90f) * 4.0f));
            outL[i] = std::fabs (l) < 1e-24f ? 0.0f : l;
            outR[i] = std::fabs (r) < 1e-24f ? 0.0f : r;
            pkL = std::max (pkL, std::fabs (outL[i]));
            pkR = std::max (pkR, std::fabs (outR[i]));
        }

        outPeakL_ = std::max (pkL, outPeakL_ * 0.86f);
        outPeakR_ = std::max (pkR, outPeakR_ * 0.86f);
    }

    int   activeVoices() const { return core_.getActiveVoiceCount(); }
    float peakL() const        { return outPeakL_; }
    float peakR() const        { return outPeakR_; }

private:
    // Smooth logarithmic envelope time mapping: 0.0 = 3 ms, 0.5 = 140 ms, 0.8 = 1.4 s, 1.0 = 6.6 s
    uint32_t timeToStep (float norm0To1) const
    {
        const double t   = static_cast<double> (clampf (norm0To1, 0.0f, 1.0f));
        const double sec = 0.003 * std::pow (2200.0, t);
        const double updates = std::max (1.0, (sr_ / 32.0) * sec);
        const double step = 4294901760.0 / updates;
        if (step < 1.0) return 1u;
        if (step > 4294967295.0) return 0xFFFFFFFFu;
        return static_cast<uint32_t> (step);
    }

    static bool paramsEqual (const SynthParams& a, const SynthParams& b)
    {
        return a.volume == b.volume && a.lineMode == b.lineMode
            && a.l1Wave1 == b.l1Wave1 && a.l1Wave2 == b.l1Wave2 && a.modMode == b.modMode
            && a.dcwTimbre == b.dcwTimbre && a.dcwEnv == b.dcwEnv
            && a.dcwAtt == b.dcwAtt && a.dcwDec == b.dcwDec && a.dcwSus == b.dcwSus && a.dcwRel == b.dcwRel
            && a.dcaAtt == b.dcaAtt && a.dcaDec == b.dcaDec && a.dcaSus == b.dcaSus && a.dcaRel == b.dcaRel
            && a.chorus == b.chorus
            && a.l2Wave1 == b.l2Wave1 && a.l2Wave2 == b.l2Wave2
            && a.l2Oct == b.l2Oct && a.l2Semi == b.l2Semi && a.l2Fine == b.l2Fine
            && a.l2Mix == b.l2Mix && a.l2Dcw == b.l2Dcw && a.l2EnvOfs == b.l2EnvOfs
            && a.pitchEnv == b.pitchEnv && a.pitchDec == b.pitchDec
            && a.vibRate == b.vibRate && a.vibDepth == b.vibDepth
            && a.voiceMode == b.voiceMode && a.glide == b.glide
            && a.spaceSend == b.spaceSend && a.spaceDecay == b.spaceDecay;
    }

    void syncParams (const SynthParams& p)
    {
        if (paramsApplied_ && paramsEqual (p, lastParams_))
            return;
        lastParams_ = p;
        paramsApplied_ = true;

        core_.setLineSelect (static_cast<CrispyZebra::LineSelectMode> (clampi (p.lineMode, 0, 3)));
        core_.setLine1Wave1 (static_cast<uint8_t> (clampi (p.l1Wave1, 0, 7) + 1));
        core_.setLine1Wave2 (static_cast<uint8_t> (clampi (p.l1Wave2, 0, 8)));
        core_.setLine2Wave1 (static_cast<uint8_t> (clampi (p.l2Wave1, 0, 7) + 1));
        core_.setLine2Wave2 (static_cast<uint8_t> (clampi (p.l2Wave2, 0, 8)));

        const int mm = clampi (p.modMode, 0, 3);
        core_.setRingModulation (mm == 1 || mm == 3);
        core_.setNoiseModulation (mm == 2 || mm == 3);

        const int totalCents = clampi (p.l2Oct, -2, 2) * 1200
                             + clampi (p.l2Semi, -12, 12) * 100
                             + clampi (p.l2Fine, -60, 60);
        const int8_t sign = totalCents < 0 ? -1 : 1;
        const int absCents = std::abs (totalCents);
        const int oct  = std::min (3, absCents / 1200);
        const int rem1 = absCents - oct * 1200;
        const int semi = std::min (11, rem1 / 100);
        const int rem2 = rem1 - semi * 100;
        const int fine = std::min (60, (rem2 * 60 + 50) / 100);
        core_.setDetuneSign (sign);
        core_.setDetuneOctave (static_cast<uint8_t> (oct));
        core_.setDetuneNote (static_cast<uint8_t> (semi));
        core_.setDetuneFine (static_cast<uint8_t> (fine));

        // Live real-time DCW Timbre & DCW Env Depth
        const float l2Dcw  = clampf (p.l2Dcw, 0.0f, 1.0f);
        const float timbre = clampf (p.dcwTimbre, 0.0f, 1.0f);
        const float envAmt = clampf (p.dcwEnv, 0.0f, 1.0f);
        core_.setDcwFloor    (1, static_cast<uint16_t> (timbre * 65535.0f));
        core_.setDcwFloor    (2, static_cast<uint16_t> (timbre * l2Dcw * 65535.0f));
        core_.setDcwEnvScale (1, static_cast<uint16_t> (envAmt * 65535.0f));
        core_.setDcwEnvScale (2, static_cast<uint16_t> (envAmt * l2Dcw * 65535.0f));
        core_.setLine2Mix    (static_cast<uint16_t> (clampf (p.l2Mix, 0.0f, 1.0f) * 65535.0f));

        // Bipolar Line 2 Envelope Offset (-1 = snappy transient/tine; +1 = slow blooming pad layer)
        const float ofs = clampf (p.l2EnvOfs, -1.0f, 1.0f);
        auto shiftTime = [ofs] (float baseT, bool isAttack) -> float {
            if (ofs < 0.0f)
                return clampf (baseT * (1.0f + ofs * (isAttack ? 0.85f : 0.58f)), 0.0f, 1.0f);
            return clampf (baseT + ofs * (isAttack ? 0.34f : 0.24f), 0.0f, 1.0f);
        };

        const uint8_t dcwSusL  = static_cast<uint8_t> (clampi ((int) std::lround (clampf (p.dcwSus, 0.0f, 1.0f) * 99.0f), 0, 99));
        const float l2SusScale = ofs < -0.05f ? clampf (1.0f + ofs * 0.85f, 0.06f, 1.0f) : 1.0f;
        const uint8_t dcwSusL2 = static_cast<uint8_t> (clampi ((int) std::lround ((float) dcwSusL * l2SusScale), 0, 99));

        core_.setEgRateDirect (1, 1, 0, timeToStep (p.dcwAtt));
        core_.setEgLevel      (1, 1, 0, 99);
        core_.setEgRateDirect (1, 1, 1, timeToStep (p.dcwDec));
        core_.setEgLevel      (1, 1, 1, dcwSusL);
        core_.setEgRateDirect (1, 1, 2, timeToStep (p.dcwRel));
        core_.setEgLevel      (1, 1, 2, 0);
        core_.setEgSustainPoint (1, 1, 1);
        core_.setEgEndPoint     (1, 1, 2);

        core_.setEgRateDirect (2, 1, 0, timeToStep (shiftTime (p.dcwAtt, true)));
        core_.setEgLevel      (2, 1, 0, 99);
        core_.setEgRateDirect (2, 1, 1, timeToStep (shiftTime (p.dcwDec, false)));
        core_.setEgLevel      (2, 1, 1, dcwSusL2);
        core_.setEgRateDirect (2, 1, 2, timeToStep (shiftTime (p.dcwRel, false)));
        core_.setEgLevel      (2, 1, 2, 0);
        core_.setEgSustainPoint (2, 1, 1);
        core_.setEgEndPoint     (2, 1, 2);

        const uint8_t dcaSusL  = static_cast<uint8_t> (clampi ((int) std::lround (clampf (p.dcaSus, 0.0f, 1.0f) * 99.0f), 0, 99));
        const uint8_t dcaSusL2 = static_cast<uint8_t> (clampi ((int) std::lround ((float) dcaSusL * l2SusScale), 0, 99));

        core_.setEgRateDirect (1, 2, 0, timeToStep (p.dcaAtt));
        core_.setEgLevel      (1, 2, 0, 99);
        core_.setEgRateDirect (1, 2, 1, timeToStep (p.dcaDec));
        core_.setEgLevel      (1, 2, 1, dcaSusL);
        core_.setEgRateDirect (1, 2, 2, timeToStep (p.dcaRel));
        core_.setEgLevel      (1, 2, 2, 0);
        core_.setEgSustainPoint (1, 2, 1);
        core_.setEgEndPoint     (1, 2, 2);

        core_.setEgRateDirect (2, 2, 0, timeToStep (shiftTime (p.dcaAtt, true)));
        core_.setEgLevel      (2, 2, 0, 99);
        core_.setEgRateDirect (2, 2, 1, timeToStep (shiftTime (p.dcaDec, false)));
        core_.setEgLevel      (2, 2, 1, dcaSusL2);
        core_.setEgRateDirect (2, 2, 2, timeToStep (shiftTime (p.dcaRel, false)));
        core_.setEgLevel      (2, 2, 2, 0);
        core_.setEgSustainPoint (2, 2, 1);
        core_.setEgEndPoint     (2, 2, 2);

        // DCO Pitch Envelope
        const float pEnv = clampf (p.pitchEnv, 0.0f, 1.0f);
        const uint8_t pPeak = pEnv <= 0.005f ? 0 : static_cast<uint8_t> (clampi ((int) std::lround (pEnv * 99.0f), 1, 99));
        const uint32_t pDecStep = timeToStep (p.pitchDec);
        for (int line = 1; line <= 2; ++line)
        {
            core_.setEgRateDirect (line, 0, 0, 0xFFFFFFFFu);
            core_.setEgLevel      (line, 0, 0, pPeak);
            core_.setEgRateDirect (line, 0, 1, pDecStep);
            core_.setEgLevel      (line, 0, 1, 0);
            core_.setEgSustainPoint (line, 0, 0xFF);
            core_.setEgEndPoint     (line, 0, 1);
        }

        core_.setVibratoRate  (static_cast<uint16_t> (clampi ((int) std::lround (clampf (p.vibRate,  0.0f, 1.0f) * 99.0f), 0, 99)));
        core_.setVibratoDepth (static_cast<uint16_t> (clampi ((int) std::lround (clampf (p.vibDepth, 0.0f, 1.0f) * 99.0f), 0, 99)));

        const int vm = clampi (p.voiceMode, 0, 2);
        if (vm == 2)
        {
            core_.setMonoMode (true);
            core_.setMaxPolyphony (1);
        }
        else
        {
            core_.setMonoMode (false);
            core_.setMaxPolyphony (vm == 1 ? 4 : 8);
        }

        core_.setPortamentoEnabled (p.glide > 0.01f);
        core_.setPortamentoTime (static_cast<uint8_t> (clampi ((int) std::lround (clampf (p.glide, 0.0f, 1.0f) * 99.0f), 0, 99)));
        core_.setMasterVolume (static_cast<uint8_t> (clampi ((int) std::lround (clampf (p.volume, 0.0f, 1.0f) * 99.0f), 0, 99)));
    }

    double sr_ = 44100.0;
    std::array<int16_t, kLutSize> sinLut_ {};
    CrispyZebra::Engine<8> core_;
    CzStereoChorus chorus_;
    CzSpaceFx space_;
    SynthParams lastParams_ {};
    bool paramsApplied_ = false;
    float outPeakL_ = 0.0f, outPeakR_ = 0.0f;
};

} // namespace czboy
