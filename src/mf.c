#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <math.h>
#include <portaudio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mf.h"
#include "recording.h"
#include "lowpass.h"
#include "delay.h"

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
  if (amp < 0.0f || amp > 4.0f) {
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

float *mf_pan_get(int osc_num) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return NULL; // Invalid oscillator index
  }
  static float pan[2];
  pan[0] = state.osc[osc_num].amp_l; // Left channel amplitude
  pan[1] = state.osc[osc_num].amp_r; // Right channel amplitude
  return pan;                        // Return panning array
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
  mfx_lowpass_set(&state.osc[osc_num].lp, cutoff); // Initialize lowpass filter
  return 0; // Success
}

float mf_lowpass_get(int osc_num) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  return state.osc[osc_num].lp.cutoff;
}

int mf_delay_set(int osc_num, int delay_samples, float feedback, float mix) {
  if (osc_num < 0 || osc_num >= OSC_COUNT) {
    return -1; // Invalid oscillator index
  }
  if (delay_samples < 0 || feedback < 0.0f || feedback >= 1.0f || mix < 0.0f ||
      mix > 1.0f) {
    return -2; // Invalid delay parameters
  }
  mfx_delay_set(&state.osc[osc_num].delay, delay_samples, feedback, mix);
  return 0; // Success
}

int mf_mute_all() {
  for (int i = 0; i < OSC_COUNT; i++) {
    state.osc[i].amp = 0.0f; // Mute all oscillators
  }
  return 0; // Success
}

int mf_bus_amp_set(float amp) {
  if (amp < 0.0f || amp > 4.0f) {
    return -1; // Invalid bus amplitude value
  }
  state.bus_amp = amp; // Set the bus amplitude
  return 0;           // Success
}

float mf_bus_amp_get() {
  return state.bus_amp; // Return the bus amplitude
}

int bus_lowpass_set(float cutoff) {
  if (cutoff < 20.0f || cutoff > 20000.0f) {
    return -1; // Invalid cutoff frequency
  }
  mfx_lowpass_set(&state.bus_lp_l, cutoff); // Set left channel lowpass filter
  mfx_lowpass_set(&state.bus_lp_r, cutoff); // Set right channel lowpass filter
  return 0; // Success
}

int mf_bus_delay_set_l(int delay_samples, float feedback, float mix) {
  if (delay_samples < 0 || feedback < 0.0f || feedback >= 1.0f || mix < 0.0f ||
      mix > 1.0f) {
    return -1; // Invalid delay parameters
  }
  mfx_delay_set(&state.bus_delay_l, delay_samples, feedback, mix);
  return 0; // Success
}

