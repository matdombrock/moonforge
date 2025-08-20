#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <math.h>
#include <portaudio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mf.h"
#include "lowpass.h"

mf_state state = {}; // Global state
mf_wave_data wave_data; // Global wave data

///////
/// Core functions
///////

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
  state.osc[osc_num].freq =
      freq; // Set the frequency for the specified oscillator
  return 0; // Success
}

float mf_freq_get(int osc_num) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  return state.osc[osc_num]
      .freq; // Return the frequency of the specified oscillator
}

int mf_amp_set(int osc_num, float amp) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  if (amp < 0.0f || amp > 1.0f) {
    return -2; // Invalid amplitude value
  }
  state.osc[osc_num].amp =
      amp;  // Set the amplitude for the specified oscillator
  return 0; // Success
}

float mf_amp_get(int osc_num) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  return state.osc[osc_num]
      .amp; // Return the amplitude of the specified oscillator
}

int mf_pan_set(int osc_num, float pan_l, float pan_r) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  if (pan_l < 0.0f || pan_l > 1.0f || pan_r < 0.0f || pan_r > 1.0f) {
    return -2; // Invalid panning values
  }
  state.osc[osc_num].amp_l = pan_l; // Set left channel amplitude
  state.osc[osc_num].amp_r = pan_r; // Set right channel amplitude
  return 0;                         // Success
}

float mf_pan_get_l(int osc_num) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  return state.osc[osc_num].amp_l; // Return left channel amplitude
}

float mf_pan_get_r(int osc_num) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  return state.osc[osc_num].amp_r; // Return right channel amplitude
}

int mf_lowpass_set(int osc_num, float cutoff) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  if (cutoff < 20.0f || cutoff > 20000.0f) {
    return -2; // Invalid cutoff frequency
  }
  lowpass_set(&state.osc[osc_num].lp, cutoff); // Initialize lowpass filter
  return 0; // Success
}

float mf_lowpass_get(int osc_num) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  return state.osc[osc_num].lp.cutoff;
}

int mf_mute_all() {
  for (int i = 0; i < OSC_COUNT; i++) {
    state.osc[i].amp = 0.0f; // Mute all oscillators
  }
  return 0; // Success
}

int mf_wavetable_write(enum Wave wave, float *data) {
  if (wave < CA || wave > CD) {
    return -2; // Invalid custom wave type
  }
  switch (wave) {
    case CA:
      memcpy(wave_data.ca, data, sizeof(float) * TABLE_SIZE);
      break;
    case CB:
      memcpy(wave_data.cb, data, sizeof(float) * TABLE_SIZE);
      break;
    case CC:
      memcpy(wave_data.cc, data, sizeof(float) * TABLE_SIZE);
      break;
    case CD:
      memcpy(wave_data.cd, data, sizeof(float) * TABLE_SIZE);
      break;
    default:
      return -3; // Unsupported wave type
  }
  return 0;                       // Success
}

int mf_exit() {
  state.flags.exit = 1; // Set exit flag
  return 0;            // Success
}

///////
/// Lua bindings
///////

static int _l_mf_lua_index(int input) {
  return input - 1; // Lua uses 1-based indexing, C uses 0-based
}

static int l_mf_wave_set(lua_State *L) {
  int osc_num = luaL_checkinteger(L, 1);
  osc_num = _l_mf_lua_index(osc_num);
  const char *waveStr = luaL_checkstring(L, 2);
  enum Wave wave;

  if (strcmp(waveStr, "SINE") == 0) {
    wave = SINE;
  } else if (strcmp(waveStr, "SQUARE") == 0) {
    wave = SQUARE;
  } else if (strcmp(waveStr, "TRIANGLE") == 0) {
    wave = TRIANGLE;
  } else if (strcmp(waveStr, "SAW") == 0) {
    wave = SAW;
  } else if (strcmp(waveStr, "NOISE") == 0) {
    wave = NOISE;
  } else if (strcmp(waveStr, "CA") == 0) {
    wave = CA;
  } else if (strcmp(waveStr, "CB") == 0) {
    wave = CB;
  } else if (strcmp(waveStr, "CC") == 0) {
    wave = CC;
  } else if (strcmp(waveStr, "CD") == 0) {
    wave = CD;
  } else {
    return luaL_error(L, "Invalid wave type: %s", waveStr);
  }

  int result = mf_wave_set(osc_num, wave);
  lua_pushinteger(L, result);
  return 1; // 
}

