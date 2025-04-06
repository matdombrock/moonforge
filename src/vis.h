#include "../SDL2/include/SDL2/SDL.h"
#include "globals.h"
#include <stdio.h>
#include <pthread.h>

SDL_Window *window;
volatile int vis_thread_running = 0;
pthread_mutex_t vis_mutex = PTHREAD_MUTEX_INITIALIZER;

// Color palette (duplicate of luaB_v_colors from luaBinds.h)
int vis_colors[][3] = {
    {0,0,0},
    {255,0,0},
    {0,255,0},
    {0,0,255},
    {255,255,0},
    {255,0,255},
    {0,255,255},
    {255,255,255}
};

// Helper function to set render color
void vis_set_color(SDL_Renderer *renderer, int palette_index) {
    // Original used LUA_INDEX which is defined as 1
    // Make sure we're using 0-based indexing for our array
    int adjusted_index = (palette_index - 1); // Lua indices start at 1 but C indices start at 0
    adjusted_index = adjusted_index < 0 ? 0 : adjusted_index;
    adjusted_index = adjusted_index % 8;
    
    // For debugging
    /*printf("Setting color index %d (adjusted to %d)\n", palette_index, adjusted_index);*/
    
    int r = vis_colors[adjusted_index][0];
    int g = vis_colors[adjusted_index][1];
    int b = vis_colors[adjusted_index][2];
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
}

// Process visualization commands in batches
void vis_process_commands() {
    pthread_mutex_lock(&vis_mutex);
    
    if (_vis.cmd_buffer_ready && _vis.cmd_count > 0) {
        int totalCommands = _vis.cmd_count;
        int processedCommands = 0;
        int batchSize = 5000; // Process 5000 commands at a time
        
        /*printf("Starting to process %d visualization commands\n", totalCommands);*/
        
        // Convert consecutive pixel operations to more efficient batch operations
        // Create arrays for batch processing of pixels
        SDL_Point points[batchSize];
        int pointCount = 0;
        int currentPointColor = -1;
        
        // Track if we need to process the batch after this function
        int needToProcessBatch = 0;
        
        // Process commands in batches
        while (processedCommands < totalCommands) {
            int endIdx = processedCommands + batchSize;
            if (endIdx > totalCommands) {
                endIdx = totalCommands;
            }
            
            // Process current batch
            for (int i = processedCommands; i < endIdx; i++) {
                VisCmd cmd = _vis.cmd_buffer[i];
                
                switch (cmd.type) {
                    case VIS_CMD_CLEAR:
                        // If we have points in the buffer, draw them first
                        if (pointCount > 0) {
                            vis_set_color(_vis.renderer, currentPointColor);
                            SDL_RenderDrawPoints(_vis.renderer, points, pointCount);
                            pointCount = 0;
                        }
                        
                        // Now process the clear command
                        vis_set_color(_vis.renderer, cmd.params[0]);
                        SDL_RenderClear(_vis.renderer);
                        break;
                        
                    case VIS_CMD_RECT:
                        // If we have points in the buffer, draw them first
                        if (pointCount > 0) {
                            vis_set_color(_vis.renderer, currentPointColor);
                            SDL_RenderDrawPoints(_vis.renderer, points, pointCount);
                            pointCount = 0;
                        }
                        
                        // Now process the rect command
                        {
                        SDL_Rect rect = {cmd.params[0], cmd.params[1], cmd.params[2], cmd.params[3]};
                        vis_set_color(_vis.renderer, cmd.params[4]);
                        SDL_RenderFillRect(_vis.renderer, &rect);
                        }
                        break;
                        
                    case VIS_CMD_PIXEL:
                        // If this pixel has a different color or our point buffer is full,
                        // flush the current batch
                        if (currentPointColor != cmd.params[2] || pointCount >= batchSize) {
                            if (pointCount > 0) {
                                vis_set_color(_vis.renderer, currentPointColor);
                                SDL_RenderDrawPoints(_vis.renderer, points, pointCount);
                                pointCount = 0;
                            }
                            currentPointColor = cmd.params[2];
                        }
                        
                        // Add this pixel to our batch
                        points[pointCount].x = cmd.params[0];
                        points[pointCount].y = cmd.params[1];
                        pointCount++;
                        break;
                        
                    case VIS_CMD_LINE:
                        // If we have points in the buffer, draw them first
                        if (pointCount > 0) {
                            vis_set_color(_vis.renderer, currentPointColor);
                            SDL_RenderDrawPoints(_vis.renderer, points, pointCount);
                            pointCount = 0;
                        }
                        
                        // Now process the line command
                        vis_set_color(_vis.renderer, cmd.params[4]);
                        SDL_RenderDrawLine(_vis.renderer, cmd.params[0], cmd.params[1], 
                                          cmd.params[2], cmd.params[3]);
                        break;
                        
                    default:
                        // Unknown command type - skip it
                        break;
                }
            }
            
            processedCommands = endIdx;
            needToProcessBatch = 1;
            
            // If we're processing in smaller batches, we can present intermediate results
            if (processedCommands < totalCommands && processedCommands % (batchSize * 2) == 0) {
                // Flush any remaining points
                if (pointCount > 0) {
                    vis_set_color(_vis.renderer, currentPointColor);
                    SDL_RenderDrawPoints(_vis.renderer, points, pointCount);
                    pointCount = 0;
                    needToProcessBatch = 0;
                }
                
                // Present the rendered frame for intermediate feedback
                SDL_RenderPresent(_vis.renderer);
                /*printf("Processed %d/%d commands, presenting intermediate result\n", */
                       /*processedCommands, totalCommands);*/
            }
        }
        
        // Draw any remaining points in the buffer
        if (pointCount > 0 && needToProcessBatch) {
            vis_set_color(_vis.renderer, currentPointColor);
            SDL_RenderDrawPoints(_vis.renderer, points, pointCount);
        }
        
        // Partial reset - only reset commands we've processed
        _vis.cmd_count = 0;
        _vis.cmd_buffer_ready = 0;
        
        /*printf("Command buffer processed and reset (processed %d commands)\n", totalCommands);*/
    }
    
    pthread_mutex_unlock(&vis_mutex);
}

