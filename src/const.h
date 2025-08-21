#pragma once

///////
// Exposed to Lua
///////

#define VERSION "0.1.0"
#define SAMPLE_RATE 48000
#define BLOCK_SIZE SAMPLE_RATE / 1000 // 1k ticks per second
#define TABLE_SIZE 256
#define TUNING 440.0f
#define OSC_COUNT 8
#define TICK_WAIT 1 // 1ms per tick, this should likely not be changed

///////
// Not exposed to Lua  
///////

// Recording settings
// RECORDING_BUFFER_SIZE = 48000 * 60 * 2 = 5,760,000 samples per 60 seconds
// Bytes = 5,760,000 * 4 = 23,040,000 bytes (23 MB)
#define RECORDING_ENABLED 1 // Enabling recording greatly increases memory usage
#define MAX_RECORDING_SECONDS 240 // The longer the recording, the more memory is used 
#define RECORDING_BUFFER_SIZE (SAMPLE_RATE * MAX_RECORDING_SECONDS * 2) // Stereo buffer

// Startup settings
#define PRINT_STARTUP_INFO 1
#define PRINT_STARTUP_CONSTS 1

// Console color codes for terminal output
#define COL_RED "\033[31m"
#define COL_GREEN "\033[32m"
#define COL_YELLOW "\033[33m"
#define COL_BLUE "\033[34m"
#define COL_MAGENTA "\033[35m"
#define COL_CYAN "\033[36m"
#define COL_WHITE "\033[37m"
#define COL_RESET "\033[0m"
