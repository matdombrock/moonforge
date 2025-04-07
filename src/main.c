#include <string.h>
#include "config.h"
#define GLOBALS_IMPLEMENTATION
#include "globals.h"
#include "synth.h"
#include "vis.h"
#include "pa.h"

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
    
    // Initialize synth 
    synth_init();
    
    // Start audio on secondary thread
    if (pa_start_thread() != 0) {
        fprintf(stderr, "Failed to start audio thread\n");
        return 1;
    }
    
    // In visualization mode, run SDL on main thread
    if (_sys.output_mode == 2) {
        printf("Starting visualizer on main thread...\n");
        
        // Run visualization loop on main thread (vis_loop handles initialization)
        if (vis_loop() != 0) {
            fprintf(stderr, "Visualization failed\n");
            pa_stop_thread();
            return 1;
        }
    } else {
        // In non-visualization mode, just wait for user input
        printf("Press ENTER to stop the audio...\n");
        getchar();
    }
    
    // Stop audio thread when exiting
    pa_stop_thread();
    
    // Make sure visualization is also stopped
    vis_stop();
    
    printf("exiting...\n");
    return 0;
}
