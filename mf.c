#include <math.h>
#include <portaudio.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 256
#define TABLE_SIZE 200
#define AMPLITUDE 0.25
#define A4 440.0f
#define OSC_COUNT 2

typedef struct {
  float sine[TABLE_SIZE];
  float square[TABLE_SIZE];
  float triangle[TABLE_SIZE];
  float saw[TABLE_SIZE];
  // float noise[TABLE_SIZE];
} paWaveData;
enum Wave { SINE, SQUARE, TRIANGLE, SAW, NOISE };

typedef struct {
  float freq;
  float phase;
  float amp;
  enum Wave wave;
} mfOsc;

typedef struct {
  int exit;
} mfFlags;

typedef struct {
  mfOsc osc[OSC_COUNT];
  mfFlags flags; 
} mfState;
mfState state = {};

int mf_beat_to_ticks(float bpm, float beat) {
  float ticks_per_second = 1000.0f; // 1 tick = 1 ms
  float seconds_per_beat = 60.0f / bpm;
  return round(beat * seconds_per_beat * ticks_per_second);
}

int mf_wave_set(int osc_num, enum Wave wave) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  state.osc[osc_num].wave = wave;
  return 0; // Success
}

int mf_freq_set(int osc_num, float freq) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  if (freq < 20.0f || freq > 20000.0f) {
    return -2; // Invalid frequency value
  }
  state.osc[osc_num].freq = freq; // Set the frequency for the specified oscillator
  return 0; // Success
}

int mf_freq_change(int osc_num, float freq_mod) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  state.osc[osc_num].freq += freq_mod;
  return 0; // Success
}

int mf_amp_set(int osc_num, float amp) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  if (amp < 0.0f || amp > 1.0f) {
    return -2; // Invalid amplitude value
  }
  state.osc[osc_num].amp = amp; // Set the amplitude for the specified oscillator
  return 0; // Success
}

int mf_amp_change(int osc_num, float amp_mod) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  state.osc[osc_num].amp += amp_mod;
  if (state.osc[osc_num].amp < 0.0f) {
    state.osc[osc_num].amp = 0.0f; // Ensure amplitude does not go below 0
  } else if (state.osc[osc_num].amp > 1.0f) {
    state.osc[osc_num].amp = 1.0f; // Ensure amplitude does not exceed 1
  }
  return 0; // Success
}

int mf_mute_all() {
  for (int i = 0; i < OSC_COUNT; i++) {
    state.osc[i].amp = 0.0f; // Mute all oscillators
  }
  return 0; // Success
}
