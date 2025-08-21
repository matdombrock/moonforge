// This file contains the PortAudio wrapper functions for audio playback.
// In thoery this is all that would need to be replaced to use a different
// audio library, such as SDL, OpenAL, JUCE or a custom audio system.

#include "as.h"
#include "const.h"
#include "util.h"
#include "mf.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// A buffer to hold the audio data for recording.
float recording_buffer[RECORDING_BUFFER_SIZE];
int recording_index = 0; // Current index in the recording buffer

// Main function handling the audio synthesis callback.
// time_info and user_data are not used here
// no actual PortAudio code is used here
int as_synthesis_callback(const void *input_buffer, void *output_buffer,
                          unsigned long frames_per_buffer,
                          const PaStreamCallbackTimeInfo *time_info,
                          PaStreamCallbackFlags status_flags, void *user_data) {

  // TODO: None of this contains PA logic and it could be split out for clarity
  mf_wave_data *data = (mf_wave_data *)user_data;
  float *out = (float *)output_buffer;
  unsigned int i;
  for (i = 0; i < frames_per_buffer; i++) {
    float sample_mix_l = 0.0f;
    float sample_mix_r = 0.0f;
    for (int osc = 0; osc < OSC_COUNT; osc++) {
      float sample_a = 0.0f;
      float sample_b = 0.0f;
      // Adjust rate for sine wave frequency
      float freq = state.osc[osc].freq / TUNING;
      int phase_index = (int)state.osc[osc].phase;
      int phase_index_n = (phase_index + 1) % TABLE_SIZE;
      switch (state.osc[osc].wave) {
      case SINE:
        sample_a += data->sine[phase_index];
        sample_b += data->sine[phase_index_n];
        break;
      case SQUARE:
        sample_a += data->square[phase_index];
        sample_b += data->square[phase_index_n];
        break;
      case TRIANGLE:
        sample_a += data->triangle[phase_index];
        sample_b += data->triangle[phase_index_n];
        break;
      case SAW:
        sample_a += data->saw[phase_index];
        sample_b += data->saw[phase_index_n];
        break;
      case NOISE:
        sample_a += ((float)random() / (float)RAND_MAX) * 2.0f - 1.0f;
        sample_b += sample_a;
        break;
      case CA:
        sample_a += data->ca[phase_index];
        sample_b += data->ca[phase_index_n];
        break;
      case CB:
        sample_a += data->cb[phase_index];
        sample_b += data->cb[phase_index_n];
        break;
      case CC:
        sample_a += data->cc[phase_index];
        sample_b += data->cc[phase_index_n];
        break;
      case CD:
        sample_a += data->cd[phase_index];
        sample_b += data->cd[phase_index_n];
        break;
      default:
        sample_a += 0.0f;
        sample_b += 0.0f;
        break;
      }
      // Interpolate the samples
      float phase_frac = state.osc[osc].phase - phase_index;
      float sample = (sample_a * (1.0f - phase_frac)) + (sample_b * phase_frac);
      // Apply lowpass filter
      sample = mfx_lowpass_process(&state.osc[osc].lp, sample);
      // Apply panning and amplitude
      float amp = state.osc[osc].amp * state.bus_amp;
      sample_mix_l += sample * amp * state.osc[osc].amp_l;
      sample_mix_r += sample * amp * state.osc[osc].amp_r;
      // Apply delay effect
      sample_mix_l = mfx_delay_process(&state.osc[osc].delay, sample_mix_l);
      sample_mix_r = mfx_delay_process(&state.osc[osc].delay, sample_mix_r);
      // Increment phase
      state.osc[osc].phase = fmod(
          state.osc[osc].phase + freq * (TUNING * TABLE_SIZE / SAMPLE_RATE),
          TABLE_SIZE);
    }
    // Apply bus effects
    sample_mix_l = mfx_lowpass_process(&state.bus_lp_l, sample_mix_l);
    sample_mix_r = mfx_lowpass_process(&state.bus_lp_r, sample_mix_r);
    sample_mix_l = mfx_delay_process(&state.bus_delay_l, sample_mix_l);
    sample_mix_r = mfx_delay_process(&state.bus_delay_r, sample_mix_r);
    // Add samples to the buffer
    // Samples are read in pairs with odd samples being left and even being
    // right
    *out++ = sample_mix_l; // Left
    *out++ = sample_mix_r; // Right
    // Record samples if recording is enabled
    if (recording_index < RECORDING_BUFFER_SIZE - 2) {
      recording_buffer[recording_index++] = sample_mix_l; // Left
      recording_buffer[recording_index++] = sample_mix_r; // Right
    } else {
      // Buffer is full, stop recording
      printf("Recording buffer full, stopping recording.\n");
      recording_index = RECORDING_BUFFER_SIZE; // Prevent further writes
      util_write_wav("recording.wav", recording_buffer, recording_index / 2,
                   SAMPLE_RATE);
    }
  }
  return paContinue;
}

PaStream *stream;
int as_init() {
  Pa_Initialize();
  Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, SAMPLE_RATE, BLOCK_SIZE,
                       as_synthesis_callback, &wave_data);
  Pa_StartStream(stream);
  return 0;
}

int as_sleep(int ms) {
  Pa_Sleep(ms);
  return 0; // Success
}

int as_close() {
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();
  return 0; // Success
}
