#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "const.h"

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