static int l_mf_freq_set(lua_State *L) {
  int osc_num = luaL_checkinteger(L, 1);
  osc_num = _l_mf_lua_index(osc_num);
  float freq = luaL_checknumber(L, 2);
  int result = mf_freq_set(osc_num, freq);
  lua_pushinteger(L, result);
  return 1; // 
}

static int l_mf_freq_get(lua_State *L) {
  int osc_num = luaL_checkinteger(L, 1);
  osc_num = _l_mf_lua_index(osc_num);
  float freq = mf_freq_get(osc_num);
  if (freq < 0) {
    return luaL_error(L, "Invalid oscillator index: %d", osc_num + 1);
  }
  lua_pushnumber(L, freq);
  return 1; // 
}

static int l_mf_amp_set(lua_State *L) {
  int osc_num = luaL_checkinteger(L, 1);
  osc_num = _l_mf_lua_index(osc_num);
  float amp = luaL_checknumber(L, 2);
  int result = mf_amp_set(osc_num, amp);
  lua_pushinteger(L, result);
  return 1; // 
}

static int l_mf_amp_get(lua_State *L) {
  int osc_num = luaL_checkinteger(L, 1);
  osc_num = _l_mf_lua_index(osc_num);
  float amp = mf_amp_get(osc_num);
  if (amp < 0) {
    return luaL_error(L, "Invalid oscillator index: %d", osc_num + 1);
  }
  lua_pushnumber(L, amp);
  return 1; // 
}

static int l_mf_pan_set(lua_State *L) {
  int osc_num = luaL_checkinteger(L, 1);
  osc_num = _l_mf_lua_index(osc_num);
  float pan_l = luaL_checknumber(L, 2);
  float pan_r = luaL_checknumber(L, 3);
  int result = mf_pan_set(osc_num, pan_l, pan_r);
  lua_pushinteger(L, result);
  return 1; // 
}

static int l_mf_pan_get_l(lua_State *L) {
  int osc_num = luaL_checkinteger(L, 1);
  osc_num = _l_mf_lua_index(osc_num);
  float pan_l = mf_pan_get_l(osc_num);
  if (pan_l < 0) {
    return luaL_error(L, "Invalid oscillator index: %d", osc_num + 1);
  }
  lua_pushnumber(L, pan_l);
  return 1; // 
}

static int l_mf_pan_get_r(lua_State *L) {
  int osc_num = luaL_checkinteger(L, 1);
  osc_num = _l_mf_lua_index(osc_num);
  float pan_r = mf_pan_get_r(osc_num);
  if (pan_r < 0) {
    return luaL_error(L, "Invalid oscillator index: %d", osc_num + 1);
  }
  lua_pushnumber(L, pan_r);
  return 1; // 
}

static int l_mf_lowpass_set(lua_State *L) {
  int osc_num = luaL_checkinteger(L, 1);
  osc_num = _l_mf_lua_index(osc_num);
  float cutoff = luaL_checknumber(L, 2);
  int result = mf_lowpass_set(osc_num, cutoff);
  lua_pushinteger(L, result);
  return 1; // 
}

static int l_mf_lowpass_get(lua_State *L) {
  int osc_num = luaL_checkinteger(L, 1);
  osc_num = _l_mf_lua_index(osc_num);
  float cutoff = mf_lowpass_get(osc_num);
  if (cutoff < 0) {
    return luaL_error(L, "Invalid oscillator index: %d", osc_num + 1);
  }
  lua_pushnumber(L, cutoff);
  return 1; // 
}

static int l_mf_mute_all(lua_State *L) {
  int result = mf_mute_all();
  lua_pushinteger(L, result);
  return 1; // 
}

