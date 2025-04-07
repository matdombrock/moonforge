#include <string.h>
#include "pa.h"
#include "synth.h"
#include "config.h"
#include "globals.h"
#include "vis.h"

int main(int argc, char *argv[])
{
    _sys.filepath = argv[1];
    if (argc > 2) {
        if (strcmp(argv[2], "d") == 0) {
            _sys.output_mode = 1;
        }
        else if (strcmp(argv[2], "v") == 0) {
            _sys.output_mode = 2;
        } 
        else if (strcmp(argv[2], "vs") == 0) {
            _sys.output_mode = 2;
            _sys.stop_on_error = 1;
        } 
    }   
    
    // Initialize synth first
    synth_init();
    
    // Start audio processing thread
    if (pa_start_thread() != 0) {
        fprintf(stderr, "Failed to start audio thread\n");
        return 1;
    }
    
    // If visualization mode is enabled, run the visualization on the main thread
    if (_sys.output_mode == 2) {
        // Initialize visualization (now runs in the main thread)
        vis_init();
        vis_loop();
        // When vis_loop exits, clean up visualization
        vis_cleanup();
    } else {
        printf("Press ENTER to stop the audio...\n");
        // Wait for ENTER if not in visualization mode
        getchar();
    }
    
    // Stop the audio thread before exiting
    pa_stop_thread();
    
    printf("exiting...\n");
    return 0;
}
