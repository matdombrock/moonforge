#include <stdio.h>
#include <stdlib.h>
// TODO: Do we need sys/time and time?
#include <sys/time.h>
#include <time.h>
#include "const.h"
#include "mf.h"
#include "as.h"
#include "util.h"

int main(int argc, char *argv[]) {
  // Handle arguments
  char *script_path = util_get_args(argc, argv);

  // srand(); // Seed the random number generator
  srand((unsigned int)time(NULL));

  // Init the global state
  mf_init();

  // Initalize audio system
  as_init();

  // Clear the console
  printf("\033[H\033[J");

  // Print startup info
  util_print_startup_info(script_path);

  // Initialize Lua and run the script on a loop
  // while (state.flags.exit == 0) {
  //   mf_loop(script_path); // Call the Lua loop function
  //   as_sleep(TICK_WAIT);
  // }
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
  printf("Clean exit...\n");
  printf(COL_RESET);

  // Close audio system
  as_close();
  return 0;
}
