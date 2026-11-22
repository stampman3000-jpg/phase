// CZboy — Casio CZ-Series Phase Distortion Synthesizer for Akai MPC.
// Built on CrispyZebra fixed-point C++ engine & official Akai MPC Plugin Builder SDK.
// Copyright (C) 2026 Stampman3000 — GNU General Public License v3.0 or later.

#include "../../../bridge/CzBoyPresets.h"
#include "../../../dsp/CzBoyEngine.h"
#include "mpc_plugin.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace czboy {

enum ParamIdx : int
{
    P_VOLUME = 0, // Harmless master level when MPC nudges param 0 on load

    // Page 1: Core CZ Voice, Waveforms, Live DCW Timbre & ADSR Envelopes (1..15 + 0)
    P_LINE_MODE = 1,
    P_L1_WAVE1,
    P_L1_WAVE2,
    P_MOD_MODE,
    P_DCW_TIMBRE,
    P_DCW_ATT,
    P_DCW_DEC,
    P_DCW_SUS,
    P_DCA_ATT,
    P_DCA_DEC,
    P_DCA_SUS,
    P_DCA_REL,
    P_DCW_REL,
    P_CHORUS,
    P_DCW_ENV,

    // Page 2: Line 2 Layer, Detune, Pitch EG, Vibrato & Voice (16..31)
    P_L2_WAVE1 = 16,
    P_L2_WAVE2,
    P_L2_OCT,
    P_L2_SEMI,
    P_L2_FINE,
    P_L2_MIX,
    P_L2_DCW,
    P_L2_ENV_OFS,
    P_PITCH_ENV,
    P_PITCH_DEC,
    P_VIB_RATE,
    P_VIB_DEPTH,
    P_VOICE_MODE,
    P_GLIDE,
    P_VIB_DELAY,
    P_VEL_SENS,

    // Non-automatable UI Telemetry (32..35)
    P_METER_L = 32,
    P_METER_R = 33,
    P_STAT_INFO = 34,
    P_STAT_WAVE = 35,

    P_COUNT = 36
};

class CzBoyPlugin final : public mpc::Plugin
{
public:
    CzBoyPlugin() : mpc::Plugin (makeInfo()), engine_ (std::make_unique<CzBoyEngine>())
    {
        registerAllParams();
        registerFactoryPresets();

        for (auto& d : uiDirty_) d.store (false, std::memory_order_relaxed);
        setProgram (0);
        worker_ = std::thread ([this] { uiWorkerLoop(); });
    }

    ~CzBoyPlugin() override
    {
        stopWorker_.store (true, std::memory_order_release);
        if (worker_.joinable()) worker_.join();
    }

    static mpc::PluginInfo makeInfo()
    {
        mpc::PluginInfo info;
        info.name         = "CZboy";
        info.vendor       = "Stampman3000";
        info.uniqueId     = MPC_FOURCC ('C', 'Z', 'B', 'Y');
        info.version      = 1001;
        info.isInstrument = true;
        info.numInputs    = 0;
        info.numOutputs   = 2;
        info.wantsMidi    = true;
        info.sendsMidi    = false;
        info.latencySamples = 0;
        info.tailSamples  = 44100 * 2;
        return info;
    }

    void prepare (double sr, int) override
    {
        engine_->prepare (sr > 8000.0 ? sr : 44100.0);
        readyForHost_.store (true, std::memory_order_release);
    }

    void reset() override
    {
        engine_->forceSilence();
    }

    void parameterChanged (int index, float) override
    {
        if (index == P_LINE_MODE || index == P_L1_WAVE1 || index == P_L1_WAVE2 ||
            index == P_MOD_MODE || index == P_VOICE_MODE || index == P_L2_WAVE1)
        {
            bumpStatus (P_STAT_INFO);
            bumpStatus (P_STAT_WAVE);
        }
    }

