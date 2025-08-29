#pragma once
#include <lua.h>
#include <lualib.h>
#include "const.h"
#include "lowpass.h"
#include "delay.h"

typedef struct {
  float sine[TABLE_SIZE];
  float square[TABLE_SIZE];
  float triangle[TABLE_SIZE];
  float saw[TABLE_SIZE];
  float ca[TABLE_SIZE]; // Custom audio data
  float cb[TABLE_SIZE]; // Custom audio data
  float cc[TABLE_SIZE]; // Custom audio data
  float cd[TABLE_SIZE]; // Custom audio data
} mf_wave_data;

enum mf_wave { SINE, SQUARE, TRIANGLE, SAW, NOISE, CA, CB, CC, CD };

typedef enum {
  FX_NONE,
  FX_DELAY,
  FX_LOWPASS,
  FX_HIGHPASS,
  FX_REVERB
} mf_fx_type;

typedef struct {
  mf_fx_type type;
  union {
    mfx_delay_state delay;
    mfx_lowpass_state lowpass;
    // Add other effect states here
  } state;
} mf_fx;

typedef struct {
  float freq;
  float phase;
  float amp; // Main amplitude
  float amp_l; // Left pan
  float amp_r; // Right pan
  mfx_lowpass_state amp_lp; // Control smoothing
  enum mf_wave wave;
  mf_fx fx_chain[MAX_CHAIN];
  int fx_slot_index; // For tracking which effect slot to use next
} mf_osc;

typedef struct {
  int exit;
} mf_flags;

typedef struct {
  mf_osc osc[OSC_COUNT];
  mf_flags flags;
  float mb_amp;
  mfx_lowpass_state mb_amp_lp; // Control smoothing for bus amp
  mf_fx mb_fx_chain[MAX_CHAIN]; // Global bus effects
  int mb_fx_slot_index; // For tracking which effect slot to use next
} mf_state;

extern mf_state state;
extern mf_wave_data wave_data;

// User functions
int mf_wave_set(int osc_num, enum mf_wave wave);
int mf_freq_set(int osc_num, float freq);
int mf_freq_change(int osc_num, float freq_mod);
float mf_freq_get(int osc_num);
int mf_amp_set(int osc_num, float amp);
int mf_amp_change(int osc_num, float amp_mod);
float mf_amp_get(int osc_num);
int mf_pan_set(int osc_num, float pan_l, float pan_r);
int mf_wavetable_write(enum mf_wave wave, float *data);
int mf_exit();

// System functions
void mf_synth_callback(const void *output_buffer, unsigned long frames_per_buffer, void *user_data);
int mf_init();
int mf_init_lua(char *script_path);
int mf_run_lua(lua_State *L);
void mf_loop(char *script_path);
