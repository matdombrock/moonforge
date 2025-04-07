#include "../SDL2/include/SDL2/SDL.h"
#include "globals.h"
#include <stdio.h>
#include <pthread.h>

SDL_Window *window = NULL;
pthread_mutex_t vis_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile int vis_running = 0;

// SDL must create windows on the main thread on macOS
int vis_init() {
    printf("Initializing SDL for visualization...\n");
    
    // If SDL hasn't been initialized yet, initialize it with both video and audio
    if (!SDL_WasInit(0)) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            printf("SDL initialization failed! SDL_Error: %s\n", SDL_GetError());
            return 1;
        }
    } else if (!(SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO)) {
        // Otherwise just initialize video if it's not already initialized
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
            printf("SDL video initialization failed! SDL_Error: %s\n", SDL_GetError());
            return 1;
        }
    }

    // Create a window (must be on main thread for macOS)
    window = SDL_CreateWindow("MOONFORGE", 
            SDL_WINDOWPOS_UNDEFINED, 
            SDL_WINDOWPOS_UNDEFINED, 
            VIS_WIDTH * 3, VIS_HEIGHT * 3, 
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a renderer
    pthread_mutex_lock(&vis_mutex);
    _vis.renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    pthread_mutex_unlock(&vis_mutex);
    
    if (_vis.renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return 1;
    }
    
    // Set logical size for scaling
    SDL_RenderSetLogicalSize(_vis.renderer, VIS_WIDTH, VIS_HEIGHT);
    printf("SDL visualization initialized successfully!\n");
    vis_running = 1;
    return 0;
}

// Handle one frame of visualization
void vis_update() {
    // Handle SDL events
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        // User requests quit
        if (e.type == SDL_QUIT) {
            vis_running = 0;
        }
    }
    
    // Lock mutex before accessing shared resources
    if (_vis.render_ready) {
        pthread_mutex_lock(&vis_mutex);
        /*for (int y = 0; y < 240; y++) {*/
        /*    for (int x = 0; x < 360; x++) {*/
        /*        // Set pixel color*/
        /*        if (x % 16 == 0 && y % 16 == 0) {*/
        /*            SDL_SetRenderDrawColor(_vis.renderer, 55, 55, 55, 55);*/
        /*            SDL_RenderDrawPoint(_vis.renderer, x, y);*/
        /*        }*/
        /*    }*/
        /*}*/
        SDL_RenderPresent(_vis.renderer);
        pthread_mutex_unlock(&vis_mutex);
        _vis.render_ready = 0;
    }
}

// Cleanup visualization resources
void vis_cleanup() {
    pthread_mutex_lock(&vis_mutex);
    if (_vis.renderer) {
        SDL_DestroyRenderer(_vis.renderer);
        _vis.renderer = NULL;
    }
    pthread_mutex_unlock(&vis_mutex);
    
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
}

// Check if visualization is still running
int vis_is_running() {
    return vis_running;
}

// Function to stop visualization
void vis_stop() {
    vis_running = 0;
}