    bool formatValue (int index, float plainValue, std::string& out) override
    {
        char buf[64];
        if (index == P_STAT_INFO)
        {
            const char* pname = programName (currentProgram());
            static const char* kLines[4] = { "L1", "L2", "1+2", "1+2'" };
            const int lm = clampi (getInt (P_LINE_MODE), 0, 3);
            const int av = activeVoicesAtomic_.load (std::memory_order_relaxed);
            std::snprintf (buf, sizeof (buf), "%.17s | %s | V:%d", pname ? pname : "CZboy", kLines[lm], av);
            out = buf;
            return true;
        }
        if (index == P_STAT_WAVE)
        {
            static const char* kWaves[8]  = { "SAW", "SQR", "PLS", "DSIN", "S-PL", "RZ-S", "RZ-T", "RZ-P" };
            static const char* kVoices[3] = { "POLY8", "POLY4", "MONO" };
            static const char* kMods[4]   = { "", " RING", " NOISE", " R+N" };
            const int w1 = clampi (getInt (P_L1_WAVE1), 0, 7);
            const int w2 = clampi (getInt (P_L1_WAVE2), 0, 8);
            const int vm = clampi (getInt (P_VOICE_MODE), 0, 2);
            const int mm = clampi (getInt (P_MOD_MODE), 0, 3);
            if (w2 > 0)
                std::snprintf (buf, sizeof (buf), "%s+%s | %s%s", kWaves[w1], kWaves[w2 - 1], kVoices[vm], kMods[mm]);
            else
                std::snprintf (buf, sizeof (buf), "%s | %s%s", kWaves[w1], kVoices[vm], kMods[mm]);
            out = buf;
            return true;
        }
        if (index == P_L2_SEMI)
        {
            const int v = (int) std::lround (plainValue);
            std::snprintf (buf, sizeof (buf), "%+d st", v);
            out = buf;
            return true;
        }
        if (index == P_L2_FINE)
        {
            const int v = (int) std::lround (plainValue);
            std::snprintf (buf, sizeof (buf), "%+d ct", v);
            out = buf;
            return true;
        }
        if (index == P_L2_ENV_OFS)
        {
            const int v = (int) std::lround (plainValue);
            std::snprintf (buf, sizeof (buf), "%+d%%", v);
            out = buf;
            return true;
        }
        return false;
    }

    void process (mpc::ProcessContext& ctx) override
    {
        const int n = ctx.numFrames;
        if (n <= 0 || ! ctx.outputs || ctx.numOutputs < 1 || ! ctx.outputs[0]) return;
        float* outL = ctx.outputs[0];
        float* outR = (ctx.numOutputs >= 2 && ctx.outputs[1]) ? ctx.outputs[1] : outL;

        const SynthParams p = snapshotParams();

        int cursor = 0;
        for (int e = 0; e < ctx.numMidiEvents; ++e)
        {
            const auto& ev = ctx.midi[e];
            const int target = clampi (ev.sampleOffset, cursor, n);
            if (target > cursor)
            {
                engine_->render (outL + cursor, outR + cursor, target - cursor, p);
                cursor = target;
            }
            handleMidi (ev, p);
        }
        if (cursor < n)
            engine_->render (outL + cursor, outR + cursor, n - cursor, p);

        const int mL = std::min (7, (int) (std::sqrt (clampf (engine_->peakL() * 1.25f, 0.0f, 1.0f)) * 7.6f));
        const int mR = std::min (7, (int) (std::sqrt (clampf (engine_->peakR() * 1.25f, 0.0f, 1.0f)) * 7.6f));
        meterLAtomic_.store (mL, std::memory_order_relaxed);
        meterRAtomic_.store (mR, std::memory_order_relaxed);
        activeVoicesAtomic_.store (engine_->activeVoices(), std::memory_order_relaxed);
    }

private:
    void handleMidi (const mpc::MidiEvent& ev, const SynthParams& p)
    {
        if (ev.isNoteOn())
        {
            engine_->noteOn (ev.note(), (float) ev.velocity() / 127.0f, p);
        }
        else if (ev.isNoteOff())
        {
            engine_->noteOff (ev.note());
        }
        else if (ev.isPitchBend())
        {
            const int raw = (int) (ev.data[1] & 0x7F) | ((int) (ev.data[2] & 0x7F) << 7);
            engine_->setPitchBend ((float) (raw - 8192) / 8192.0f);
        }
        else if (ev.isController())
        {
            const int cc  = ev.controller();
            const int val = ev.data[2] & 0x7F;
            if (cc == 1)       engine_->setModWheel ((float) val / 127.0f);
            else if (cc == 64) engine_->setSustain (val >= 64);
            else if (cc == 120 || cc == 123) engine_->allNotesOff();
        }
    }

