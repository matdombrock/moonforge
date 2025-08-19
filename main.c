#include "mf.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "const.h"

static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      PaStreamCallbackFlags statusFlags, void *userData) {
  mf_wave_data *data = (mf_wave_data *)userData;
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

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <lua_script>\n", argv[0]);
    return 1;
  }
  char *script_path = argv[1];
  printf("Lua script to run: %s\n", script_path);
  // Check if Lua script exists
  FILE *file = fopen(script_path, "r");
  if (!file) {
    fprintf(stderr, "Error: Lua script '%s' not found.\n", script_path);
    return 1;
  }
  fclose(file);

  mf_wave_data data = mf_init();
  Pa_Initialize();
  PaStream *stream;
  Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER,
                       paCallback, &data);
  Pa_StartStream(stream);

  while (state.flags.exit == 0) {
    Pa_Sleep(1); // Keep playing indefinitely
    mf_loop(script_path); // Call the Lua loop function
  }

  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();
  return 0;
}
