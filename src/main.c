#include "as.h"
#include "const.h"
#include "mf.h"
#include "recording.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
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

    // Calculate elapsed time in nanoseconds
    long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_usec - start.tv_usec) * 1000L;

    // TICK_WAIT is assumed to be in milliseconds; convert to nanoseconds
    long wait_ns = (long)(TICK_WAIT * 1000000L) - elapsed_ns;

    if (wait_ns > 0) {
      struct timespec ts = {.tv_sec = wait_ns / 1000000000L, .tv_nsec = wait_ns % 1000000000L};
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
