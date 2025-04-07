#pragma once
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include "luaBinds.h"
#include "globals.h"

// Represents the internal state of the oscillator
// Not directly exposed to the user
typedef struct {
  float phase[OSC_COUNT];
} Synth_Internal;

void synth_init() { 
    for (int i = 0; i < 8; i++) {
        Synth def = {
            .freq = FREQUENCY,
            .detune = 0.0f,
            .amp = 1.0f,
            .pan = 0.0f,
            .env = {0.1f, 0.1f, 0.1f},
            .env_pos = 9999.0f,
            .env_enabled = 0,
            .lp_cutoff = 20000.0f,
            .lp_resonance = 1.0f,
            .lp_enabled = 0,
            .hp_cutoff = 0.0f,
            .hp_resonance = 1.0f,
            .hp_enabled = 0,
            .wave = 1,
            .enabled = 0,
        };
        _synth[i] = def; 
    }
    // Activate the first oscillator
    _synth[0].enabled = 1;
}

float synth_get_env(int osc) {
    float env = 0.0f;
    float pos = _synth[osc].env_pos;
    float attack_t = _synth[osc].env[0];
    float sustain_t = _synth[osc].env[1];
    float release_t = _synth[osc].env[2];
    
    if (pos < attack_t) { // Attack
        env = pos / attack_t;
    } 
    else if (pos < (attack_t + sustain_t)) { // Sustain
        env = 1.0f;
    } 
    else { // Release
        env = 1.0f - (pos - attack_t - sustain_t) / release_t;
    }

    _synth[osc].env_pos += (1.0f / SAMPLE_RATE) * _sys.speed;
    if (_synth[osc].env_pos > (attack_t + release_t + sustain_t)) {
        _synth[osc].env_pos = attack_t + release_t + sustain_t;
    }
    return env;
}
float synth_get_sample(float phase, int osc) {
    int mode = _synth[osc].wave % 6; // Wrap wave mode
    float sample = 0.0f;
    switch (mode) {
        case 0: // Off
            sample = 0;
            break;
        case 1: // Sine
            sample = sinf(phase);
            break;
        case 2: // Square
            sample = sinf(phase) > 0 ? 1.0f : -1.0f;
            break;
        case 3: // Saw
            sample = 2.0f * (phase / (2.0f * M_PI)) - 1.0f;
            break;
        case 4: // Triangle
            sample = 2.0f * fabs(2.0f * (phase / (2.0f * M_PI)) - 1.0f) - 1.0f;
            break;
        case 5: // Noise
            sample = 2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f;
            break;
        default: // Off
            sample = 0;
            break;
    }
    if (_synth[osc].env_enabled) sample *= synth_get_env(osc);
    return sample;
}

float synth_phase_increment(float freq, float detune) {
    return 2.0f * M_PI * freq / ((float)SAMPLE_RATE / DOWNSAMPLE) * (1.0f + detune);
}

// Implements an Infinite Impulse Response (IIR) lowpass filter
float _synth_lowpass_buf_in[2 + OSC_COUNT] = {0};
float _synth_lowpass_buf_out[2 + OSC_COUNT] = {0};
float synth_lowpass(int index, float input, float cutoff, float dt) {
    float prev_input = _synth_lowpass_buf_in[index];
    float prev_output = _synth_lowpass_buf_out[index];

    float RC = 1.0f / (2.0f * M_PI * cutoff);
    float alpha = dt / (RC + dt);

    float current_output = alpha * input + (1.0f - alpha) * prev_output;

    _synth_lowpass_buf_in[index] = input;
    _synth_lowpass_buf_out[index] = current_output;

    return current_output;
}

// Implements an Infinite Impulse Response (IIR) highpass filter
// TODO: Can these be static?
float _synth_highpass_buf_in[2 + OSC_COUNT] = {0};
float _synth_highpass_buf_out[2 + OSC_COUNT] = {0};
float synth_highpass(int index, float input, float cutoff, float dt) {
    float prev_input = _synth_highpass_buf_in[index];
    float prev_output = _synth_highpass_buf_out[index];

    float RC = 1.0f / (2.0f * M_PI * cutoff);
    float alpha = RC / (RC + dt);

    float current_output = alpha * prev_output + alpha * (input - prev_input);

    _synth_highpass_buf_in[index] = input;
    _synth_highpass_buf_out[index] = current_output;

    return current_output;
}


