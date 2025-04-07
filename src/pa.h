#pragma once
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "config.h"
#include "util.h"
#include "synth.h"
#include "vis.h"

// Audio callback for SDL
void sdl_audio_callback(void *userdata, Uint8 *stream, int len) {
    Synth_Internal *data = (Synth_Internal *)userdata;
    float *out = (float *)stream;
    
    // Run Lua to process the next audio frame
    synth_lua();
    
    // Generate samples
    synth_get_buffer(data, out);
}

int pa_init() {
    Synth_Internal data;
    SDL_AudioSpec want, have;
    SDL_AudioDeviceID dev;
    
    // Initialize phase values
    for (int i = 0; i < OSC_COUNT; i++) {
        data.phase[i] = 0.0f;
    }
    
    // Initialize Lua state at startup
    luaB_init();
    
    debug("sdl: init\n");
    
    // If in visualization mode, initialize the visualization first
    // which will also initialize SDL audio and video subsystems
    if (_sys.output_mode == 2) {
        if (vis_init() != 0) {
            fprintf(stderr, "Failed to initialize visualization\n");
            return 1;
        }
    } else {
        // Otherwise just initialize SDL audio
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
            return 1;
        }
    }
    
    // Setup the audio specification
    SDL_zero(want);
    want.freq = (int)SAMPLE_RATE / DOWNSAMPLE;
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = BUFFER_SIZE;
    want.callback = sdl_audio_callback;
    want.userdata = &data;
    
    // Open the audio device
    debug("sdl: open audio device\n");
    dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (dev == 0) {
        fprintf(stderr, "SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Check if we got the format we requested
    if (have.format != want.format) {
        fprintf(stderr, "SDL_OpenAudioDevice couldn't get requested format\n");
        SDL_CloseAudioDevice(dev);
        SDL_Quit();
        return 1;
    }
    
    // Start playing audio
    debug("sdl: start audio playback\n");
    SDL_PauseAudioDevice(dev, 0);
    
    printf("------- AUDIO SYSTEM STARTED -------\n");
    char modes[][32] = {"none", "debug", "visualizer"};
    printf("Console output mode: %s\n", modes[_sys.output_mode]);
    
    if (_sys.output_mode == 2) {
        printf("Running visualization on main thread...\n");
        
        // Main loop - handle visualization on the main thread
        while (vis_is_running()) {
            vis_update();
            /*SDL_Delay(16); // ~60fps*/
        }
        
        // Cleanup visualization
        vis_cleanup();
    } else {
        printf("Press ENTER to stop the audio...\n");
        // Wait for ENTER
        getchar();
    }
    
    // Stop audio playback
    SDL_CloseAudioDevice(dev);
    
    // Clean up Lua state
    luaB_cleanup();
    
    SDL_Quit();
    return 0;
}
