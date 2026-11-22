/*
 * CrispyZebra.h
 * v1.0.0
 * Copyright (C) 2026 Leo Kuroshita @kurogedelic
 * https://github.com/hugelton/CrispyZebra/
 *
 * Modified 2026 by Stampman3000 for CZboy (Akai MPC ARMv7/ARM64 standalone):
 *   - Hoisted per-block pitch bend, drive, volume, and pan math out of the inner sample loop
 *   - Skipped inactive line evaluation in single-line modes (LINE1_ONLY / LINE2_ONLY)
 *   - Added runtime polyphony limit, MIDI velocity scaling, sustain pedal (CC64),
 *     mod-wheel vibrato depth, allNotesOff()/forceSilence(), and live waveform/detune updates
 *   - Fixed Voice::noteOff() DCO EG keyOff and block-accurate Voice::shouldRelease(numSamples)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>

#ifndef CRISPY_ZEBRA_DEFAULT_SAMPLE_RATE
#define CRISPY_ZEBRA_DEFAULT_SAMPLE_RATE 44100u
#endif

namespace CrispyZebra {

// Fixed-point arithmetic support macros and inline functions
inline int32_t multiply_q15(int32_t a, int32_t b) { return (a * b) >> 15; }
inline int32_t multiply_q16(int32_t a, int32_t b) { return (a * b) >> 16; }
static constexpr uint32_t DEFAULT_SAMPLE_RATE = CRISPY_ZEBRA_DEFAULT_SAMPLE_RATE;

enum class Waveform : uint8_t {
    SAW         = 1, // Sawtooth
    SQUARE      = 2, // Square wave
    PULSE       = 3, // Pulse wave
    DOUBLE_SINE = 4, // Double sine
    SAW_PULSE   = 5, // Saw-pulse
    REZ_SAW     = 6, // Resonance (sawtooth window)
    REZ_TRI     = 7, // Resonance (triangle window)
    REZ_TRAP    = 8, // Resonance (trapezoid window)
    NULL_WAVE   = 9, // Hidden SysEx waveform: flat/silent
    PULSE2      = 10, // Hidden SysEx waveform: alternate pulse
    REZ_PULSE   = 11, // Hidden SysEx resonance (pulse window)
    REZ_DBL_SAW = 12  // Hidden SysEx resonance (double-saw window)
};

inline Waveform decodeCzWaveformBits(uint8_t wave_code, uint8_t window_code) {
    switch (wave_code & 0x07) {
        case 0: return Waveform::SAW;
        case 1: return Waveform::SQUARE;
        case 2: return Waveform::PULSE;
        case 3: return Waveform::NULL_WAVE;
        case 4: return Waveform::DOUBLE_SINE;
        case 5: return Waveform::SAW_PULSE;
        case 6:
            switch (window_code & 0x07) {
                case 2: return Waveform::REZ_TRI;
                case 3: return Waveform::REZ_TRAP;
                case 4: return Waveform::REZ_PULSE;
                case 5:
                case 6:
                case 7: return Waveform::REZ_DBL_SAW;
                case 0:
                case 1:
                default: return Waveform::REZ_SAW;
            }
        case 7: return Waveform::PULSE2;
        default: return Waveform::SAW;
    }
}

inline Waveform decodeCzWaveformWord(uint16_t word, bool second_wave) {
    uint8_t wave_code = second_wave
        ? static_cast<uint8_t>((word >> 10) & 0x07)
        : static_cast<uint8_t>((word >> 13) & 0x07);
    uint8_t window_code = static_cast<uint8_t>((word >> 6) & 0x07);
    return decodeCzWaveformBits(wave_code, window_code);
}

enum class LineSelectMode : uint8_t {
    LINE1_ONLY  = 0, // LINE 1 only
    LINE2_ONLY  = 1, // LINE 2 only
    LINE1_2     = 2, // LINE 1 + 2 (normal)
    LINE1_2_INV  = 3 // LINE 1 + 2' (phase inverted)
};

enum class LfoWaveform : uint8_t {
    TRIANGLE    = 0, // Triangle wave
    SQUARE      = 1, // Square wave
    SAW_UP      = 2, // Upward sawtooth
    SAW_DOWN    = 3  // Downward sawtooth
};

enum class DcaCurveMode : uint8_t {
    LINEAR     = 0,
    PSEUDO_EXP = 1
};

// ==========================================
// EG
// ==========================================
class Envelope {
public:
    struct Stage {
        uint32_t rate;   // Speed (Q16 increment amount per sample)
        uint16_t level;  // Target value (0 to 65535)
    };

    Stage stages[8];
    uint32_t current_level = 0; // Q16 internal state
    uint8_t current_stage = 0;

    // Synth hardware-compatible point system for direct SysEx assignment
    uint8_t sustain_point = 0xFF; // 0-7, 0xFF = no sustain
    uint8_t end_point = 7;        // 0-7, end of envelope stage

    bool is_key_on = false;

    // Initialize with standard envelope defaults
    void initCz101Defaults() {
        stages[0] = {0xFFFFFFFF, 25000};
        stages[1] = {500000, 22000};
        stages[2] = {300000, 20000};
        stages[3] = {0, 20000};          // Sustain point (Rate=0 holds the stage)
        stages[4] = {200000, 10000};     // Release 1
        stages[5] = {200000, 0};         // Release 2
        stages[6] = {0, 0};              // End
        stages[7] = {0, 0};

        sustain_point = 3;
        end_point = 6;
    }

    void keyOn() {
        current_stage = 0;
        current_level = 0;
        is_key_on = true;
    }

    void keyOff() {
        is_key_on = false;
        if (sustain_point != 0xFF && current_stage <= sustain_point) {
            current_stage = sustain_point + 1;
        }
    }

    // Called once every 32 samples
    inline uint16_t update() {
        uint8_t release_stage = end_point < 7 ? end_point + 1 : end_point;
        if (current_stage > release_stage) {
            current_level = 0;
            return 0;
        }

        uint32_t target = (current_stage > end_point) ? 0 : (stages[current_stage].level << 16);
        uint32_t step = stages[current_stage].rate;

        // Approach target value with overflow/underflow protection
        if (current_level < target) {
            current_level += step;
            if (current_level > target || current_level < step) current_level = target;
        } else if (current_level > target) {
            current_level = (current_level > step) ? (current_level - step) : 0;
            if (current_level < target) current_level = target;
        }

        // Stage transition check
        if (current_level == target) {
            if (is_key_on && current_stage == sustain_point) {
                return current_level >> 16;
            }
            current_stage++;
        }
        return current_level >> 16;
    }
};

// ==========================================
// Vibrato
// ==========================================
class LFO {
private:
    uint32_t phase = 0;
    uint32_t phase_inc = 0;
    uint16_t delay_counter = 0;
    bool is_active = false;

public:
    LfoWaveform waveform = LfoWaveform::TRIANGLE;
    uint16_t rate = 0;        
    uint16_t depth = 0;       
    uint16_t mod_wheel_depth = 0;
    uint16_t delay = 0;       
    uint16_t delay_samples = 0; 

    void setSampleRate(uint32_t sr) {
        float freq = 0.1f + (static_cast<float>(rate) / 99.0f) * 19.9f;
        phase_inc = static_cast<uint32_t>((freq * 4294967296.0f) / sr);
        delay_samples = (static_cast<uint32_t>(delay) * sr) / 100; 
    }

    void trigger() {
        phase = 0;
        delay_counter = 0;
        is_active = true;
    }
    void release() { is_active = false; }

    inline uint16_t effectiveDepth() const {
        uint16_t d = static_cast<uint16_t>(depth + mod_wheel_depth);
        return d > 99 ? 99 : d;
    }

    inline int32_t nextSample() {
        const uint16_t eff_depth = effectiveDepth();
        if (!is_active || eff_depth == 0) return 0;
        if (delay_counter < delay_samples) {
            delay_counter++;
            return 0;
        }

        phase += phase_inc;
        uint16_t phase_16 = phase >> 16;
        int32_t output = 0;

        switch (waveform) {
            case LfoWaveform::TRIANGLE:
                if (phase_16 < 32768) output = (phase_16 << 1) - 32768; 
                else output = 65535 - (phase_16 << 1); 
                break;
            case LfoWaveform::SQUARE:
                output = (phase_16 < 32768) ? 32767 : -32768;
                break;
            case LfoWaveform::SAW_UP:
                output = static_cast<int32_t>(phase_16) - 32768;
                break;
            case LfoWaveform::SAW_DOWN:
                output = 32767 - static_cast<int32_t>(phase_16);
                break;
        }
        return (output * eff_depth) / 99;
    }
};

// ==========================================
// Waveform tables 
// ==========================================
struct Tables {
    const int16_t* sin_lut;
    uint16_t lut_size;
};

// ==========================================
// CZ-style Oscillator Core with 3 independent EGs
// ==========================================
class Oscillator {
private:
    uint32_t phase = 0;  
    uint8_t eg_divider = 0;  
    uint16_t cached_dco = 0;  
    uint16_t cached_dcw = 0;  
    uint16_t cached_dca = 0;  
    bool wave_cycle_toggle = false;
public:
    uint32_t base_phase_inc = 0;  
    uint32_t phase_inc = 0;  

    Waveform wave1 = Waveform::REZ_SAW;  
    Waveform wave2 = Waveform::REZ_SAW;  
    DcaCurveMode dca_curve_mode = DcaCurveMode::LINEAR;

    Envelope dco_eg;  
    Envelope dcw_eg;  
    Envelope dca_eg;  

    static inline uint16_t scaleDcw(uint16_t value, uint16_t max_value) {
        return static_cast<uint16_t>((static_cast<uint32_t>(value) * max_value) >> 16);
    }

    static inline uint16_t applyDcaCurve(uint16_t value, DcaCurveMode mode) {
        if (mode == DcaCurveMode::PSEUDO_EXP) {
            return static_cast<uint16_t>(((static_cast<uint32_t>(value) * value) + 32768) >> 16);
        }
        return value;
    }

    void resetForNoteOn() {
        phase = 0;
        eg_divider = 31;  
        cached_dco = 0;
        cached_dcw = 0;
        cached_dca = 0;
        wave_cycle_toggle = false;
    }

    inline int32_t nextSample(const Tables& tables) {
        if (++eg_divider >= 32) {
            eg_divider = 0;
            cached_dco = dco_eg.update();
            cached_dcw = dcw_eg.update();
            cached_dca = dca_eg.update();
        }

        uint32_t previous_phase = phase;
        phase += phase_inc;
        if (phase < previous_phase) wave_cycle_toggle = !wave_cycle_toggle;

        uint16_t dcw = scaleDcw(cached_dcw, 56000);
        uint16_t rez_dcw = scaleDcw(dcw, 44000);
        uint16_t dca = applyDcaCurve(cached_dca, dca_curve_mode);

        uint16_t phase_16 = phase >> 16;
        int32_t raw_sample = 0;
        uint8_t shift_bits = 16 - (32 - __builtin_clz(tables.lut_size - 1));

        uint16_t local_phase = phase_16;
        Waveform current_wave = wave1;

        if (wave2 != Waveform(0) && wave2 != wave1) {
            current_wave = wave_cycle_toggle ? wave2 : wave1;
        }

        uint32_t pd_phase = local_phase;

        switch (current_wave) {
            case Waveform::SAW: {
                // dcw=0 gives symmetric shape at 32768, dcw=65535 drives the bend point to maximum asymmetry at 65535
                uint32_t xp_saw = 32768 + ((static_cast<uint32_t>(dcw) * 32767) >> 16);
                if (local_phase < xp_saw) {
                    pd_phase = (static_cast<uint32_t>(local_phase) * 32768) / xp_saw;
                } else {
                    uint32_t denom = 65535 - xp_saw;
                    if (denom == 0) denom = 1;
                    pd_phase = 32768 + ((static_cast<uint32_t>(local_phase) - xp_saw) * 32767) / denom;
                }
                raw_sample = static_cast<int32_t>(tables.sin_lut[pd_phase >> shift_bits]) + 32768;
                break;
            }
            case Waveform::SQUARE: {
                // Trim the first half pulse width with dcw. Scale safely with 31120/65536 to avoid overflow
                uint32_t xp_sq = 32768 - ((static_cast<uint32_t>(dcw) * 31120) >> 16);
                if (local_phase < xp_sq) {
                    pd_phase = (static_cast<uint32_t>(local_phase) * 32768) / xp_sq;
                } else if (local_phase < 32768) {
                    pd_phase = 32768;
                } else if (local_phase < 32768 + xp_sq) {
                    pd_phase = 32768 + ((static_cast<uint32_t>(local_phase) - 32768) * 32767) / xp_sq;
                } else {
                    pd_phase = 65535;
                }
                raw_sample = static_cast<int32_t>(tables.sin_lut[pd_phase >> shift_bits]) + 32768;
                break;
            }
            case Waveform::PULSE: {
                // Use safe fixed-point multiplication scaling to eliminate bit overflow seen in JS
                uint32_t xp_pulse = 65535 - ((static_cast<uint32_t>(dcw) * 63487) >> 16);
                if (local_phase < xp_pulse) {
                    pd_phase = (static_cast<uint32_t>(local_phase) * 65535) / xp_pulse;
                } else {
                    pd_phase = 65535;
                }
                raw_sample = static_cast<int32_t>(tables.sin_lut[pd_phase >> shift_bits]) + 32768;
                break;
            }
            case Waveform::NULL_WAVE: {
                raw_sample = 32768;
                break;
            }
            case Waveform::PULSE2: {
                uint32_t xp_pulse2 = 65535 - ((static_cast<uint32_t>(dcw) * 50000) >> 16);
                if (local_phase < xp_pulse2) {
                    pd_phase = (static_cast<uint32_t>(local_phase) * 65535) / xp_pulse2;
                } else {
                    pd_phase = 65535;
                }
                raw_sample = static_cast<int32_t>(tables.sin_lut[pd_phase >> shift_bits]) + 32768;
                break;
            }
            case Waveform::DOUBLE_SINE: {
                uint32_t xp_dsin = 65535 - ((static_cast<uint32_t>(dcw) * 49151) >> 16);
                if (local_phase < xp_dsin) {
                    pd_phase = (static_cast<uint32_t>(local_phase) * 65535) / xp_dsin;
                } else {
                    uint32_t denom = 65535 - xp_dsin;
                    if (denom == 0) denom = 1;
                    pd_phase = ((static_cast<uint32_t>(local_phase) - xp_dsin) * 65535) / denom;
                }
                raw_sample = static_cast<int32_t>(tables.sin_lut[pd_phase >> shift_bits]) + 32768;
                break;
            }
            case Waveform::SAW_PULSE: {

                uint32_t xp_sp = 65535 - ((static_cast<uint32_t>(dcw) * 32767) >> 16);

                uint32_t t_peak_16 = 32768 + ((static_cast<uint32_t>(dcw) * 29491) >> 16);
                uint32_t peak_pos = (static_cast<uint64_t>(xp_sp) * t_peak_16) >> 16;
                if (peak_pos == 0) peak_pos = 1;

                if (local_phase < xp_sp) {
                    if (local_phase < peak_pos) {

                        pd_phase = (static_cast<uint32_t>(local_phase) * 32768) / peak_pos;
                    } else {
                        // B. Steep descent zone: fall from highest point (32768) to lowest point (65535)
                        uint32_t denom = xp_sp - peak_pos;
                        if (denom == 0) denom = 1;
                        pd_phase = 32768 + ((static_cast<uint32_t>(local_phase - peak_pos) * 32767) / denom);
                    }
                } else {
                    // C. Flat second half: hold the inverse cosine table at its lowest point (65535)
                    pd_phase = 65535;
                }

                raw_sample = static_cast<int32_t>(tables.sin_lut[pd_phase >> shift_bits]) + 32768;
                break;
            }
            case Waveform::REZ_SAW: {
                uint32_t res_phase_32 = static_cast<uint32_t>(local_phase)
                    + ((static_cast<uint64_t>(local_phase) * rez_dcw * 7) >> 16);
                uint16_t res_phase = static_cast<uint16_t>(res_phase_32 & 0xFFFF);
                int32_t core_bipolar = (static_cast<int32_t>(tables.sin_lut[res_phase >> shift_bits]) * 5) >> 3;
                uint32_t window = 65535 - local_phase;
                raw_sample = ((core_bipolar * static_cast<int32_t>(window)) >> 16) + 32768;
                break;
            }
            case Waveform::REZ_TRI: {
                uint32_t res_phase_32 = static_cast<uint32_t>(local_phase)
                    + ((static_cast<uint64_t>(local_phase) * rez_dcw * 7) >> 16);
                uint16_t res_phase = static_cast<uint16_t>(res_phase_32 & 0xFFFF);
                int32_t core_bipolar = (static_cast<int32_t>(tables.sin_lut[res_phase >> shift_bits]) * 5) >> 3;
                uint32_t window = (local_phase < 32768) ? (local_phase << 1) : ((65535 - local_phase) << 1);
                raw_sample = ((core_bipolar * static_cast<int32_t>(window)) >> 16) + 32768;
                break;
            }
            case Waveform::REZ_TRAP: {
                uint32_t res_phase_32 = static_cast<uint32_t>(local_phase)
                    + ((static_cast<uint64_t>(local_phase) * rez_dcw * 7) >> 16);
                uint16_t res_phase = static_cast<uint16_t>(res_phase_32 & 0xFFFF);
                int32_t core_bipolar = (static_cast<int32_t>(tables.sin_lut[res_phase >> shift_bits]) * 5) >> 3;
                uint32_t window;
                if (local_phase < 16384) window = local_phase << 2;
                else if (local_phase > 49152) window = (65535 - local_phase) << 2;
                else window = 65535;
                raw_sample = ((core_bipolar * static_cast<int32_t>(window)) >> 16) + 32768;
                break;
            }
            case Waveform::REZ_PULSE: {
                uint32_t res_phase_32 = static_cast<uint32_t>(local_phase)
                    + ((static_cast<uint64_t>(local_phase) * rez_dcw * 7) >> 16);
                uint16_t res_phase = static_cast<uint16_t>(res_phase_32 & 0xFFFF);
                int32_t core_bipolar = (static_cast<int32_t>(tables.sin_lut[res_phase >> shift_bits]) * 5) >> 3;
                uint32_t window = (local_phase < 32768) ? 65535 : 0;
                raw_sample = ((core_bipolar * static_cast<int32_t>(window)) >> 16) + 32768;
                break;
            }
            case Waveform::REZ_DBL_SAW: {
                uint32_t res_phase_32 = static_cast<uint32_t>(local_phase)
                    + ((static_cast<uint64_t>(local_phase) * rez_dcw * 7) >> 16);
                uint16_t res_phase = static_cast<uint16_t>(res_phase_32 & 0xFFFF);
                int32_t core_bipolar = (static_cast<int32_t>(tables.sin_lut[res_phase >> shift_bits]) * 5) >> 3;
                uint32_t window = (local_phase < 32768)
                    ? 65535 - (static_cast<uint32_t>(local_phase) << 1)
                    : 65535 - (static_cast<uint32_t>(local_phase - 32768) << 1);
                raw_sample = ((core_bipolar * static_cast<int32_t>(window)) >> 16) + 32768;
                break;
            }
        }
        
        int32_t sample = (raw_sample * dca) >> 16;
        return sample - (static_cast<int32_t>(dca) >> 1);
    }
};

// ==========================================
// Voice container with Ring Modulation
// ==========================================
class Voice {
public:
    Oscillator line1;
    Oscillator line2;
    uint32_t sample_rate = DEFAULT_SAMPLE_RATE;
    uint8_t current_note = 0;
    bool is_active = false;
    bool sustained = false;
    uint64_t age = 0;
    uint16_t velocity_gain = 65535;
    uint32_t current_line1_base_phase_inc = 0;
    uint32_t current_line2_base_phase_inc = 0;
    uint32_t target_line1_base_phase_inc = 0;
    uint32_t target_line2_base_phase_inc = 0;

    uint32_t samples_since_note_off = 0;
    uint8_t line1_wave1 = 6;  
    uint8_t line1_wave2 = 0;  
    uint8_t line2_wave1 = 6;  
    uint8_t line2_wave2 = 0;  

    uint8_t detune_octave = 0;     // 0-3
    uint8_t detune_note = 0;       // 0-11
    uint8_t detune_fine = 0;       // 0-60
    int8_t detune_sign = 1;        // -1 or 1

    uint8_t dcw_key_follow = 0;    // 0-99 
    uint8_t dca_key_follow = 0;    // 0-99    

    bool noise_modulation = false;  
    uint32_t noise_phase = 0;       

    void init(uint32_t sr) { sample_rate = sr; }

    float getDetuneFactor() const {
        float oct_val = static_cast<float>(detune_octave);
        float note_val = static_cast<float>(detune_note);
        float fine_val = static_cast<float>(detune_fine);
        float sign_val = static_cast<float>(detune_sign);

        float octave_factor = __builtin_powf(2.0f, sign_val * oct_val);
        float note_factor = __builtin_powf(2.0f, sign_val * note_val / 12.0f);
        float fine_factor = __builtin_powf(2.0f, sign_val * fine_val / (12.0f * 60.0f));
        return octave_factor * note_factor * fine_factor;
    }

    void noteOn(uint8_t note, uint32_t p_inc, bool portamento_enabled = false, uint32_t previous_line1_phase_inc = 0) {
        current_note = note;
        is_active = true;
        samples_since_note_off = 0;
        target_line1_base_phase_inc = p_inc;
        target_line2_base_phase_inc = static_cast<uint32_t>(p_inc * getDetuneFactor());

        if (portamento_enabled && previous_line1_phase_inc > 0) {
            current_line1_base_phase_inc = previous_line1_phase_inc;
            current_line2_base_phase_inc = static_cast<uint32_t>(previous_line1_phase_inc * getDetuneFactor());
        } else {
            current_line1_base_phase_inc = target_line1_base_phase_inc;
            current_line2_base_phase_inc = target_line2_base_phase_inc;
        }

        line1.base_phase_inc = current_line1_base_phase_inc;
        line2.base_phase_inc = current_line2_base_phase_inc;
    }

    void noteOff() {
        sustained = false;
        line1.dco_eg.keyOff(); line1.dcw_eg.keyOff(); line1.dca_eg.keyOff();
        line2.dco_eg.keyOff(); line2.dcw_eg.keyOff(); line2.dca_eg.keyOff();
        samples_since_note_off = 0;  
    }

    bool isKeyHeld() const {
        return line1.dca_eg.is_key_on || line2.dca_eg.is_key_on;
    }

    uint32_t getReleaseLevel() const {
        return (line1.dca_eg.current_level >> 16) + (line2.dca_eg.current_level >> 16);
    }

    inline bool shouldRelease(uint32_t block_samples = 1, LineSelectMode mode = LineSelectMode::LINE1_2, bool ring_mod = false) {
        if (!is_active) return false;
        if (!line1.dca_eg.is_key_on && !line2.dca_eg.is_key_on) {
            samples_since_note_off += block_samples;
            const bool need_l1 = ring_mod || mode != LineSelectMode::LINE2_ONLY;
            const bool need_l2 = ring_mod || mode != LineSelectMode::LINE1_ONLY;
            const bool l1_done = !need_l1 || (line1.dca_eg.current_level < 100);
            const bool l2_done = !need_l2 || (line2.dca_eg.current_level < 100);
            if (l1_done && l2_done) return true;
            if (samples_since_note_off > sample_rate * 4) return true;
        } else {
            samples_since_note_off = 0;
        }
        return false;
    }

    inline int32_t process(const Tables& tables, LineSelectMode mode = LineSelectMode::LINE1_2, bool ring_modulation = false) {
        if (!is_active) return 0;
        const bool need_l1 = ring_modulation || mode != LineSelectMode::LINE2_ONLY;
        const bool need_l2 = ring_modulation || mode != LineSelectMode::LINE1_ONLY;
        int32_t s1 = need_l1 ? line1.nextSample(tables) : 0;
        int32_t s2 = need_l2 ? line2.nextSample(tables) : 0;

        if (noise_modulation) {
            noise_phase = noise_phase * 1103515245u + 12345u;
            int32_t n_bipolar = static_cast<int32_t>((noise_phase >> 16) & 0xFFFF) - 32768;
            if (need_l1) {
                int32_t n1 = (n_bipolar * static_cast<int32_t>(line1.dca_eg.current_level >> 16)) >> 16;
                s1 = (s1 * 7 + n1 * 2) >> 3;
            }
            if (need_l2) {
                int32_t n2 = (n_bipolar * static_cast<int32_t>(line2.dca_eg.current_level >> 16)) >> 16;
                s2 = (s2 * 7 + n2 * 2) >> 3;
            }
        }

        int32_t out = 0;
        if (ring_modulation) {
            out = (s1 * s2) >> 14;
            if (out > 32767) out = 32767;
            else if (out < -32768) out = -32768;
        } else {
            switch (mode) {
                case LineSelectMode::LINE1_ONLY:
                    out = s1;
                    break;
                case LineSelectMode::LINE2_ONLY:
                    out = s2;
                    break;
                case LineSelectMode::LINE1_2_INV:
                    out = (s1 - s2) >> 1;
                    break;
                case LineSelectMode::LINE1_2:
                default:
                    out = (s1 + s2) >> 1;
                    break;
            }
        }
        return (out * static_cast<int32_t>(velocity_gain)) >> 16;
    }
};

// ==========================================
//  Template-driven engine class
//  MaxVoices is an intentional engine capacity, not a CZ line-mode limiter.
//  CrispyZebra keeps Engine<8> available in both one-line and two-line modes.
// ==========================================
template <uint8_t MaxVoices = 8>
class Engine {
private:
    Voice voices[MaxVoices];
    Tables tables;
    uint32_t sample_rate = DEFAULT_SAMPLE_RATE;
    Waveform current_waveform = Waveform::REZ_SAW;  

    LFO vibrato_lfo;
    LineSelectMode line_select = LineSelectMode::LINE1_2;
    bool ring_modulation = false;
    bool noise_modulation = false;
    float dc_x1 = 0.0f;
    float dc_y1 = 0.0f;

    // MASTER section (PARAMETERS.md)
    uint8_t master_octave = 1;         // 0-2 (1=center)
    uint8_t master_note = 0;           // 0-11
    uint8_t master_fine = 30;          // 0-60 (30=center)
    uint8_t master_pan = 50;           // 0-99 (50=center)
    uint8_t master_drive = 0;          // 0-99
    uint8_t master_volume = 80;        // 0-99
    bool portamento_enabled = false;
    uint8_t portamento_time = 0;       // 0-99
    bool mono_mode = false;
    uint8_t max_polyphony = MaxVoices;
    uint8_t velocity_sens = 55;        // 0-99
    bool sustain_pedal = false;
    uint64_t voice_age_counter = 0;
    uint8_t pitch_bend_up_semitones = 2;
    uint8_t pitch_bend_down_semitones = 2;
    int16_t pitch_bend_value = 0;      // -8192 to +8191
    uint32_t last_note_phase_inc = 0;

    // Preset system
    uint8_t current_preset_index = 0; 
    char preset_name[21] = "Default";
    char preset_category[21] = "User";
    bool dark_mode = true;

public:
    void setup(const int16_t* sin_lut, uint16_t lut_size, uint32_t sr) {
        tables.sin_lut = sin_lut;
        tables.lut_size = lut_size;
        sample_rate = sr;
        for (auto& v : voices) {
            v.init(sr);
            v.line1.dco_eg.initCz101Defaults();
            v.line1.dcw_eg.initCz101Defaults();
            v.line1.dca_eg.initCz101Defaults();
            v.line2.dco_eg.initCz101Defaults();
            v.line2.dcw_eg.initCz101Defaults();
            v.line2.dca_eg.initCz101Defaults();
        }
        vibrato_lfo.setSampleRate(sr);
    }

    void setSampleRate(uint32_t sr) {
        sample_rate = sr;
        for (auto& v : voices) v.sample_rate = sr;
        vibrato_lfo.setSampleRate(sr);  
    }

    uint32_t convertCzRateToInternal(uint8_t cz_rate, int eg_type) const {
        static const uint8_t rate_points[5] = {0, 25, 50, 75, 99};
        static const double dco_seconds[5] = {235.0, 14.0, 0.921, 0.054, 0.004};
        static const double dca_dcw_seconds[5] = {104.0, 4.5, 0.22, 0.016, 0.003};

        const double* seconds_points = (eg_type == 0) ? dco_seconds : dca_dcw_seconds;
        int segment = 3;
        for (int i = 0; i < 4; i++) {
            if (cz_rate <= rate_points[i + 1]) {
                segment = i;
                break;
            }
        }

        double r0 = static_cast<double>(rate_points[segment]);
        double r1 = static_cast<double>(rate_points[segment + 1]);
        double t = (static_cast<double>(cz_rate) - r0) / (r1 - r0);
        if (eg_type != 0) t = __builtin_pow(t, 0.62);
        double log_seconds = __builtin_log(seconds_points[segment])
            + t * (__builtin_log(seconds_points[segment + 1]) - __builtin_log(seconds_points[segment]));
        double seconds = __builtin_exp(log_seconds);

        double updates = (static_cast<double>(sample_rate) / 32.0) * seconds;
        if (updates < 1.0) updates = 1.0;

        double step_per_update = 4294901760.0 / updates;
        if (step_per_update < 1.0) return 1;
        if (step_per_update > 4294967295.0) return 0xFFFFFFFF;
        return static_cast<uint32_t>(step_per_update);
    }

void setEgRate(int line, int eg_type, int stage, uint8_t value) {
    if (stage < 0 || stage > 7) return;
    
    // Adjust the name to match the internal definition
    uint32_t internal_rate = convertCzRateToInternal(value, eg_type);
    
    for (auto& v : voices) {
        Oscillator& osc = (line == 1) ? v.line1 : v.line2;
        if (eg_type == 0) osc.dco_eg.stages[stage].rate = internal_rate;
        else if (eg_type == 1) osc.dcw_eg.stages[stage].rate = internal_rate;
        else if (eg_type == 2) osc.dca_eg.stages[stage].rate = internal_rate;
    }
}

    void setEgLevel(int line, int eg_type, int stage, uint8_t value) {
        if (stage < 0 || stage > 7) return;
        uint16_t internal_level = (value * 65535) / 99; // 0-99 to 0-65535
        for (auto& v : voices) {
            Oscillator& osc = (line == 1) ? v.line1 : v.line2;
            Envelope* eg = nullptr;
            if (eg_type == 0) eg = &osc.dco_eg;
            else if (eg_type == 1) eg = &osc.dcw_eg;
            else if (eg_type == 2) eg = &osc.dca_eg;
            if (!eg) continue;

            eg->stages[stage].level = internal_level;
        }
    }

    void setEgSustainPoint(int line, int eg_type, uint8_t point) {
        for (auto& v : voices) {
            Oscillator& osc = (line == 1) ? v.line1 : v.line2;
            if (eg_type == 0) osc.dco_eg.sustain_point = point;
            else if (eg_type == 1) osc.dcw_eg.sustain_point = point;
            else if (eg_type == 2) osc.dca_eg.sustain_point = point;
        }
    }

    void setEgEndPoint(int line, int eg_type, uint8_t point) {
        for (auto& v : voices) {
            Oscillator& osc = (line == 1) ? v.line1 : v.line2;
            Envelope* eg = nullptr;
            if (eg_type == 0) eg = &osc.dco_eg;
            else if (eg_type == 1) eg = &osc.dcw_eg;
            else if (eg_type == 2) eg = &osc.dca_eg;
            if (!eg || point > 7) continue;

            eg->end_point = point;
        }
    }

    // Legacy individual setter (for backward compatibility, typo corrected)
  void setDcaEgDecay2(uint8_t value) {
    for (auto& v : voices) {
        // Call with eg_type = 2 because this is DCA
        v.line1.dca_eg.stages[2].rate = convertCzRateToInternal(value, 2);
        v.line2.dca_eg.stages[2].rate = v.line1.dca_eg.stages[2].rate;
    }
}

    // ==========================================
    // Other setter functions
    // ==========================================
    void setWaveform(Waveform wave) { current_waveform = wave; }
    void setLine1Wave1(uint8_t wave) {
        for (auto& v : voices) {
            v.line1_wave1 = wave;
            v.line1.wave1 = wave != 0 ? static_cast<Waveform>(wave) : current_waveform;
            if (v.line1_wave2 == 0) v.line1.wave2 = v.line1.wave1;
        }
    }
    void setLine1Wave2(uint8_t wave) {
        for (auto& v : voices) {
            v.line1_wave2 = wave;
            v.line1.wave2 = wave != 0 ? static_cast<Waveform>(wave) : v.line1.wave1;
        }
    }
    void setLine2Wave1(uint8_t wave) {
        for (auto& v : voices) {
            v.line2_wave1 = wave;
            v.line2.wave1 = wave != 0 ? static_cast<Waveform>(wave) : current_waveform;
            if (v.line2_wave2 == 0) v.line2.wave2 = v.line2.wave1;
        }
    }
    void setLine2Wave2(uint8_t wave) {
        for (auto& v : voices) {
            v.line2_wave2 = wave;
            v.line2.wave2 = wave != 0 ? static_cast<Waveform>(wave) : v.line2.wave1;
        }
    }
    void setWave1(uint8_t wave) { setLine1Wave1(wave); setLine2Wave1(wave); }
    void setWave2(uint8_t wave) { setLine1Wave2(wave); setLine2Wave2(wave); }

    void setLineSelect(LineSelectMode mode) { line_select = mode; }
    void setRingModulation(bool enable) { ring_modulation = enable; }
    void setNoiseModulation(bool enable) {
        noise_modulation = enable;
        for (auto& v : voices) v.noise_modulation = enable;
    }

    void setVibratoWaveform(LfoWaveform waveform) { vibrato_lfo.waveform = waveform; }
    void setVibratoDelay(uint16_t delay) { vibrato_lfo.delay = delay; vibrato_lfo.setSampleRate(sample_rate); }
    void setVibratoRate(uint16_t rate) { vibrato_lfo.rate = rate; vibrato_lfo.setSampleRate(sample_rate); }
    void setVibratoDepth(uint16_t depth) { vibrato_lfo.depth = depth; }
    void setModWheel(uint8_t cc1) { vibrato_lfo.mod_wheel_depth = static_cast<uint16_t>((static_cast<uint32_t>(cc1) * 99u) / 127u); }

    void updateLiveDetune() {
        for (auto& v : voices) {
            if (v.is_active) {
                v.target_line2_base_phase_inc = static_cast<uint32_t>(v.target_line1_base_phase_inc * v.getDetuneFactor());
                if (!portamento_enabled || portamento_time == 0) {
                    v.current_line2_base_phase_inc = v.target_line2_base_phase_inc;
                }
            }
        }
    }
    void setDetuneOctave(uint8_t octave) { for (auto& v : voices) v.detune_octave = octave > 3 ? 3 : octave; updateLiveDetune(); }
    void setDetuneNote(uint8_t note) { for (auto& v : voices) v.detune_note = note > 11 ? 11 : note; updateLiveDetune(); }
    void setDetuneFine(uint8_t fine) { for (auto& v : voices) v.detune_fine = fine > 60 ? 60 : fine; updateLiveDetune(); }
    void setDetuneSign(int8_t sign) { for (auto& v : voices) v.detune_sign = sign >= 0 ? 1 : -1; updateLiveDetune(); }

    void setDCWKeyFollow(uint8_t amount) { for (auto& v : voices) v.dcw_key_follow = amount; }
    void setDCAKeyFollow(uint8_t amount) { for (auto& v : voices) v.dca_key_follow = amount; }
    void setDcaCurveMode(DcaCurveMode mode) {
        for (auto& v : voices) {
            v.line1.dca_curve_mode = mode;
            v.line2.dca_curve_mode = mode;
        }
    }
    uint8_t getMaxVoiceCount() const { return MaxVoices; }

    // ─── MASTER section ───
    void setMasterOctave(uint8_t oct) { master_octave = oct > 2 ? 1 : oct; }
    void setMasterNote(uint8_t note) { master_note = note > 11 ? 11 : note; }
    void setMasterFine(uint8_t fine) { master_fine = fine > 60 ? 60 : fine; }
    void setMasterPan(uint8_t pan) { master_pan = pan > 99 ? 99 : pan; }
    void setMasterDrive(uint8_t drive) { master_drive = drive > 99 ? 99 : drive; }
    void setMasterVolume(uint8_t vol) { master_volume = vol > 99 ? 99 : vol; }
    void setPortamentoEnabled(bool enabled) { portamento_enabled = enabled; }
    void setPortamentoTime(uint8_t time) { portamento_time = time > 99 ? 99 : time; }
    void setVelocitySensitivity(uint8_t sens) { velocity_sens = sens > 99 ? 99 : sens; }
    void setMaxPolyphony(uint8_t count) {
        if (count < 1) count = 1;
        if (count > MaxVoices) count = MaxVoices;
        max_polyphony = count;
        for (uint8_t i = max_polyphony; i < MaxVoices; i++) {
            voices[i].is_active = false;
            voices[i].sustained = false;
        }
    }
    void setMonoMode(bool enabled) {
        mono_mode = enabled;
        if (mono_mode) {
            for (uint8_t i = 1; i < MaxVoices; i++) {
                voices[i].is_active = false;
                voices[i].sustained = false;
            }
        }
    }
    void setSustainPedal(bool down) {
        sustain_pedal = down;
        if (!sustain_pedal) {
            bool any_held = false;
            for (auto& v : voices) {
                if (v.is_active && v.sustained) {
                    v.noteOff();
                } else if (v.is_active && v.isKeyHeld()) {
                    any_held = true;
                }
            }
            if (!any_held) vibrato_lfo.release();
        }
    }
    void allNotesOff() {
        sustain_pedal = false;
        for (auto& v : voices) {
            if (v.is_active) v.noteOff();
        }
        vibrato_lfo.release();
    }
    void forceSilence() {
        sustain_pedal = false;
        for (auto& v : voices) {
            v.is_active = false;
            v.sustained = false;
            v.line1.resetForNoteOn();
            v.line2.resetForNoteOn();
        }
        dc_x1 = 0.0f;
        dc_y1 = 0.0f;
        vibrato_lfo.release();
    }
    void setPitchBendRangeUp(uint8_t semitones) { pitch_bend_up_semitones = semitones > 24 ? 24 : semitones; }
    void setPitchBendRangeDown(uint8_t semitones) { pitch_bend_down_semitones = semitones > 24 ? 24 : semitones; }
    void setPitchBend(int16_t value) {
        if (value < -8192) value = -8192;
        if (value > 8191) value = 8191;
        pitch_bend_value = value;
    }

    // ─── Preset system ───
    void setPresetIndex(uint8_t idx) { if (idx < 16) current_preset_index = idx; }
    uint8_t getPresetIndex() const { return current_preset_index; }
    const char* getPresetName() const { return preset_name; }
    const char* getPresetCategory() const { return preset_category; }
    void setPresetName(const char* name) {
        for (int i = 0; i < 20 && name[i]; i++) preset_name[i] = name[i];
        preset_name[20] = '\0';
    }
    void setPresetCategory(const char* cat) {
        for (int i = 0; i < 20 && cat[i]; i++) preset_category[i] = cat[i];
        preset_category[20] = '\0';
    }
    void setDarkMode(bool on) { dark_mode = on; }
    bool getDarkMode() const { return dark_mode; }

    // Debug getters
    int getActiveVoiceCount() const {
        int count = 0;
        for (auto& v : voices) if (v.is_active) count++;
        return count;
    }
    uint16_t getDcaLevel(int voiceIdx) const {
        if (voiceIdx < 0 || voiceIdx >= MaxVoices) return 0;
        return voices[voiceIdx].line1.dca_eg.current_level >> 16;
    }
    uint8_t getDcaEndPoint() const {
        return voices[0].line1.dca_eg.end_point;
    }
    uint8_t getDcaSustainPoint() const {
        return voices[0].line1.dca_eg.sustain_point;
    }
    uint32_t getDcaRate(int stage) const {
        return voices[0].line1.dca_eg.stages[stage].rate;
    }
    uint16_t getDcaLevelStage(int stage) const {
        return voices[0].line1.dca_eg.stages[stage].level;
    }

    void midiNoteOn(uint8_t note, uint8_t velocity = 127) {
        const uint8_t limit = mono_mode ? 1 : max_polyphony;
        bool any_held = false;
        for (uint8_t i = 0; i < limit; i++) {
            if (voices[i].is_active && voices[i].isKeyHeld()) {
                any_held = true;
                break;
            }
        }
        if (!any_held) vibrato_lfo.trigger();

        // MASTER transposition
        int master_semitones = (static_cast<int>(master_octave) - 1) * 12 + static_cast<int>(master_note);
        float master_cents = static_cast<float>(static_cast<int>(master_fine) - 30);
        float master_factor = (master_semitones == 0 && master_fine == 30)
            ? 1.0f
            : __builtin_powf(2.0f, (master_semitones + master_cents / 100.0f) / 12.0f);

        Voice* target = nullptr;
        if (mono_mode) {
            target = &voices[0];
            for (uint8_t i = 1; i < MaxVoices; i++) voices[i].is_active = false;
        } else {
            for (uint8_t i = 0; i < limit; i++) {
                if (voices[i].is_active && voices[i].current_note == note) {
                    target = &voices[i];
                    break;
                }
            }
            if (!target) {
                for (uint8_t i = 0; i < limit; i++) {
                    if (!voices[i].is_active) {
                        target = &voices[i];
                        break;
                    }
                }
            }
            if (!target) {
                for (uint8_t i = 0; i < limit; i++) {
                    if (!voices[i].isKeyHeld()) {
                        if (!target || voices[i].getReleaseLevel() < target->getReleaseLevel()) {
                            target = &voices[i];
                        }
                    }
                }
            }
            if (!target) {
                target = &voices[0];
                for (uint8_t i = 1; i < limit; i++) {
                    if (voices[i].age < target->age) target = &voices[i];
                }
            }
        }

        float freq = 440.0f * __builtin_powf(2.0f, (static_cast<int>(note) - 69) / 12.0f) * master_factor;
        uint32_t p_inc = static_cast<uint32_t>((freq * 4294967296.0f) / sample_rate);

        target->line1.wave1 = target->line1_wave1 != 0 ? (CrispyZebra::Waveform)target->line1_wave1 : current_waveform;
        target->line1.wave2 = target->line1_wave2 != 0 ? (CrispyZebra::Waveform)target->line1_wave2 : target->line1.wave1;
        target->line2.wave1 = target->line2_wave1 != 0 ? (CrispyZebra::Waveform)target->line2_wave1 : current_waveform;
        target->line2.wave2 = target->line2_wave2 != 0 ? (CrispyZebra::Waveform)target->line2_wave2 : target->line2.wave1;

        const float vel_norm = static_cast<float>(velocity > 127 ? 127 : (velocity < 1 ? 1 : velocity)) / 127.0f;
        const float sens = static_cast<float>(velocity_sens) / 99.0f;
        const float gain = (1.0f - sens) + sens * (0.18f + 0.82f * vel_norm);
        target->velocity_gain = static_cast<uint16_t>(gain * 65535.0f);
        target->sustained = false;
        target->age = ++voice_age_counter;

        target->noteOn(note, p_inc, portamento_enabled && any_held, last_note_phase_inc);
        last_note_phase_inc = p_inc;

        target->line1.dco_eg.keyOn();
        target->line1.dcw_eg.keyOn();
        target->line1.dca_eg.keyOn();
        target->line2.dco_eg.keyOn();
        target->line2.dcw_eg.keyOn();
        target->line2.dca_eg.keyOn();

        target->line1.resetForNoteOn();
        target->line2.resetForNoteOn();
    }

    void midiNoteOff(uint8_t note) {
        for (auto& v : voices) {
            if (v.is_active && v.current_note == note) {
                if (sustain_pedal) v.sustained = true;
                else v.noteOff();
            }
        }
        bool any_held = false;
        for (auto& v : voices) {
            if (v.is_active && v.isKeyHeld()) {
                any_held = true;
                break;
            }
        }
        if (!any_held) vibrato_lfo.release();
    }

    template <typename SampleType, bool Stereo = true>
    void processBlock(SampleType* outputBuffer, uint32_t numSamples) {
        const uint8_t limit = mono_mode ? 1 : max_polyphony;
        const bool has_bend = (pitch_bend_value != 0);
        float pitch_bend_factor = 1.0f;
        if (has_bend) {
            float bend_norm = pitch_bend_value >= 0
                ? static_cast<float>(pitch_bend_value) / 8191.0f
                : static_cast<float>(pitch_bend_value) / 8192.0f;
            float bend_semitones = bend_norm >= 0
                ? bend_norm * static_cast<float>(pitch_bend_up_semitones)
                : bend_norm * static_cast<float>(pitch_bend_down_semitones);
            pitch_bend_factor = __builtin_powf(2.0f, bend_semitones / 12.0f);
        }

        const bool has_vib = (vibrato_lfo.effectiveDepth() > 0);
        const bool has_drive = (master_drive > 0);
        const float drive_amount = 1.0f + static_cast<float>(master_drive) / 99.0f * 3.5f;
        const float vol = static_cast<float>(master_volume) / 99.0f;
        const float pan = static_cast<float>(master_pan) / 99.0f;
        const float pan_left = (pan <= 0.5f) ? 1.0f : 1.0f - (pan - 0.5f) * 2.0f;
        const float pan_right = (pan >= 0.5f) ? 1.0f : pan * 2.0f;
        const bool do_porta = (portamento_enabled && portamento_time > 0);
        const uint32_t glide = 1 + static_cast<uint32_t>(portamento_time) * 16;

        for (uint32_t i = 0; i < numSamples; i++) {
            int32_t lfo_mod = has_vib ? vibrato_lfo.nextSample() : 0;
            int32_t mix = 0;

            for (uint8_t vi = 0; vi < limit; vi++) {
                Voice& v = voices[vi];
                if (!v.is_active) continue;

                if (do_porta) {
                    int64_t diff1 = static_cast<int64_t>(v.target_line1_base_phase_inc) - v.current_line1_base_phase_inc;
                    int64_t diff2 = static_cast<int64_t>(v.target_line2_base_phase_inc) - v.current_line2_base_phase_inc;
                    v.current_line1_base_phase_inc += static_cast<int32_t>(diff1 / glide);
                    v.current_line2_base_phase_inc += static_cast<int32_t>(diff2 / glide);
                    if (diff1 > -2 && diff1 < 2) v.current_line1_base_phase_inc = v.target_line1_base_phase_inc;
                    if (diff2 > -2 && diff2 < 2) v.current_line2_base_phase_inc = v.target_line2_base_phase_inc;
                } else {
                    v.current_line1_base_phase_inc = v.target_line1_base_phase_inc;
                    v.current_line2_base_phase_inc = v.target_line2_base_phase_inc;
                }

                v.line1.base_phase_inc = v.current_line1_base_phase_inc;
                v.line2.base_phase_inc = v.current_line2_base_phase_inc;

                uint32_t line1_phase_inc = has_bend
                    ? static_cast<uint32_t>(v.line1.base_phase_inc * pitch_bend_factor)
                    : v.line1.base_phase_inc;
                uint32_t line2_phase_inc = has_bend
                    ? static_cast<uint32_t>(v.line2.base_phase_inc * pitch_bend_factor)
                    : v.line2.base_phase_inc;

                const uint32_t dco1_lvl = v.line1.dco_eg.current_level >> 16;
                const uint32_t dco2_lvl = v.line2.dco_eg.current_level >> 16;
                if (dco1_lvl > 0) {
                    line1_phase_inc += static_cast<uint32_t>((static_cast<uint64_t>(line1_phase_inc) * dco1_lvl) >> 16);
                }
                if (dco2_lvl > 0) {
                    line2_phase_inc += static_cast<uint32_t>((static_cast<uint64_t>(line2_phase_inc) * dco2_lvl) >> 16);
                }

                if (has_vib && lfo_mod != 0) {
                    int32_t lfo_range1 = static_cast<int32_t>(v.line1.base_phase_inc >> 4);
                    int32_t lfo_pitch_mod1 = static_cast<int32_t>((static_cast<int64_t>(lfo_mod) * lfo_range1) >> 15);
                    line1_phase_inc += static_cast<uint32_t>(lfo_pitch_mod1);

                    int32_t lfo_range2 = static_cast<int32_t>(v.line2.base_phase_inc >> 4);
                    int32_t lfo_pitch_mod2 = static_cast<int32_t>((static_cast<int64_t>(lfo_mod) * lfo_range2) >> 15);
                    line2_phase_inc += static_cast<uint32_t>(lfo_pitch_mod2);
                }

                v.line1.phase_inc = line1_phase_inc;
                v.line2.phase_inc = line2_phase_inc;

                mix += v.process(tables, line_select, ring_modulation);
            }

            mix = mix >> 1;
            if (mix > 24576) {
                int32_t excess = mix - 24576;
                mix = 24576 + ((excess * 8192) / (8192 + excess));
            } else if (mix < -24576) {
                int32_t excess = -mix - 24576;
                mix = -24576 - ((excess * 8192) / (8192 + excess));
            }
            if (mix > 32767) mix = 32767;
            else if (mix < -32768) mix = -32768;

            // Float one-pole DC blocker (zero residual bias when idle)
            const float raw_f = static_cast<float>(mix) * (1.0f / 32768.0f);
            float f = raw_f - dc_x1 + 0.995f * dc_y1;
            dc_x1 = raw_f;
            if (f > -1e-24f && f < 1e-24f) f = 0.0f;
            dc_y1 = f;

            // MASTER drive (smooth soft saturation)
            if (has_drive) {
                const float x = f * drive_amount;
                const float ax = x < 0.0f ? -x : x;
                f = x / (1.0f + 0.65f * ax);
            }
            f *= vol;

            SampleType out_val;
            if (sizeof(SampleType) == sizeof(float)) {
                out_val = static_cast<SampleType>(f);
            } else {
                float scaled = f * 32767.0f;
                if (scaled > 32767.0f) scaled = 32767.0f;
                else if (scaled < -32768.0f) scaled = -32768.0f;
                out_val = static_cast<SampleType>(scaled);
            }

            if (Stereo) {
                outputBuffer[i * 2] = static_cast<SampleType>(static_cast<float>(out_val) * pan_left);
                outputBuffer[i * 2 + 1] = static_cast<SampleType>(static_cast<float>(out_val) * pan_right);
            } else {
                outputBuffer[i] = out_val;
            }
        }

        for (uint8_t vi = 0; vi < limit; vi++) {
            if (voices[vi].shouldRelease(numSamples, line_select, ring_modulation)) {
                voices[vi].is_active = false;
            }
        }
    }
};

} // namespace CrispyZebra
