#pragma once
#include "../SDL2/include/SDL2/SDL.h"
#include "globals.h"
#include <stdio.h>
#include <pthread.h>

// External variable declarations
extern SDL_Window *window;
extern SDL_Event e;
extern volatile int vis_running;
extern pthread_mutex_t vis_mutex;

// Function declarations - only the public interface
int vis_loop();
void vis_stop();
