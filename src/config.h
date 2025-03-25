#define SAMPLE_RATE 44100
#define DOWNSAMPLE 1 // Downsample factor (1 = no downsample)
#define FREQUENCY 440.0 / 2 // Base frequency (A4)
#define AMPLITUDE 1.0 // Amplitude of the wave
#define BUFFER_SIZE 128 // Number of frames (samples) per buffer
#define LUA_INDEX 1 // Use lua style indexing (1 = true, 0 = false)
#define OSC_COUNT 8
#define RMS_WINDOW (128 * 8)
