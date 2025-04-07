#pragma once
#include "../SDL2/include/SDL2/SDL.h"
#include "globals.h"
#include <stdio.h>
#include <pthread.h>

// Shared mutex for synchronizing between audio and visualization
pthread_mutex_t vis_mutex = PTHREAD_MUTEX_INITIALIZER;
static SDL_Window *window = NULL;
static volatile int vis_running = 0;

// Initialize SDL and create a window and renderer (now runs on main thread)
int vis_init() {
    printf("Initializing SDL...\n");
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    window = SDL_CreateWindow("MOONFORGE", 
            SDL_WINDOWPOS_UNDEFINED, 
            SDL_WINDOWPOS_UNDEFINED, 
            VIS_WIDTH * 3, VIS_HEIGHT * 3, 
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    pthread_mutex_lock(&vis_mutex);
    _vis.renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    pthread_mutex_unlock(&vis_mutex);
    
    if (_vis.renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Set logical size for scaling
    SDL_RenderSetLogicalSize(_vis.renderer, VIS_WIDTH, VIS_HEIGHT);
    printf("SDL initialized successfully!\n");
    
    vis_running = 1;
    return 0;
}

// Main visualization loop (runs on main thread)
int vis_loop() { 
    // Main loop flag
    int quit = 0;

    // Event handler
    SDL_Event e;

    // While application is running
    while (!quit && vis_running) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = 1;
                vis_running = 0;
            }
        }
        
        // Lock mutex before accessing shared resources
        if (_vis.render_ready) {
            pthread_mutex_lock(&vis_mutex);
            if (_vis.renderer) {
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
            }
            _vis.render_ready = 0;
            pthread_mutex_unlock(&vis_mutex);
        }
        
        // Add a small sleep to prevent consuming too much CPU
        SDL_Delay(10);
    }

    return 0;
}

// Clean up SDL resources
void vis_cleanup() {
    // Cleanup
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

    SDL_Quit();
}
