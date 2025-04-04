#define SAMPLE_RATE 44100
#define DOWNSAMPLE 1 // Downsample factor (1 = no downsample)
#define FREQUENCY 440.0 / 2 // Base frequency (A4)
#define AMPLITUDE 1.0 // Amplitude of the wave
#define BUFFER_SIZE 128 // Number of frames (samples) per buffer
#define LUA_INDEX 1 // Use lua style indexing (1 = true, 0 = false)
#define OSC_COUNT 8
#define LUA_TIME_WINDOW 128
#define RMS_WINDOW (128 * 8) // This is vis
#define MEMORY_SIZE 128
#define VIS_WIDTH 24
#define VIS_HEIGHT 16
#define VIS_NULL_CHAR L'/'
#define VIS_ENABLE_INPUT 0
#define VIS_BUS_N 16
#define VIS_RMS_BUS_HIST 32
