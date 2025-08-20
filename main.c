#include "const.h"
#include "mf.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static int paCallback(const void *inputBuffer, void *outputBuffer,
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

  // mf_wave_data data = mf_init();
  mf_init();
  Pa_Initialize();
  PaStream *stream;
  Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER,
                       paCallback, &wave_data);
  Pa_StartStream(stream);

  // Clear the console
  printf("\033[H\033[J");
  printf("MOONFORGE\n");

  while (state.flags.exit == 0) {
    Pa_Sleep(TICK_WAIT);
    mf_loop(script_path); // Call the Lua loop function
  }

  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();
  return 0;
}
