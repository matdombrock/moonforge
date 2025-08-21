#include "const.h"
#include <stdio.h>
#include <stdlib.h>

char *util_get_args(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "%sUsage: %s <lua_script>%s\n", COL_RED, argv[0], COL_RESET);
    exit(1);
  }
  char *script_path = argv[1];

  // Check if Lua script exists
  FILE *file = fopen(script_path, "r");
  if (!file) {
    fprintf(stderr, "%sError: Lua script '%s' not found.%s\n", COL_RED, script_path, COL_RESET);
    exit(1);
  }
  fclose(file);
  return script_path;
}
void util_print_startup_info(char *script_path) {
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
    printf("Frames per Buffer: %d\n", BLOCK_SIZE);
    printf("Oscillators: %d\n", OSC_COUNT);
    printf("Wave Table Size: %d\n", TABLE_SIZE);
    printf("Tuning: %f (Hz)\n", TUNING);
    printf("Tick Wait: %d ms\n", TICK_WAIT);
  }
  printf(COL_RED);
  printf("Press Ctrl+C to exit.\n");
  printf(COL_GREEN);
  printf("\n");
  printf("Starting audio...\n");
  printf(COL_RESET);
}

#include <stdint.h>
void util_write_wav(const char *filename, float *data, int num_frames, int sample_rate) {
  FILE *f = fopen(filename, "wb");
  int num_channels = 2;
  int bits_per_sample = 32;
  int byte_rate = sample_rate * num_channels * bits_per_sample / 8;
  int block_align = num_channels * bits_per_sample / 8;
  int data_size = num_frames * num_channels * sizeof(float);

  // Write RIFF header
  fwrite("RIFF", 1, 4, f);
  uint32_t chunk_size = 36 + data_size;
  fwrite(&chunk_size, 4, 1, f);
  fwrite("WAVE", 1, 4, f);

  // Write fmt chunk
  fwrite("fmt ", 1, 4, f);
  uint32_t fmt_size = 16;
  fwrite(&fmt_size, 4, 1, f);
  uint16_t audio_format = 3; // IEEE float
  fwrite(&audio_format, 2, 1, f);
  fwrite(&num_channels, 2, 1, f);
  fwrite(&sample_rate, 4, 1, f);
  fwrite(&byte_rate, 4, 1, f);
  fwrite(&block_align, 2, 1, f);
  fwrite(&bits_per_sample, 2, 1, f);

  // Write data chunk
  fwrite("data", 1, 4, f);
  fwrite(&data_size, 4, 1, f);
  fwrite(data, sizeof(float), num_frames * num_channels, f);

  fclose(f);
  printf("WAV file '%s' written successfully with %d frames at %d Hz.\n", filename, num_frames, sample_rate);
}
