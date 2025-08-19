#pragma once
#include <portaudio.h>
#include <lua.h>
#include <lualib.h>

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
  float noise[TABLE_SIZE];
} mf_wave_data;

enum Wave { SINE, SQUARE, TRIANGLE, SAW, NOISE };

typedef struct {
  float freq;
  float phase;
  float amp;
  enum Wave wave;
} mf_osc;

typedef struct {
  int exit;
} mf_flags;

typedef struct {
  mf_osc osc[OSC_COUNT];
  mf_flags flags;
} mf_state;

extern mf_state state;

// User functions
int mf_beat_to_ticks(float bpm, float beat);
int mf_wave_set(int osc_num, enum Wave wave);
int mf_freq_set(int osc_num, float freq);
int mf_freq_change(int osc_num, float freq_mod);
int mf_amp_set(int osc_num, float amp);
int mf_amp_change(int osc_num, float amp_mod);
int mf_mute_all();

// System functions
mf_wave_data mf_init();
int mf_init_lua(char *script_path);
int mf_run_lua(lua_State *L);
void mf_loop(char *script_path);