// Mutex is now declared in luaBinds.h

void synth_get_buffer(Synth_Internal *data, float *out) {
  // Get local copy of rms_index to reduce lock time
  int local_rms_index;
  
  pthread_mutex_lock(&vis_mutex);
  local_rms_index = _vis.rms_index;
  pthread_mutex_unlock(&vis_mutex);
  
  // Create local buffers to minimize lock time
  float local_rms_buffer[OSC_COUNT][RMS_WINDOW];
  float local_rms_buffer_bus[2][RMS_WINDOW];
  
  // Copy existing buffer data if needed
  if (local_rms_index > 0) {
    pthread_mutex_lock(&vis_mutex);
    for (int ii = 0; ii < OSC_COUNT; ii++) {
      for (int j = 0; j < local_rms_index; j++) {
        local_rms_buffer[ii][j] = _vis.rms_buffer[ii][j];
      }
    }
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < local_rms_index; j++) {
        local_rms_buffer_bus[i][j] = _vis.rms_buffer_bus[i][j];
      }
    }
    pthread_mutex_unlock(&vis_mutex);
  }
  
  for (int i = 0; i < BUFFER_SIZE; i++) {
        float samples[OSC_COUNT];
        int enabled_count = 0;
        for (int ii = 0; ii < OSC_COUNT; ii++) {
            if (!_synth[ii].enabled || _synth[ii].freq == 0 || _synth[ii].wave == 0) {
                samples[ii] = 0;
                local_rms_buffer[ii][local_rms_index] = 0;
                continue;
            }
            enabled_count++;
            float sample = AMPLITUDE * synth_get_sample(data->phase[ii], ii);
            if (_synth[ii].lp_enabled) {
                sample = synth_lowpass(2 + ii, sample, _synth[ii].lp_cutoff, _synth[ii].lp_resonance / SAMPLE_RATE);
            }
            if (_synth[ii].hp_enabled) {
                sample = synth_highpass(2 + ii, sample, _synth[ii].hp_cutoff, _synth[ii].hp_resonance / SAMPLE_RATE);
            }
            sample *= _synth[ii].amp;
            samples[ii] = sample;
            local_rms_buffer[ii][local_rms_index] = sample;
        }
        float mixL = 0;
        float mixR = 0;
        for (int ii = 0; ii < OSC_COUNT; ii++) {
            // Negative values pan left, positive values pan right
            float pan = _synth[ii].pan;
            float panL = 0.5f - 0.5f * pan;
            float panR = 0.5f + 0.5f * pan;
            mixL += panL * samples[ii];
            mixR += panR * samples[ii];
        }
        /*mixL = mixL / (float)enabled_count;*/
        /*mixR = mixR / (float)enabled_count;*/
        // Bus lowpass filter
        mixL = synth_lowpass(0, mixL, _bus.lp_cutoff, _bus.lp_resonance / SAMPLE_RATE);
        mixR = synth_lowpass(1, mixR, _bus.lp_cutoff, _bus.lp_resonance / SAMPLE_RATE);
        // We can get pops on some systems as the audio system inits
        // Fade in over 32 ticks to avoid this
        float fadein = 1.0f;
        if (_sys.tick_num < 32) {
            fadein = (float)_sys.tick_num / 32.0f;
            fadein = fadein < 0.1f ? 0.0f : fadein;
        }
        // Bus AMPLITUDE
        mixL *= _bus.amp * fadein;
        mixR *= _bus.amp * fadein;
        // Add to bus rms_buffer
        local_rms_buffer_bus[0][local_rms_index] = mixL;
        local_rms_buffer_bus[1][local_rms_index] = mixR;
        /**out++ = mix;*/
        out[i * 2] = mixL; // Left
        out[i * 2 + 1] = mixR; // Right

        // Increment phase
        for (int ii = 0; ii < OSC_COUNT; ii++) {
            data->phase[ii] += synth_phase_increment(_synth[ii].freq, _synth[ii].detune);
            if (data->phase[ii] >= 2.0f * M_PI) {
                data->phase[ii] -= 2.0f * M_PI;
            }
        }
        //
        local_rms_index++;
        _sys.sample_num++;
    }
    
    // Calculate RMS if needed
    if (local_rms_index >= RMS_WINDOW) {
        float local_rms[OSC_COUNT];
        float local_rms_bus[2];
        float local_rms_bus_history[2][VIS_RMS_BUS_HIST];
        
        // Calculate oscillator RMS values
        for (int i = 0; i < OSC_COUNT; i++) {
            float sum = 0.0f;
            for (int j = 0; j < RMS_WINDOW; j++) {
                sum += local_rms_buffer[i][j] * local_rms_buffer[i][j];
            }
            local_rms[i] = sqrtf(sum / RMS_WINDOW);
        }
        
        // Calculate bus RMS
        float sumL = 0.0f;
        float sumR = 0.0f;
        for (int i = 0; i < RMS_WINDOW; i++) {
            sumL += local_rms_buffer_bus[0][i] * local_rms_buffer_bus[0][i];
            sumR += local_rms_buffer_bus[1][i] * local_rms_buffer_bus[1][i];
        }
        local_rms_bus[0] = sqrtf(sumL / RMS_WINDOW);
        local_rms_bus[1] = sqrtf(sumR / RMS_WINDOW);
        
        // Copy existing history data for updating
        pthread_mutex_lock(&vis_mutex);
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < VIS_RMS_BUS_HIST; j++) {
                local_rms_bus_history[i][j] = _vis.rms_bus_history[i][j];
            }
        }
        pthread_mutex_unlock(&vis_mutex);
        
        // Update history using FILO pattern
        for (int i = 0; i < (VIS_RMS_BUS_HIST - 1); i++) {
            local_rms_bus_history[0][i] = local_rms_bus_history[0][i + 1];
            local_rms_bus_history[1][i] = local_rms_bus_history[1][i + 1];
        }
        local_rms_bus_history[0][VIS_RMS_BUS_HIST - 1] = local_rms_bus[0];
        local_rms_bus_history[1][VIS_RMS_BUS_HIST - 1] = local_rms_bus[1];
        
        // Now update all the visualization data at once with a lock
        pthread_mutex_lock(&vis_mutex);
        
        // Update RMS values
        for (int i = 0; i < OSC_COUNT; i++) {
            _vis.rms[i] = local_rms[i];
        }
        
        // Update bus RMS values
        _vis.rms_bus[0] = local_rms_bus[0];
        _vis.rms_bus[1] = local_rms_bus[1];
        
        // Update history
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < VIS_RMS_BUS_HIST; j++) {
                _vis.rms_bus_history[i][j] = local_rms_bus_history[i][j];
            }
        }
        
        // Update the buffers
        for (int ii = 0; ii < OSC_COUNT; ii++) {
            for (int j = 0; j < RMS_WINDOW; j++) {
                _vis.rms_buffer[ii][j] = local_rms_buffer[ii][j];
            }
        }
        
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < RMS_WINDOW; j++) {
                _vis.rms_buffer_bus[i][j] = local_rms_buffer_bus[i][j];
            }
        }
        
        // Reset index and set render flag
        _vis.rms_index = 0;
        
        pthread_mutex_unlock(&vis_mutex);
    } else {
        // Update buffers and index
        pthread_mutex_lock(&vis_mutex);
        
        // Update the buffers
        for (int ii = 0; ii < OSC_COUNT; ii++) {
            for (int j = 0; j < local_rms_index; j++) {
                _vis.rms_buffer[ii][j] = local_rms_buffer[ii][j];
            }
        }
        
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < local_rms_index; j++) {
                _vis.rms_buffer_bus[i][j] = local_rms_buffer_bus[i][j];
            }
        }
        
        _vis.rms_index = local_rms_index;
        
        pthread_mutex_unlock(&vis_mutex);
    }
}

void synth_lua() {
    luaB_run();
}
