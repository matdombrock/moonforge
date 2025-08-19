#pragma once
#include <portaudio.h>

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

extern mfState state;

int mf_beat_to_ticks(float bpm, float beat);
int mf_wave_set(int osc_num, enum Wave wave);
int mf_freq_set(int osc_num, float freq);
int mf_freq_change(int osc_num, float freq_mod);
int mf_amp_set(int osc_num, float amp);
int mf_amp_change(int osc_num, float amp_mod);
int mf_mute_all();

