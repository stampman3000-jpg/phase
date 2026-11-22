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
    float volume    = 0.82f; // 0..1 (Param 0)

    // Page 1: Core CZ Voice, Waveforms, Live DCW Timbre & Envelopes
    int   lineMode  = 2;     // 0=Line1, 1=Line2, 2=1+2, 3=1+2'
    int   l1Wave1   = 2;     // 0..7 -> CrispyZebra::Waveform 1..8
    int   l1Wave2   = 5;     // 0=Off, 1..8 -> Waveform 1..8
    int   modMode   = 0;     // 0=Off, 1=Ring, 2=Noise, 3=Ring+Noise

    float dcwTimbre = 0.25f; // 0..1 live real-time Phase Distortion floor/cutoff
    float dcwAtt    = 0.14f; // 0..1 fast..slow
    float dcwDec    = 0.44f; // 0..1 fast..slow
    float dcwSus    = 0.45f; // 0..1 sustain level
    float dcwRel    = 0.30f; // 0..1 fast..slow

    float dcaAtt    = 0.06f; // 0..1 fast..slow
    float dcaDec    = 0.42f; // 0..1 fast..slow
    float dcaSus    = 0.85f; // 0..1 sustain level
    float dcaRel    = 0.24f; // 0..1 fast..slow

    float chorus    = 0.32f; // 0..1 CZ-5000 stereo BBD chorus
    float dcwEnv    = 0.72f; // 0..1 live real-time DCW EG depth

    // Page 2: Line 2 Layer, Detune, Pitch EG, Vibrato & Voice
    int   l2Wave1   = 0;     // 0..7 -> Waveform 1..8
    int   l2Wave2   = 5;     // 0=Off, 1..8 -> Waveform 1..8
    int   l2Oct     = 0;     // -2..+2
    int   l2Semi    = 0;     // -12..+12
    int   l2Fine    = 14;    // -60..+60 cents
    float l2Mix     = 0.85f; // 0..1 Line 2 level / Ring blend
    float l2Dcw     = 0.85f; // 0..1 Line 2 DCW trim relative to Line 1
    float l2EnvOfs  = 0.0f;  // -1..+1 bipolar Line 2 envelope time offset (faster pluck <-> slower swell)

    float pitchEnv  = 0.0f;  // 0..1 DCO pitch blip depth
    float pitchDec  = 0.18f; // 0..1 DCO pitch blip decay time
    float vibRate   = 0.34f; // 0..1
    float vibDepth  = 0.0f;  // 0..1
    int   voiceMode = 0;     // 0=Poly 8, 1=Poly 4, 2=Mono
    float glide     = 0.0f;  // 0..1 portamento
    float vibDelay  = 0.30f; // 0..1 vibrato delay
    float velSens   = 0.55f; // 0..1 velocity sensitivity
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
        lpCoef_ = 1.0f - std::exp (-kTwoPi * 7200.0f / sr_);
        reset();
    }

    void reset()
    {
        buf_.fill (0.0f);
        pos_ = 0;
        ph1_ = 0.0f;
        ph2_ = 0.37f;
        lpL_ = lpR_ = 0.0f;
    }

    inline void process (float* outL, float* outR, int n, float mix)
    {
        if (mix <= 0.005f)
        {
            for (int i = 0; i < n; ++i)
            {
                buf_[(size_t) pos_] = outL[i];
                pos_ = (pos_ + 1) & kMask;
            }
            return;
        }

        const float ms   = sr_ * 0.001f;
        const float inc1 = 0.52f / sr_;
        const float inc2 = 1.18f / sr_;
        const float wet  = 0.72f * mix;
        const float dry  = 1.0f - 0.18f * mix;

        for (int i = 0; i < n; ++i)
        {
            const float x = outL[i];
            buf_[(size_t) pos_] = x;

            ph1_ += inc1; if (ph1_ >= 1.0f) ph1_ -= 1.0f;
            ph2_ += inc2; if (ph2_ >= 1.0f) ph2_ -= 1.0f;

            const float t1L = 4.0f * std::fabs (ph1_ - 0.5f) - 1.0f;
            float p1R = ph1_ + 0.333f; if (p1R >= 1.0f) p1R -= 1.0f;
            const float t1R = 4.0f * std::fabs (p1R - 0.5f) - 1.0f;
            const float t2  = 4.0f * std::fabs (ph2_ - 0.5f) - 1.0f;

            const float tapL = 0.65f * readDelay ((7.4f + 2.8f * t1L) * ms)
                             + 0.35f * readDelay ((13.2f + 1.5f * t2) * ms);
            const float tapR = 0.65f * readDelay ((8.9f + 2.8f * t1R) * ms)
                             - 0.35f * readDelay ((12.1f - 1.5f * t2) * ms);

            lpL_ += (tapL - lpL_) * lpCoef_;
            lpR_ += (tapR - lpR_) * lpCoef_;
            if (std::fabs (lpL_) < 1e-24f) lpL_ = 0.0f;
            if (std::fabs (lpR_) < 1e-24f) lpR_ = 0.0f;

            outL[i] = x * dry + lpL_ * wet;
            outR[i] = x * dry + lpR_ * wet;
            pos_ = (pos_ + 1) & kMask;
        }
    }

