// This file contains the PortAudio wrapper functions for audio playback.
// In thoery this is all that would need to be replaced to use a different
// audio library, such as SDL, OpenAL, JUCE or a custom audio system.

#pragma once
#include <math.h>
#include "const.h"
#include "as.h"
#include "mf.h"

// Main function handling the audio synthesis callback.
int as_synthesis_callback(const void *inputBuffer, void *outputBuffer,
                 unsigned long framesPerBuffer,
                 const PaStreamCallbackTimeInfo *timeInfo,
                 PaStreamCallbackFlags statusFlags, void *userData) {

  mf_wave_data *data = (mf_wave_data *)userData;
  float *out = (float *)outputBuffer;
  unsigned int i;
  for (i = 0; i < framesPerBuffer; i++) {
    float sample_mix_l = 0.0f;
    float sample_mix_r = 0.0f;
    for (int osc = 0; osc < OSC_COUNT; osc++) {
      float sample = 0.0f;
      float freq =
          state.osc[osc].freq / A4; // Adjust rate for sine wave frequency
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
        sample += data->noise[(int)state.osc[osc].phase];
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
      float amp = state.osc[osc].amp;
      sample_mix_l += sample * amp * state.osc[osc].amp_l;
      sample_mix_r += sample * amp * state.osc[osc].amp_r;
      state.osc[osc].phase =
          fmod(state.osc[osc].phase + freq * (A4 * TABLE_SIZE / SAMPLE_RATE),
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
  Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER,
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
