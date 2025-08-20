#include <stdio.h>
#include "const.h"
#include "mf.h"
#include "as.h"
#include "util.h"

int main(int argc, char *argv[]) {
  // Handle arguments
  char *script_path = util_get_args(argc, argv);

  srand(99); // Seed the random number generator

  // Init the global state
  mf_init();

  // Initalize audio system
  as_init();

  // Clear the console
  printf("\033[H\033[J");

  // Print startup info
  util_print_startup_info(script_path);

  // Initialize Lua and run the script on a loop
  while (state.flags.exit == 0) {
    mf_loop(script_path); // Call the Lua loop function
    as_sleep(TICK_WAIT);
  }

  // Exit gracefully
  printf(COL_GREEN);
  printf("Clean exit...\n");
  printf(COL_RESET);

  // Close audio system
  as_close();
  return 0;
}