private:
    inline float readDelay (float d) const
    {
        const float dc = clampf (d, 2.0f, (float) (kBuf - 4));
        const int di = (int) dc;
        const float frac = dc - (float) di;
        const int i0 = (pos_ - di) & kMask;
        const int i1 = (i0 - 1) & kMask;
        return buf_[(size_t) i0] + frac * (buf_[(size_t) i1] - buf_[(size_t) i0]);
    }

    float sr_ = 44100.0f;
    float lpCoef_ = 0.65f;
    std::array<float, kBuf> buf_ {};
    int   pos_ = 0;
    float ph1_ = 0.0f, ph2_ = 0.37f;
    float lpL_ = 0.0f, lpR_ = 0.0f;
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
        core_.setMasterDrive (8); // Subtle 1984 CZ analog output warmth
        core_.setPitchBendRangeUp (2);
        core_.setPitchBendRangeDown (2);
        core_.setVibratoWaveform (CrispyZebra::LfoWaveform::TRIANGLE);
        core_.setDcaCurveMode (CrispyZebra::DcaCurveMode::PSEUDO_EXP);
        chorus_.prepare ((float) sr_);
        paramsApplied_ = false;
        outPeakL_ = outPeakR_ = 0.0f;
    }

    void reset()
    {
        core_.forceSilence();
        chorus_.reset();
        outPeakL_ = outPeakR_ = 0.0f;
    }

    void allNotesOff()  { core_.allNotesOff(); }
    void forceSilence() { core_.forceSilence(); chorus_.reset(); }

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

    void render (float* outL, float* outR, int numSamples, const SynthParams& p)
    {
        if (numSamples <= 0) return;
        syncParams (p);

        if ((int) monoBuf_.size() < numSamples)
            monoBuf_.resize ((size_t) numSamples, 0.0f);

        core_.processBlock<float, false> (monoBuf_.data(), static_cast<uint32_t> (numSamples));

        for (int i = 0; i < numSamples; ++i)
        {
            float s = monoBuf_[(size_t) i];
            if (! std::isfinite (s) || std::fabs (s) < 1e-24f) s = 0.0f;
            outL[i] = s;
            outR[i] = s;
        }

        chorus_.process (outL, outR, numSamples, clampf (p.chorus, 0.0f, 1.0f));

        float pkL = 0.0f, pkR = 0.0f;
        for (int i = 0; i < numSamples; ++i)
        {
            const float l = clampf (outL[i], -0.99f, 0.99f);
            const float r = clampf (outR[i], -0.99f, 0.99f);
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
    static uint8_t timeToCzRate (float norm0To1)
    {
        const float t = clampf (norm0To1, 0.0f, 1.0f);
        const int r = 99 - static_cast<int> (std::lround (std::pow (t, 0.72f) * 74.0f));
        return static_cast<uint8_t> (clampi (r, 25, 99));
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
            && a.vibRate == b.vibRate && a.vibDepth == b.vibDepth && a.vibDelay == b.vibDelay
            && a.voiceMode == b.voiceMode && a.glide == b.glide && a.velSens == b.velSens;
    }

    void syncParams (const SynthParams& p)
    {
        if (paramsApplied_ && paramsEqual (p, lastParams_))
            return;
        lastParams_ = p;
        paramsApplied_ = true;

        // Line select & waveforms (1..8 in CrispyZebra, 0=off for wave2)
        core_.setLineSelect (static_cast<CrispyZebra::LineSelectMode> (clampi (p.lineMode, 0, 3)));
        core_.setLine1Wave1 (static_cast<uint8_t> (clampi (p.l1Wave1, 0, 7) + 1));
        core_.setLine1Wave2 (static_cast<uint8_t> (clampi (p.l1Wave2, 0, 8)));
        core_.setLine2Wave1 (static_cast<uint8_t> (clampi (p.l2Wave1, 0, 7) + 1));
        core_.setLine2Wave2 (static_cast<uint8_t> (clampi (p.l2Wave2, 0, 8)));

        // Ring & Noise modulation
        const int mm = clampi (p.modMode, 0, 3);
        core_.setRingModulation (mm == 1 || mm == 3);
        core_.setNoiseModulation (mm == 2 || mm == 3);

        // Line 2 Detune (exact signed cents decomposition)
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

        // Live real-time DCW Timbre (base floor) & DCW Env Depth + Line 2 scaling
        const float l2Dcw  = clampf (p.l2Dcw, 0.0f, 1.0f);
        const float timbre = clampf (p.dcwTimbre, 0.0f, 1.0f);
        const float envAmt = clampf (p.dcwEnv, 0.0f, 1.0f);
        core_.setDcwFloor    (1, static_cast<uint16_t> (timbre * 65535.0f));
        core_.setDcwFloor    (2, static_cast<uint16_t> (timbre * l2Dcw * 65535.0f));
        core_.setDcwEnvScale (1, static_cast<uint16_t> (envAmt * 65535.0f));
        core_.setDcwEnvScale (2, static_cast<uint16_t> (envAmt * l2Dcw * 65535.0f));
        core_.setLine2Mix    (static_cast<uint16_t> (clampf (p.l2Mix, 0.0f, 1.0f) * 65535.0f));

        // Bipolar Line 2 Envelope Offset (-1 = snappy transient/tine on L2; +1 = slow evolving swell on L2)
        const float ofs = clampf (p.l2EnvOfs, -1.0f, 1.0f);
        auto shiftTime = [ofs] (float baseT, bool isAttack) -> float {
            if (ofs < 0.0f)
            {
                // Faster transient/pluck on Line 2
                const float factor = 1.0f + ofs * (isAttack ? 0.85f : 0.72f);
                return clampf (baseT * factor, 0.0f, 1.0f);
            }
            // Slower blooming layer on Line 2
            return clampf (baseT + ofs * (isAttack ? 0.48f : 0.36f), 0.0f, 1.0f);
        };

        // DCW Envelopes (Line 1 normalized 0..99 peak, scaled live by dcw_env_scale!)
        const uint8_t dcwSusL = static_cast<uint8_t> (clampi ((int) std::lround (clampf (p.dcwSus, 0.0f, 1.0f) * 99.0f), 0, 99));
        // When L2 Env Offset is strongly negative, also lower L2 DCW & DCA sustain so L2 acts as a true transient/tine layer!
        const float l2SusScale = ofs < -0.05f ? clampf (1.0f + ofs * 0.88f, 0.08f, 1.0f) : 1.0f;
        const uint8_t dcwSusL2 = static_cast<uint8_t> (clampi ((int) std::lround ((float) dcwSusL * l2SusScale), 0, 99));

        core_.setEgRate (1, 1, 0, timeToCzRate (p.dcwAtt));
        core_.setEgLevel (1, 1, 0, 99);
        core_.setEgRate (1, 1, 1, timeToCzRate (p.dcwDec));
        core_.setEgLevel (1, 1, 1, dcwSusL);
        core_.setEgRate (1, 1, 2, timeToCzRate (p.dcwRel));
        core_.setEgLevel (1, 1, 2, 0);
        core_.setEgSustainPoint (1, 1, 1);
        core_.setEgEndPoint (1, 1, 2);

        core_.setEgRate (2, 1, 0, timeToCzRate (shiftTime (p.dcwAtt, true)));
        core_.setEgLevel (2, 1, 0, 99);
        core_.setEgRate (2, 1, 1, timeToCzRate (shiftTime (p.dcwDec, false)));
        core_.setEgLevel (2, 1, 1, dcwSusL2);
        core_.setEgRate (2, 1, 2, timeToCzRate (shiftTime (p.dcwRel, false)));
        core_.setEgLevel (2, 1, 2, 0);
        core_.setEgSustainPoint (2, 1, 1);
        core_.setEgEndPoint (2, 1, 2);

        // DCA Envelopes (Line 1 & independent Line 2 offset)
        const uint8_t dcaSusL  = static_cast<uint8_t> (clampi ((int) std::lround (clampf (p.dcaSus, 0.0f, 1.0f) * 99.0f), 0, 99));
        const uint8_t dcaSusL2 = static_cast<uint8_t> (clampi ((int) std::lround ((float) dcaSusL * l2SusScale), 0, 99));

        core_.setEgRate (1, 2, 0, timeToCzRate (p.dcaAtt));
        core_.setEgLevel (1, 2, 0, 99);
        core_.setEgRate (1, 2, 1, timeToCzRate (p.dcaDec));
        core_.setEgLevel (1, 2, 1, dcaSusL);
        core_.setEgRate (1, 2, 2, timeToCzRate (p.dcaRel));
        core_.setEgLevel (1, 2, 2, 0);
        core_.setEgSustainPoint (1, 2, 1);
        core_.setEgEndPoint (1, 2, 2);

        core_.setEgRate (2, 2, 0, timeToCzRate (shiftTime (p.dcaAtt, true)));
        core_.setEgLevel (2, 2, 0, 99);
        core_.setEgRate (2, 2, 1, timeToCzRate (shiftTime (p.dcaDec, false)));
        core_.setEgLevel (2, 2, 1, dcaSusL2);
        core_.setEgRate (2, 2, 2, timeToCzRate (shiftTime (p.dcaRel, false)));
        core_.setEgLevel (2, 2, 2, 0);
        core_.setEgSustainPoint (2, 2, 1);
        core_.setEgEndPoint (2, 2, 2);

        // DCO Pitch Envelope (0% = dead-accurate pitch; >0% = classic CZ upward pitch blip decaying to 0)
        const float pEnv = clampf (p.pitchEnv, 0.0f, 1.0f);
        const uint8_t pPeak = pEnv <= 0.005f ? 0 : static_cast<uint8_t> (clampi ((int) std::lround (pEnv * 99.0f), 1, 99));
        const uint8_t pDecR = timeToCzRate (p.pitchDec);
        for (int line = 1; line <= 2; ++line)
        {
            core_.setEgRate (line, 0, 0, 99);
            core_.setEgLevel (line, 0, 0, pPeak);
            core_.setEgRate (line, 0, 1, pDecR);
            core_.setEgLevel (line, 0, 1, 0);
            core_.setEgSustainPoint (line, 0, 0xFF);
            core_.setEgEndPoint (line, 0, 1);
        }

        // Vibrato LFO (classic Casio CZ Triangle vibrato with Rate, Depth & Delay)
        core_.setVibratoRate  (static_cast<uint16_t> (clampi ((int) std::lround (clampf (p.vibRate,  0.0f, 1.0f) * 99.0f), 0, 99)));
        core_.setVibratoDepth (static_cast<uint16_t> (clampi ((int) std::lround (clampf (p.vibDepth, 0.0f, 1.0f) * 99.0f), 0, 99)));
        core_.setVibratoDelay (static_cast<uint16_t> (clampi ((int) std::lround (clampf (p.vibDelay, 0.0f, 1.0f) * 99.0f), 0, 99)));

        // Voice mode, portamento, velocity, volume
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
        core_.setVelocitySensitivity (static_cast<uint8_t> (clampi ((int) std::lround (clampf (p.velSens, 0.0f, 1.0f) * 99.0f), 0, 99)));
        core_.setMasterVolume (static_cast<uint8_t> (clampi ((int) std::lround (clampf (p.volume, 0.0f, 1.0f) * 99.0f), 0, 99)));
    }

    double sr_ = 44100.0;
    std::array<int16_t, kLutSize> sinLut_ {};
    CrispyZebra::Engine<8> core_;
    CzStereoChorus chorus_;
    std::vector<float> monoBuf_;
    SynthParams lastParams_ {};
    bool paramsApplied_ = false;
    float outPeakL_ = 0.0f, outPeakR_ = 0.0f;
};

} // namespace czboy
