#pragma once
#include <lua.h>
#include <lualib.h>
#include "const.h"

typedef struct {
  float sine[TABLE_SIZE];
  float square[TABLE_SIZE];
  float triangle[TABLE_SIZE];
  float saw[TABLE_SIZE];
  float noise[TABLE_SIZE];
  float ca[TABLE_SIZE]; // Custom audio data
  float cb[TABLE_SIZE]; // Custom audio data
  float cc[TABLE_SIZE]; // Custom audio data
  float cd[TABLE_SIZE]; // Custom audio data
} mf_wave_data;

enum Wave { SINE, SQUARE, TRIANGLE, SAW, NOISE, CA, CB, CC, CD };

typedef struct {
  float freq;
  float phase;
  float amp;
  float amp_l;
  float amp_r;
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
extern mf_wave_data wave_data;

// User functions
int mf_beat_to_ticks(float bpm, float beat);
int mf_wave_set(int osc_num, enum Wave wave);
int mf_freq_set(int osc_num, float freq);
int mf_freq_change(int osc_num, float freq_mod);
float mf_freq_get(int osc_num);
int mf_amp_set(int osc_num, float amp);
int mf_amp_change(int osc_num, float amp_mod);
float mf_amp_get(int osc_num);
int mf_pan_set(int osc_num, float pan_l, float pan_r);
int mf_mute_all();
int mf_exit();

// System functions
int mf_init();
int mf_init_lua(char *script_path);
int mf_run_lua(lua_State *L);
void mf_loop(char *script_path);
