#include "vis.h"

// Global variables for visualization
SDL_Window *window = NULL;
SDL_Event e;
volatile int vis_running = 0;
pthread_mutex_t vis_mutex = PTHREAD_MUTEX_INITIALIZER;

static int vis_init() {
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

    // Create a renderer - no need for mutex lock as this is now on main thread
    _vis.renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
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

static int vis_process_events() {
    // Handle events on queue
    while (SDL_PollEvent(&e) != 0) {
        // User requests quit
        if (e.type == SDL_QUIT) {
            vis_running = 0;
            return 1; // Signal to quit
        }
        
        // Handle keyboard events
        if (e.type == SDL_KEYDOWN) {
            // If ESC key is pressed, quit
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                vis_running = 0;
                return 1; // Signal to quit
            }
            
            // Store the key press in the system state for Lua scripts to access
            pthread_mutex_lock(&vis_mutex);
            _sys.keypress = e.key.keysym.sym;
            pthread_mutex_unlock(&vis_mutex);
        }
        
        // Handle key release
        if (e.type == SDL_KEYUP) {
            // Reset the keypress value when key is released
            pthread_mutex_lock(&vis_mutex);
            if (_sys.keypress == e.key.keysym.sym) {
                _sys.keypress = -1;
            }
            pthread_mutex_unlock(&vis_mutex);
        }
    }
    return 0;
}

static int vis_render_frame() {
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
        
        // Signal that visualization is complete, which allows the audio thread
        // to update the visualization data again for the next frame
        pthread_mutex_unlock(&vis_mutex);
        return 1; // Frame rendered
    }
    return 0; // No frame to render
}

static void vis_cleanup() {
    // Cleanup - no mutex needed as this is on main thread
    if (_vis.renderer) {
        SDL_DestroyRenderer(_vis.renderer);
        _vis.renderer = NULL;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    SDL_Quit();
}

// Main visualization loop - to be called from main thread
int vis_loop() {
    // First initialize SDL and create window/renderer
    if (vis_init() != 0) {
        return 1; // Initialization failed
    }
    
    vis_running = 1;
    
    while (vis_running) {
        if (vis_process_events()) {
            break; // Quit requested
        }
        
        vis_render_frame();
        
        // Small delay to prevent 100% CPU usage
        SDL_Delay(10);
    }
    
    vis_cleanup();
    return 0;
}

// Signal to stop visualization
void vis_stop() {
    vis_running = 0;
}