    SynthParams snapshotParams() const
    {
        SynthParams p;
        p.volume    = get (P_VOLUME) * 0.01f;
        p.lineMode  = getInt (P_LINE_MODE);
        p.l1Wave1   = getInt (P_L1_WAVE1);
        p.l1Wave2   = getInt (P_L1_WAVE2);
        p.modMode   = getInt (P_MOD_MODE);

        p.dcwTimbre = get (P_DCW_TIMBRE) * 0.01f;
        p.dcwAtt    = get (P_DCW_ATT) * 0.01f;
        p.dcwDec    = get (P_DCW_DEC) * 0.01f;
        p.dcwSus    = get (P_DCW_SUS) * 0.01f;
        p.dcwRel    = get (P_DCW_REL) * 0.01f;

        p.dcaAtt    = get (P_DCA_ATT) * 0.01f;
        p.dcaDec    = get (P_DCA_DEC) * 0.01f;
        p.dcaSus    = get (P_DCA_SUS) * 0.01f;
        p.dcaRel    = get (P_DCA_REL) * 0.01f;

        p.chorus    = get (P_CHORUS) * 0.01f;
        p.dcwEnv    = get (P_DCW_ENV) * 0.01f;

        p.l2Wave1   = getInt (P_L2_WAVE1);
        p.l2Wave2   = getInt (P_L2_WAVE2);
        p.l2Oct     = getInt (P_L2_OCT) - 2;
        p.l2Semi    = getInt (P_L2_SEMI);
        p.l2Fine    = getInt (P_L2_FINE);
        p.l2Mix     = get (P_L2_MIX) * 0.01f;
        p.l2Dcw     = get (P_L2_DCW) * 0.01f;
        p.l2EnvOfs  = get (P_L2_ENV_OFS) * 0.01f;

        p.pitchEnv  = get (P_PITCH_ENV) * 0.01f;
        p.pitchDec  = get (P_PITCH_DEC) * 0.01f;
        p.vibRate   = get (P_VIB_RATE) * 0.01f;
        p.vibDepth  = get (P_VIB_DEPTH) * 0.01f;
        p.voiceMode = getInt (P_VOICE_MODE);
        p.glide     = get (P_GLIDE) * 0.01f;
        p.vibDelay  = get (P_VIB_DELAY) * 0.01f;
        p.velSens   = get (P_VEL_SENS) * 0.01f;
        return p;
    }

