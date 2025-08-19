#include <math.h>
#include <portaudio.h>
#include <math.h>
#include <string.h>


#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>

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

//////

static int l_mf_lua_index(int input) {
  return input - 1; // Lua uses 1-based indexing, C uses 0-based
}

static int l_mf_beat_to_ticks(lua_State *L) {
    float bpm = luaL_checknumber(L, 1);
    float beat = luaL_checknumber(L, 2);
    int ticks = mf_beat_to_ticks(bpm, beat);
    lua_pushinteger(L, ticks);
    return 1; // Return the number of results
}

static int l_mf_wave_set(lua_State *L) {
    int osc_num = luaL_checkinteger(L, 1);
    osc_num = l_mf_lua_index(osc_num);
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
    } else {
        return luaL_error(L, "Invalid wave type: %s", waveStr);
    }

    int result = mf_wave_set(osc_num, wave);
    lua_pushinteger(L, result);
    return 1; // Return the number of results
}

static int l_mf_freq_set(lua_State *L) {
    int osc_num = luaL_checkinteger(L, 1);
    osc_num = l_mf_lua_index(osc_num);
    float freq = luaL_checknumber(L, 2);
    int result = mf_freq_set(osc_num, freq);
    lua_pushinteger(L, result);
    return 1; // Return the number of results
}

static int l_mf_freq_change(lua_State *L) {
    int osc_num = luaL_checkinteger(L, 1);
    osc_num = l_mf_lua_index(osc_num);
    float freqMod = luaL_checknumber(L, 2);
    int result = mf_freq_change(osc_num, freqMod);
    lua_pushinteger(L, result);
    return 1; // Return the number of results
}

static int l_mf_amp_set(lua_State *L) {
    int osc_num = luaL_checkinteger(L, 1);
    osc_num = l_mf_lua_index(osc_num);
    float amp = luaL_checknumber(L, 2);
    int result = mf_amp_set(osc_num, amp);
    lua_pushinteger(L, result);
    return 1; // Return the number of results
}

static int l_mf_amp_change(lua_State *L) {
    int osc_num = luaL_checkinteger(L, 1);
    osc_num = l_mf_lua_index(osc_num);
    float ampMod = luaL_checknumber(L, 2);
    int result = mf_amp_change(osc_num, ampMod);
    lua_pushinteger(L, result);
    return 1; // Return the number of results
}

static int l_mf_mute_all(lua_State *L) {
    int result = mf_mute_all();
    lua_pushinteger(L, result);
    return 1; // Return the number of results
}

static const struct luaL_Reg mf_funcs[] = {
    {"beat_to_ticks", l_mf_beat_to_ticks},
    {"wave_set", l_mf_wave_set},
    {"freq_set", l_mf_freq_set},
    {"freq_change", l_mf_freq_change},
    {"amp_set", l_mf_amp_set},
    {"amp_change", l_mf_amp_change},
    {"mute_all", l_mf_mute_all},
    {NULL, NULL} // Sentinel
};

int luaopen_mf(lua_State *L) {
    luaL_newlib(L, mf_funcs);
    return 1; // Return the library table
}

//////

void mf_setup() {
  // mf_mute_all(); // Mute all oscillators initially
  // //
  // mf_amp_set(0, 0.5f); // Set amplitude for oscillator 0
  // mf_wave_set(0, SAW); // Set wave type for oscillator 0
  // //
  // mf_amp_set(1, 0.5f);    // Set amplitude for oscillator 0
  // mf_wave_set(1, SAW);    // Set wave type for oscillator 0
  // mf_freq_set(0, 439.0f); // Set frequency for oscillator 0
}

int tick = 0;
int mf_run_lua(lua_State *L) {
  lua_getglobal(L, "Loop");              // Push function onto stack
  lua_pushnumber(L, tick);              // Push argument
  if (lua_pcall(L, 1, 0, 0) != LUA_OK) { // Call function with 1 arg, 0 results
    fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
  }
  tick++; // Increment tick count
  return 0;
}

lua_State *mf_lua_init() {
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  // luaL_requiref(L, "mf", luaopen_mf, 1); // Load mf module
  lua_pushglobaltable(L);
  luaL_setfuncs(L, mf_funcs, 0);
  lua_pop(L, 1); // Remove mf module from stack
  if (luaL_dofile(L, "../mf_lua.lua") != LUA_OK) {
    fprintf(stderr, "Error running Lua script: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1); // Remove error message from stack
  }
  return L;
}

void mf_loop(int tick) {
  static lua_State *L = NULL;
  if (!L) {
    L = mf_lua_init();
  }
  mf_run_lua(L); // Call Lua main function on each loop iteration
}


