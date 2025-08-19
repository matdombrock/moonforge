#include "mf.h"
#include "user.h"
#include <math.h>
static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      PaStreamCallbackFlags statusFlags, void *userData) {
  paWaveData *data = (paWaveData *)userData;
  float *out = (float *)outputBuffer;
  unsigned int i;
  for (i = 0; i < framesPerBuffer; i++) {
    float sample = 0.0f;
    for (int osc = 0; osc < OSC_COUNT; osc++) {
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
      default:
        sample += 0.0f;
        break;
      }
      sample *= state.osc[osc].amp;
      state.osc[osc].phase =
          fmod(state.osc[osc].phase + freq * (A4 * TABLE_SIZE / SAMPLE_RATE),
               TABLE_SIZE);
    }
    *out++ = sample / OSC_COUNT; // Average the samples from all oscillators
  }
  return paContinue;
}

int main(void) {
  // Initialize oscillators
  for (int i = 0; i < OSC_COUNT; i++) {
    state.osc[i].freq = 440.0f;
    state.osc[i].phase = 0.0f;
    state.osc[i].amp = 0.25f; // Set default amplitude
    state.osc[i].wave = SINE; // Set default wave type
  }
  // Set up flags
  state.flags.exit = 0;

  paWaveData data;
  for (int i = 0; i < TABLE_SIZE; i++) {
    data.sine[i] =
        AMPLITUDE * (float)sin((double)i / (double)TABLE_SIZE * M_PI * 2);
    data.square[i] = (i < TABLE_SIZE / 2) ? AMPLITUDE : -AMPLITUDE;
    data.triangle[i] =
        AMPLITUDE * (1.0f - 2.0f * fabsf((float)i / (float)TABLE_SIZE - 0.5f));
    data.saw[i] = AMPLITUDE * (2.0f * (float)i / (float)TABLE_SIZE - 1.0f);
    // data.noise[i] = AMPLITUDE * ((float)rand() / (float)RAND_MAX * 2.0f
    // - 1.0f);
  }
  Pa_Initialize();
  PaStream *stream;
  Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER,
                       paCallback, &data);
  Pa_StartStream(stream);

  mf_setup();
  int tick = 0;
  while (state.flags.exit == 0) {
    Pa_Sleep(1); // Keep playing indefinitely
    mf_loop(tick);
    tick++;
  }

  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();
  return 0;
}