static int l_mf_wavetable_write(lua_State *L) {
  const char *waveStr = luaL_checkstring(L, 1);
  enum Wave wave;

  if (strcmp(waveStr, "CA") == 0) {
    wave = CA;
  } else if (strcmp(waveStr, "CB") == 0) {
    wave = CB;
  } else if (strcmp(waveStr, "CC") == 0) {
    wave = CC;
  } else if (strcmp(waveStr, "CD") == 0) {
    wave = CD;
  } else {
    return luaL_error(L, "Invalid custom wave type: %s", waveStr);
  }
  //
  luaL_checktype(L, 2, LUA_TTABLE);
  float data[TABLE_SIZE];
  for (int i = 0; i < TABLE_SIZE; i++) {
    lua_rawgeti(L, 2, i + 1); // Lua arrays are 1-based
    data[i] = luaL_checknumber(L, -1);
    lua_pop(L, 1);
  }
  //
  int result = mf_wavetable_write(wave, data);
  lua_pushinteger(L, result);
  return 1; // 
}

static int l_mf_exit(lua_State *L) {
  int result = mf_exit();
  lua_pushinteger(L, result);
  return 1; // 
}

static const struct luaL_Reg mf_funcs[] = {
    {"wave_set", l_mf_wave_set},
    {"freq_set", l_mf_freq_set},
    {"freq_get", l_mf_freq_get},
    {"amp_set", l_mf_amp_set},
    {"amp_get", l_mf_amp_get},
    {"pan_set", l_mf_pan_set},
    {"pan_get_l", l_mf_pan_get_l},
    {"pan_get_r", l_mf_pan_get_r},
    {"lowpass_set", l_mf_lowpass_set},
    {"lowpass_get", l_mf_lowpass_get},
    {"mute_all", l_mf_mute_all},
    {"wavetable_write", l_mf_wavetable_write},
    {"exit", l_mf_exit},
    {NULL, NULL} // Sentinel
};

int luaopen_mf(lua_State *L) {
  luaL_newlib(L, mf_funcs);
  return 1; // Return the library table
}

///////
/// System functions
///////

lua_State *mf_lua_init(char *script_path) {
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  lua_pushglobaltable(L);
  luaL_setfuncs(L, mf_funcs, 0);
  lua_pop(L, 1); // Remove mf module from stack
  lua_pushnumber(L, TABLE_SIZE); // Push TABLE_SIZE to Lua
  lua_setglobal(L, "TABLE_SIZE");
  lua_pushnumber(L, TICK_WAIT); // Push TABLE_SIZE to Lua
  lua_setglobal(L, "TICK_WAIT");
  if (luaL_dofile(L, script_path) != LUA_OK) {
    fprintf(stderr, "Error running Lua script: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1); // Remove error message from stack
    exit(1); // Exit on error
  }
  return L;
}

int mf_init() {
  // Initialize oscillators
  for (int i = 0; i < OSC_COUNT; i++) {
    state.osc[i].freq = 440.0f;
    state.osc[i].phase = 0.0f;
    state.osc[i].amp = 0.0f; // Set default amplitude
    state.osc[i].amp_l = 1.0f;
    state.osc[i].amp_r = 1.0f;
    state.osc[i].wave = SINE; // Set default wave type
    lowpass_init(&state.osc[i].lp, 20000.0f); // Initialize lowpass filter
  }
  // Set up flags
  state.flags.exit = 0;

  // Init wave data
  for (int i = 0; i < TABLE_SIZE; i++) {
    wave_data.sine[i] = (float)sin((double)i / (double)TABLE_SIZE * M_PI * 2);
    wave_data.square[i] = (i < TABLE_SIZE / 2) ? 1 : -1;
    wave_data.triangle[i] = (1.0f - 2.0f * fabsf((float)i / (float)TABLE_SIZE - 0.5f));
    wave_data.saw[i] = (2.0f * (float)i / (float)TABLE_SIZE - 1.0f);
    wave_data.noise[i] = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
    wave_data.ca[i] = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
    wave_data.cb[i] = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
    wave_data.cc[i] = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
    wave_data.cd[i] = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
  }
  return 0; // Success
}


int tick = 0;
int mf_run_lua(lua_State *L) {
  lua_getglobal(L, "Loop");              // Push function onto stack
  lua_pushnumber(L, tick);               // Push argument
  if (lua_pcall(L, 1, 0, 0) != LUA_OK) { // Call function with 1 arg, 0 results
    fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    exit(1); // Exit on error
  }
  tick++; // Increment tick count
  return 0;
}

void mf_loop(char *script_path) {
  static lua_State *L = NULL;
  if (!L) {
    L = mf_lua_init(script_path);
  }
  mf_run_lua(L); // Call Lua main function on each loop iteration
}
