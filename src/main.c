#include <stdio.h>
#include <stdlib.h>
// TODO: Do we need sys/time and time?
#include <sys/time.h>
#include <time.h>
#include "const.h"
#include "mf.h"
#include "as.h"
#include "util.h"
#include "recording.h"

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

  while (state.flags.exit == 0) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    mf_loop(script_path); // Call the Lua loop function
    gettimeofday(&end, NULL);
    long elapsed = (end.tv_sec - start.tv_sec) * 1000L + (end.tv_usec - start.tv_usec) / 1000L;
    long sleep_time = TICK_WAIT - elapsed;
    if (sleep_time > 0) {
      as_sleep(sleep_time);
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