    void registerAllParams()
    {
        using namespace mpc;
        auto addNonAuto = [this] (mpc::Param pd) { pd.automatable = false; addParam (std::move (pd)); };

        const std::vector<std::string> w1Names = {
            "Saw", "Square", "Pulse", "Dbl Sine", "Saw-Pulse", "Rez Saw", "Rez Tri", "Rez Trap"
        };
        const std::vector<std::string> w2Names = {
            "Off", "Saw", "Square", "Pulse", "Dbl Sine", "Saw-Pulse", "Rez Saw", "Rez Tri", "Rez Trap"
        };

        // 0: Master Volume
        addParam (floatParam  ("volume",     "Master Vol",   0.0f, 100.0f, 82.0f, "%", 0));

        // 1..15: Page 1 Core CZ Voice, Live DCW Timbre & ADSR Envelopes
        addParam (choiceParam ("line_mode",  "Line Mode",    { "Line 1", "Line 2", "1 + 2", "1 + 2'" }, 2));
        addParam (choiceParam ("l1_wave1",   "L1 Wave 1",    w1Names, 2));
        addParam (choiceParam ("l1_wave2",   "L1 Wave 2",    w2Names, 5));
        addParam (choiceParam ("mod_mode",   "Mod Mode",     { "Off", "Ring Mod", "Noise", "Ring+N" }, 0));
        addParam (floatParam  ("dcw_timbre", "DCW Timbre",   0.0f, 100.0f, 24.0f, "%", 0));
        addParam (floatParam  ("dcw_att",    "DCW Attack",   0.0f, 100.0f, 14.0f, "%", 0));
        addParam (floatParam  ("dcw_dec",    "DCW Decay",    0.0f, 100.0f, 42.0f, "%", 0));
        addParam (floatParam  ("dcw_sus",    "DCW Sustain",  0.0f, 100.0f, 48.0f, "%", 0));
        addParam (floatParam  ("dca_att",    "DCA Attack",   0.0f, 100.0f,  6.0f, "%", 0));
        addParam (floatParam  ("dca_dec",    "DCA Decay",    0.0f, 100.0f, 40.0f, "%", 0));
        addParam (floatParam  ("dca_sus",    "DCA Sustain",  0.0f, 100.0f, 88.0f, "%", 0));
        addParam (floatParam  ("dca_rel",    "DCA Release",  0.0f, 100.0f, 20.0f, "%", 0));
        addParam (floatParam  ("dcw_rel",    "DCW Release",  0.0f, 100.0f, 22.0f, "%", 0));
        addParam (floatParam  ("chorus",     "Chorus",       0.0f, 100.0f, 34.0f, "%", 0));
        addParam (floatParam  ("dcw_env",    "DCW Env",      0.0f, 100.0f, 76.0f, "%", 0));

        // 16..31: Page 2 Line 2 Layer, Detune, Pitch EG, Vibrato & Voice
        addParam (choiceParam ("l2_wave1",   "L2 Wave 1",    w1Names, 0));
        addParam (choiceParam ("l2_wave2",   "L2 Wave 2",    w2Names, 5));
        addParam (choiceParam ("l2_oct",     "L2 Octave",    { "-2", "-1", "0", "+1", "+2" }, 2));
        addParam (intParam    ("l2_semi",    "L2 Semi",      -12, 12, 0));
        addParam (intParam    ("l2_fine",    "L2 Fine",      -60, 60, 14));
        addParam (floatParam  ("l2_mix",     "L2 Mix",       0.0f, 100.0f, 90.0f, "%", 0));
        addParam (floatParam  ("l2_dcw",     "L2 DCW",       0.0f, 100.0f, 88.0f, "%", 0));
        addParam (floatParam  ("l2_env_ofs", "L2 Env Ofs", -100.0f, 100.0f, 16.0f, "%", 0));
        addParam (floatParam  ("pitch_env",  "Pitch Env",    0.0f, 100.0f,  7.0f, "%", 0));
        addParam (floatParam  ("pitch_dec",  "Pitch Dec",    0.0f, 100.0f, 11.0f, "%", 0));
        addParam (floatParam  ("vib_rate",   "Vib Rate",     0.0f, 100.0f, 34.0f, "%", 0));
        addParam (floatParam  ("vib_depth",  "Vib Depth",    0.0f, 100.0f,  8.0f, "%", 0));
        addParam (choiceParam ("voice_mode", "Voice Mode",   { "Poly 8", "Poly 4", "Mono" }, 0));
        addParam (floatParam  ("glide",      "Glide",        0.0f, 100.0f,  0.0f, "%", 0));
        addParam (floatParam  ("vib_delay",  "Vib Delay",    0.0f, 100.0f, 38.0f, "%", 0));
        addParam (floatParam  ("vel_sens",   "Vel Sens",     0.0f, 100.0f, 52.0f, "%", 0));

        // 32..35: Non-automatable UI Telemetry
        addNonAuto (intParam ("meter_l",   "Meter L", 0, 7, 0));
        addNonAuto (intParam ("meter_r",   "Meter R", 0, 7, 0));
        addNonAuto (intParam ("stat_info", "Info",    0, 999, 0));
        addNonAuto (intParam ("stat_wave", "Wave",    0, 999, 0));
    }

