#include <string.h>
#include "audio.h" // Contains SDL audio implementation
#include "synth.h"
#include "config.h"
#include "globals.h"

int main(int argc, char *argv[])
{
    // Default to current script if none provided
    _sys.filepath = argc > 1 ? argv[1] : "scripts/empty.lua";
    
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
    
    // Initialize synth and start audio system
    synth_init();
    pa_init(); // This now handles both audio and visualization
    
    printf("exiting...\n");
    return 0;
}
