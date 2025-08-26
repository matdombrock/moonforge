#include <stdio.h>
#include <stdlib.h>
#include "as.h"
#include "const.h"
#include "mf.h"
#include "recording.h"
#include "util.h"
#include <sys/time.h>
#include <time.h>

int main(int argc, char *argv[]) {
  // Handle arguments
  char *script_path = util_get_args(argc, argv);

  // Seed random number generator
  srand((unsigned int)time(NULL));

  // Init the global state
  mf_init();
  // Initialize recording system
  rec_init();
  // Initalize audio system
  as_init();

  // Clear the console
  printf("\033[H\033[J");

  // Print startup info
  util_print_startup_info(script_path);

  while (!state.flags.exit) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    mf_loop(script_path);
    gettimeofday(&end, NULL);

    double elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
    double wait_ms = TICK_WAIT - elapsed_ms;

    if (wait_ms > 0) {
      struct timespec ts = {.tv_sec = (time_t)(wait_ms / 1000),
                            .tv_nsec = (long)((wait_ms - ((long)(wait_ms / 1000) * 1000)) * 1000000)};
      nanosleep(&ts, NULL);
    }
  }

  // Exit gracefully
  printf(COL_GREEN);
  printf("///////\n//Clean exit...\n///////\n");
  printf(COL_RESET);

  // Close audio system
  rec_write_recording(); // Write any remaining recording data to file
  as_close();
  return 0;
}
