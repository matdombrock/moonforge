#include "../SDL2/include/SDL2/SDL.h"
#include "globals.h"
#include <stdio.h>
#include <pthread.h>

SDL_Window *window;
volatile int vis_thread_running = 0;
pthread_mutex_t vis_mutex = PTHREAD_MUTEX_INITIALIZER;

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
    return 0;
}

int vis_loop() { 
    // Main loop flag
    int quit = 0;

    // Event handler
    SDL_Event e;

    // While application is running
    while (!quit && vis_thread_running) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = 1;
                vis_thread_running = 0;
            }
        }
        
        // Lock mutex before accessing shared resources
        if (_vis.render_ready) {
            pthread_mutex_lock(&vis_mutex);
            for (int y = 0; y < 240; y++) {
                for (int x = 0; x < 360; x++) {
                    // Set pixel color
                    if (x % 16 == 0 && y % 16 == 0) {
                        SDL_SetRenderDrawColor(_vis.renderer, 55, 55, 55, 55);
                        SDL_RenderDrawPoint(_vis.renderer, x, y);
                    }
                    
                }
            }
            SDL_RenderPresent(_vis.renderer);
            _vis.render_ready = 0;
            pthread_mutex_unlock(&vis_mutex);
        }
        
        // Add a small sleep to prevent the thread from consuming too much CPU
        /*SDL_Delay(10);*/
    }

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
    return 0;
}

// Thread function for visualization
void* vis_thread_func(void* arg) {
    vis_init();
    vis_loop();
    return NULL;
}

// Function to start visualization in a separate thread
int vis_start_thread() {
    pthread_t thread_id;
    vis_thread_running = 1;
    
    int result = pthread_create(&thread_id, NULL, vis_thread_func, NULL);
    if (result != 0) {
        printf("Error creating visualization thread: %d\n", result);
        return 1;
    }
    
    // Detach the thread so it can clean up its resources when it exits
    pthread_detach(thread_id);
    return 0;
}

// Function to stop visualization thread
void vis_stop_thread() {
    vis_thread_running = 0;
}
