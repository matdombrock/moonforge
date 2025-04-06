#include "../SDL2/include/SDL2/SDL.h"
#include "globals.h"
#include <stdio.h>
SDL_Window *window;
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
            360 * 2, 240 * 2, 
            SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    _vis.renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (_vis.renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    // Set logical size for scaling
    SDL_RenderSetLogicalSize(_vis.renderer, 360, 240);
    printf("SDL initialized successfully!\n");
    return 0;
}

int vis_loop() { 

    // Main loop flag
    int quit = 0;

    // Event handler
    SDL_Event e;

    // While application is running
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
        if (_vis.render_ready) {
            // Render the visualizer
            // Update screen
            SDL_RenderPresent(_vis.renderer);
            _vis.render_ready = 0;
        }
    }

    // Destroy renderer and window
    SDL_DestroyRenderer(_vis.renderer);
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    SDL_Quit();

    return 0;
}
