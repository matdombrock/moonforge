#include "const.h"
#include "mf.h"
#include <math.h>
#include <stdio.h>

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

void print_startup_info(char *script_path) {
  if (PRINT_STARTUP_INFO == 0) {
    return; // Skip printing if flag is set to 0
  }
  printf(COL_YELLOW);
  printf(" ☾ R E P L I C A T S ☽ \n");

  // printf("█▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█\n");
  // printf("██...................██\n");
  // printf("█.......▄▄▄▄▄▄........█\n");
  // printf("█......▀...▀████▄.....█\n");
  // printf("█............████▄....█\n");
  // printf("█............████▀....█\n");
  // printf("█......▄...▄████▀.....█\n");
  // printf("█.......▀▀▀▀▀▀........█\n");
  // printf("██...................██\n");
  // printf("█▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀█\n");

  char *moon_color = COL_WHITE;
  printf("█▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█\n");
  printf("██...................██\n");
  printf("█.......%s▄▄▄▄▄▄%s........█\n", moon_color, COL_YELLOW);
  printf("█......%s▀%s...%s▀████▄%s.....█\n", moon_color, COL_YELLOW, moon_color, COL_YELLOW);
  printf("█............%s████▄%s....█\n", moon_color, COL_YELLOW);
  printf("█............%s████▀%s....█\n", moon_color, COL_YELLOW);
  printf("█......%s▄%s...%s▄████▀%s.....█\n", moon_color, COL_YELLOW, moon_color, COL_YELLOW);
  printf("█.......%s▀▀▀▀▀▀%s........█\n", moon_color, COL_YELLOW);
  printf("██...................██\n");
  printf("█▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀█\n");
  printf(" ☾ M O O N F O R G E ☽ \n");
  printf(COL_GREEN);
  printf("Version %s\n", VERSION);
  printf(COL_CYAN);
  printf("File: %s\n", script_path);
  if (PRINT_STARTUP_CONSTS) {
    printf(COL_MAGENTA);
    printf("Sample Rate: %d\n", SAMPLE_RATE);
    printf("Frames per Buffer: %d\n", FRAMES_PER_BUFFER);
    printf("Oscillators: %d\n", OSC_COUNT);
    printf("Wave Table Size: %d\n", TABLE_SIZE);
    printf("Tuning: %f (Hz)\n", A4);
    printf("Tick Wait: %d ms\n", TICK_WAIT);
  }
  printf(COL_RED);
  printf("Press Ctrl+C to exit.\n");
  printf(COL_GREEN);
  printf("\n");
  printf("Starting audio...\n");
  printf(COL_RESET);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "%sUsage: %s <lua_script>%s\n", COL_RED, argv[0], COL_RESET);
    return 1;
  }
  char *script_path = argv[1];
  // Check if Lua script exists
  FILE *file = fopen(script_path, "r");
  if (!file) {
    fprintf(stderr, "%sError: Lua script '%s' not found.%s\n", COL_RED, script_path, COL_RESET);
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

  print_startup_info(script_path);

  while (state.flags.exit == 0) {
    mf_loop(script_path); // Call the Lua loop function
    Pa_Sleep(TICK_WAIT);
  }
  printf(COL_GREEN);
  printf("Clean exit...\n");
  printf(COL_RESET);

  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();
  return 0;
}