int mf_bus_delay_set_r(int delay_samples, float feedback, float mix) {
  if (delay_samples < 0 || feedback < 0.0f || feedback >= 1.0f || mix < 0.0f ||
      mix > 1.0f) {
    return -1; // Invalid delay parameters
  }
  mfx_delay_set(&state.bus_delay_r, delay_samples, feedback, mix);
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
static int _l_mf_ticks_to_samples(int ticks) {
  return ticks * (int)(SAMPLE_RATE * (TICK_WAIT * 1000));
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

static int l_mf_pan_get(lua_State *L) {
  int osc_num = luaL_checkinteger(L, 1);
  osc_num = _l_mf_lua_index(osc_num);
  float *pan = mf_pan_get(osc_num);
  if (!pan) {
    return luaL_error(L, "Invalid oscillator index: %d", osc_num + 1);
  }
  lua_newtable(L);
  lua_pushnumber(L, pan[0]);
  lua_rawseti(L, -2, 1); // Lua arrays are 1-based
  lua_pushnumber(L, pan[1]);
  lua_rawseti(L, -2, 2);
  return 1; // Return the table
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

static int l_mf_delay_set(lua_State *L) {
  int osc_num = luaL_checkinteger(L, 1);
  osc_num = _l_mf_lua_index(osc_num);
  int delay_ticks = luaL_checkinteger(L, 2);
  int delay_samples = _l_mf_ticks_to_samples(delay_ticks);
  float feedback = luaL_checknumber(L, 3);
  float mix = luaL_checknumber(L, 4);
  int result = mf_delay_set(osc_num, delay_samples, feedback, mix);
  lua_pushinteger(L, result);
  return 1; // 
}

static int l_mf_mute_all(lua_State *L) {
  int result = mf_mute_all();
  lua_pushinteger(L, result);
  return 1; // 
}

static int l_mf_bus_amp_set(lua_State *L) {
  float amp = luaL_checknumber(L, 1);
  int result = mf_bus_amp_set(amp);
  lua_pushinteger(L, result);
  return 1; // 
}

static int l_mf_bus_amp_get(lua_State *L) {
  float amp = mf_bus_amp_get();
  lua_pushnumber(L, amp);
  return 1; // 
}

static int l_mf_bus_lowpass_set(lua_State *L) {
  float cutoff = luaL_checknumber(L, 1);
  int result = bus_lowpass_set(cutoff);
  lua_pushinteger(L, result);
  return 1; // 
}

static int l_mf_bus_delay_set_l(lua_State *L) {
  int delay_ticks = luaL_checkinteger(L, 1);
  int delay_samples = _l_mf_ticks_to_samples(delay_ticks);
  float feedback = luaL_checknumber(L, 2);
  float mix = luaL_checknumber(L, 3);
  int result = mf_bus_delay_set_l(delay_samples, feedback, mix);
  lua_pushinteger(L, result);
  return 1; // 
}

static int l_mf_bus_delay_set_r(lua_State *L) {
  int delay_ticks = luaL_checkinteger(L, 1);
  int delay_samples = _l_mf_ticks_to_samples(delay_ticks);
  float feedback = luaL_checknumber(L, 2);
  float mix = luaL_checknumber(L, 3);
  int result = mf_bus_delay_set_r(delay_samples, feedback, mix);
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
    {"pan_get", l_mf_pan_get},
    {"lowpass_set", l_mf_lowpass_set},
    {"lowpass_get", l_mf_lowpass_get},
    {"delay_set", l_mf_delay_set},
    {"mute_all", l_mf_mute_all},
    {"bus_amp_set", l_mf_bus_amp_set},
    {"bus_amp_get", l_mf_bus_amp_get},
    {"bus_lowpass_set", l_mf_bus_lowpass_set},
    {"bus_delay_set_l", l_mf_bus_delay_set_l},
    {"bus_delay_set_r", l_mf_bus_delay_set_r},
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

void mf_synth_callback(const void *output_buffer, unsigned long frames_per_buffer, void *user_data) {
  mf_wave_data *data = (mf_wave_data *)user_data;
  float *out = (float *)output_buffer;
  unsigned int i;
  for (i = 0; i < frames_per_buffer; i++) {
    float sample_mix_l = 0.0f;
    float sample_mix_r = 0.0f;
    for (int osc = 0; osc < OSC_COUNT; osc++) {
      float sample_a = 0.0f;
      float sample_b = 0.0f;
      // Adjust rate for sine wave frequency
      float freq = state.osc[osc].freq / TUNING;
      int phase_index = (int)state.osc[osc].phase;
      int phase_index_n = (phase_index + 1) % TABLE_SIZE;
      switch (state.osc[osc].wave) {
      case SINE:
        sample_a += data->sine[phase_index];
        sample_b += data->sine[phase_index_n];
        break;
      case SQUARE:
        sample_a += data->square[phase_index];
        sample_b += data->square[phase_index_n];
        break;
      case TRIANGLE:
        sample_a += data->triangle[phase_index];
        sample_b += data->triangle[phase_index_n];
        break;
      case SAW:
        sample_a += data->saw[phase_index];
        sample_b += data->saw[phase_index_n];
        break;
      case NOISE:
        sample_a += ((float)random() / (float)RAND_MAX) * 2.0f - 1.0f;
        sample_b += sample_a;
        break;
      case CA:
        sample_a += data->ca[phase_index];
        sample_b += data->ca[phase_index_n];
        break;
      case CB:
        sample_a += data->cb[phase_index];
        sample_b += data->cb[phase_index_n];
        break;
      case CC:
        sample_a += data->cc[phase_index];
        sample_b += data->cc[phase_index_n];
        break;
      case CD:
        sample_a += data->cd[phase_index];
        sample_b += data->cd[phase_index_n];
        break;
      default:
        sample_a += 0.0f;
        sample_b += 0.0f;
        break;
      }
      // Interpolate the samples
      float phase_frac = state.osc[osc].phase - phase_index;
      float sample = (sample_a * (1.0f - phase_frac)) + (sample_b * phase_frac);
      // Apply lowpass filter
      sample = mfx_lowpass_process(&state.osc[osc].lp, sample);
      // Apply panning and amplitude
      float amp = state.osc[osc].amp;
      sample_mix_l += sample * amp * state.osc[osc].amp_l;
      sample_mix_r += sample * amp * state.osc[osc].amp_r;
      // Apply delay effect
      sample_mix_l = mfx_delay_process(&state.osc[osc].delay, sample_mix_l);
      sample_mix_r = mfx_delay_process(&state.osc[osc].delay, sample_mix_r);
      // Increment phase
      state.osc[osc].phase = fmod(state.osc[osc].phase + freq * (TUNING * TABLE_SIZE / SAMPLE_RATE), TABLE_SIZE);
    }
    // Apply bus effects
    sample_mix_l = mfx_lowpass_process(&state.bus_lp_l, sample_mix_l);
    sample_mix_r = mfx_lowpass_process(&state.bus_lp_r, sample_mix_r);
    sample_mix_l = mfx_delay_process(&state.bus_delay_l, sample_mix_l);
    sample_mix_r = mfx_delay_process(&state.bus_delay_r, sample_mix_r);
    // Apply bus amplitude
    // printf("Bus Amp before LP: %f\n", state.bus_amp);
    float bus_amp = mfx_lowpass_process(&state.bus_amp_lp, state.bus_amp);
    // float bus_amp = state.bus_amp;
    // printf("Bus Amp after LP: %f\n", bus_amp);
    sample_mix_l *= bus_amp;
    sample_mix_r *= bus_amp;
    // Add samples to the buffer
    // Samples are read in pairs with odd samples being left and even being right
    *out++ = sample_mix_l; // Left
    *out++ = sample_mix_r; // Right
    rec_write_sample(sample_mix_l, sample_mix_r);
  }
}

lua_State *mf_lua_init(char *script_path) {
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  lua_pushglobaltable(L);
  luaL_setfuncs(L, mf_funcs, 0);
  lua_pop(L, 1); // Remove mf module from stack
  lua_pushnumber(L, OSC_COUNT);
  lua_setglobal(L, "OSC_COUNT");
  lua_pushnumber(L, TABLE_SIZE);
  lua_setglobal(L, "TABLE_SIZE");
  lua_pushnumber(L, TICK_WAIT);
  lua_setglobal(L, "TICK_WAIT");
  lua_pushstring(L, VERSION);
  lua_setglobal(L, "VERSION");
  lua_pushnumber(L, SAMPLE_RATE);
  lua_setglobal(L, "SAMPLE_RATE");
  lua_pushnumber(L, BLOCK_SIZE);
  lua_setglobal(L, "BLOCK_SIZE");
  lua_pushnumber(L, TUNING);
  lua_setglobal(L, "TUNING");

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
    mfx_lowpass_init(&state.osc[i].lp); // Initialize lowpass filter
    mfx_delay_init(&state.osc[i].delay); // Initialize delay
  }
  // Set up flags
  state.flags.exit = 0;

  // Setup state vars
  state.bus_amp = 1.0f; // Set default bus amplitude
  
  // Setup bus effects
  mfx_lowpass_init(&state.bus_lp_l);
  mfx_lowpass_init(&state.bus_lp_r);
  mfx_delay_init(&state.bus_delay_l);
  mfx_delay_init(&state.bus_delay_r);

  // Control filters
  mfx_lowpass_init(&state.bus_amp_lp);
  mfx_lowpass_set(&state.bus_amp_lp, 400.0f); // Default cutoff

  // Init wave data
  for (int i = 0; i < TABLE_SIZE; i++) {
    wave_data.sine[i] = (float)sin((double)i / (double)TABLE_SIZE * M_PI * 2);
    wave_data.square[i] = (i < TABLE_SIZE / 2) ? 1 : -1;
    wave_data.triangle[i] = (1.0f - 2.0f * fabsf((float)i / (float)TABLE_SIZE - 0.5f));
    wave_data.saw[i] = (2.0f * (float)i / (float)TABLE_SIZE - 1.0f);
    wave_data.ca[i] = 0;
    wave_data.cb[i] = 0; 
    wave_data.cc[i] = 0; 
    wave_data.cd[i] = 0; 
  }

  rec_init(); // Initialize recording system
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