int vis_init() {
    printf("Initializing SDL...\n");
    
    // Initialize command buffer
    pthread_mutex_lock(&vis_mutex);
    _vis.cmd_count = 0;
    _vis.cmd_processed = 0;
    _vis.cmd_buffer_ready = 0;
    
    // Clear out the command buffer completely
    for (int i = 0; i < VIS_CMD_BUFFER_SIZE; i++) {
        _vis.cmd_buffer[i].type = VIS_CMD_NONE;
        for (int j = 0; j < 6; j++) {
            _vis.cmd_buffer[i].params[j] = 0;
        }
    }
    pthread_mutex_unlock(&vis_mutex);
    
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
    while (!quit && vis_thread_running) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = 1;
                vis_thread_running = 0;
            }
        }
        
        // Process visualization commands if available
        vis_process_commands();
        
        // Lock mutex before accessing shared resources
        pthread_mutex_lock(&vis_mutex);
        
        // Check if we need to render the frame
        int shouldRender = _vis.render_ready;
        
        // Always check for commands even if not time to render
        // This allows us to process commands as they come in
        if (_vis.cmd_buffer_ready && _vis.cmd_count > 0) {
            // Mark that we've checked for commands, so we don't
            // need to check again until the mutex is released
            _vis.cmd_buffer_ready = 0;
            shouldRender = 1; // Force a render if we have commands to process
            /*printf("Commands detected in vis loop, will process immediately\n");*/
        }
        
        pthread_mutex_unlock(&vis_mutex);
        
        // Process commands again if needed
        if (shouldRender) {
            vis_process_commands();
            
            // Present the frame
            SDL_RenderPresent(_vis.renderer);
            
            // Reset the render ready flag
            pthread_mutex_lock(&vis_mutex);
            _vis.render_ready = 0;
            pthread_mutex_unlock(&vis_mutex);
        }
        
        // Add a small sleep to prevent the thread from consuming too much CPU
        SDL_Delay(1); // Extremely short sleep for better responsiveness
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
