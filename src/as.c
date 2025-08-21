// This file contains the PortAudio wrapper functions for audio playback.
// In thoery this is all that would need to be replaced to use a different
// audio library, such as SDL, OpenAL, JUCE or a custom audio system.

#pragma once
#include "as.h"
#include "const.h"
#include "mf.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Main function handling the audio synthesis callback.
int as_synthesis_callback(const void *input_buffer, void *output_buffer,
                          unsigned long frames_per_buffer,
                          const PaStreamCallbackTimeInfo *time_info,
                          PaStreamCallbackFlags status_flags, void *user_data) {

  mf_wave_data *data = (mf_wave_data *)user_data;
  float *out = (float *)output_buffer;
  unsigned int i;
  for (i = 0; i < frames_per_buffer; i++) {
    float sample_mix_l = 0.0f;
    float sample_mix_r = 0.0f;
    for (int osc = 0; osc < OSC_COUNT; osc++) {
      float sample = 0.0f;
      float freq =
          state.osc[osc].freq / TUNING; // Adjust rate for sine wave frequency
      switch (state.osc[osc].wave) {
      case SINE:
        sample += data->sine[(int)state.osc[osc].phase];
        break;
      case SQUARE:
        sample += data->square[(int)state.osc[osc].phase];
        break;
      case TRIANGLE:
        sample += data->triangle[(int)state.osc[osc].phase];
        break;
      case SAW:
        sample += data->saw[(int)state.osc[osc].phase];
        break;
      case NOISE:
        sample += ((float)random() / (float)RAND_MAX) * 2.0f - 1.0f;
        break;
      case CA:
        sample += data->ca[(int)state.osc[osc].phase];
        break;
      case CB:
        sample += data->cb[(int)state.osc[osc].phase];
        break;
      case CC:
        sample += data->cc[(int)state.osc[osc].phase];
        break;
      case CD:
        sample += data->cd[(int)state.osc[osc].phase];
        break;
      default:
        sample += 0.0f;
        break;
      }
      // Apply lowpass filter
      sample = lowpass_process(&state.osc[osc].lp, sample);
      // Apply panning and amplitude
      float amp = state.osc[osc].amp * state.bus_amp;
      sample_mix_l += sample * amp * state.osc[osc].amp_l;
      sample_mix_r += sample * amp * state.osc[osc].amp_r;
      // Apply delay effect
      sample_mix_l = delay_process(&state.osc[osc].delay, sample_mix_l);
      sample_mix_r = delay_process(&state.osc[osc].delay, sample_mix_r);
      state.osc[osc].phase =
          fmod(state.osc[osc].phase + freq * (TUNING * TABLE_SIZE / SAMPLE_RATE),
               TABLE_SIZE);
    }
    *out++ = sample_mix_l; // Left
    *out++ = sample_mix_r; // Right
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