    void registerFactoryPresets()
    {
        int count = 0;
        const CzPreset* presets = factoryPresets (count);
        for (int i = 0; i < count; ++i)
        {
            const auto& p = presets[i].p;
            std::vector<float> vals (P_COUNT, 0.0f);
            vals[P_VOLUME]     = p.volume * 100.0f;
            vals[P_LINE_MODE]  = (float) p.lineMode;
            vals[P_L1_WAVE1]   = (float) p.l1Wave1;
            vals[P_L1_WAVE2]   = (float) p.l1Wave2;
            vals[P_MOD_MODE]   = (float) p.modMode;
            vals[P_DCW_TIMBRE] = p.dcwTimbre * 100.0f;
            vals[P_DCW_ATT]    = p.dcwAtt * 100.0f;
            vals[P_DCW_DEC]    = p.dcwDec * 100.0f;
            vals[P_DCW_SUS]    = p.dcwSus * 100.0f;
            vals[P_DCA_ATT]    = p.dcaAtt * 100.0f;
            vals[P_DCA_DEC]    = p.dcaDec * 100.0f;
            vals[P_DCA_SUS]    = p.dcaSus * 100.0f;
            vals[P_DCA_REL]    = p.dcaRel * 100.0f;
            vals[P_DCW_REL]    = p.dcwRel * 100.0f;
            vals[P_CHORUS]     = p.chorus * 100.0f;
            vals[P_DCW_ENV]    = p.dcwEnv * 100.0f;

            vals[P_L2_WAVE1]   = (float) p.l2Wave1;
            vals[P_L2_WAVE2]   = (float) p.l2Wave2;
            vals[P_L2_OCT]     = (float) (p.l2Oct + 2);
            vals[P_L2_SEMI]    = (float) p.l2Semi;
            vals[P_L2_FINE]    = (float) p.l2Fine;
            vals[P_L2_MIX]     = p.l2Mix * 100.0f;
            vals[P_L2_DCW]     = p.l2Dcw * 100.0f;
            vals[P_L2_ENV_OFS] = p.l2EnvOfs * 100.0f;
            vals[P_PITCH_ENV]  = p.pitchEnv * 100.0f;
            vals[P_PITCH_DEC]  = p.pitchDec * 100.0f;
            vals[P_VIB_RATE]   = p.vibRate * 100.0f;
            vals[P_VIB_DEPTH]  = p.vibDepth * 100.0f;
            vals[P_VOICE_MODE] = (float) p.voiceMode;
            vals[P_GLIDE]      = p.glide * 100.0f;
            vals[P_VIB_DELAY]  = p.vibDelay * 100.0f;
            vals[P_VEL_SENS]   = p.velSens * 100.0f;

            addProgram (presets[i].name, vals);
        }
    }

    void storeParamQuiet (int idx, float plain)
    {
        if (idx < 0 || idx >= numParams()) return;
        values_[(size_t) idx]->store (params_[(size_t) idx].toNormalised (plain), std::memory_order_relaxed);
    }

    void bumpStatus (int idx)
    {
        const int next = (getInt (idx) + 1) % 999;
        storeParamQuiet (idx, (float) next);
        uiDirty_[(size_t) idx].store (true, std::memory_order_release);
    }

    void uiWorkerLoop()
    {
        int prevML = -1, prevMR = -1, prevV = -1, prevProg = -1;
        int tick = 0;
        while (! stopWorker_.load (std::memory_order_acquire))
        {
            std::this_thread::sleep_for (std::chrono::milliseconds (5));
            if (stopWorker_.load (std::memory_order_acquire)) break;
            if (++tick < 6) continue;
            tick = 0;
            if (! readyForHost_.load (std::memory_order_acquire)) continue;

            const int prog = currentProgram();
            if (prog != prevProg)
            {
                prevProg = prog;
                bumpStatus (P_STAT_INFO);
                bumpStatus (P_STAT_WAVE);
            }
            const int mL = meterLAtomic_.load (std::memory_order_relaxed);
            if (mL != prevML)
            {
                prevML = mL;
                storeParamQuiet (P_METER_L, (float) mL);
                uiDirty_[P_METER_L].store (true, std::memory_order_release);
            }
            const int mR = meterRAtomic_.load (std::memory_order_relaxed);
            if (mR != prevMR)
            {
                prevMR = mR;
                storeParamQuiet (P_METER_R, (float) mR);
                uiDirty_[P_METER_R].store (true, std::memory_order_release);
            }
            const int av = activeVoicesAtomic_.load (std::memory_order_relaxed);
            if (av != prevV)
            {
                prevV = av;
                bumpStatus (P_STAT_INFO);
            }

            for (int i = 0; i < P_COUNT; ++i)
            {
                if (uiDirty_[(size_t) i].exchange (false, std::memory_order_acq_rel))
                    notifyHostParameterChanged (i);
            }
        }
    }

    std::unique_ptr<CzBoyEngine> engine_;
    std::atomic<bool> readyForHost_ { false };
    std::atomic<bool> stopWorker_ { false };
    std::atomic<int>  meterLAtomic_ { 0 };
    std::atomic<int>  meterRAtomic_ { 0 };
    std::atomic<int>  activeVoicesAtomic_ { 0 };
    std::array<std::atomic<bool>, P_COUNT> uiDirty_ {};
    std::thread worker_;
};

} // namespace czboy

using MpcCzBoyPlugin = czboy::CzBoyPlugin;
MPC_PLUGIN_ENTRY (MpcCzBoyPlugin)
