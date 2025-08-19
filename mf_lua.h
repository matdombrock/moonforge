#include "mf.h"
#include <string.h>
#include <lua.h>
#include <lauxlib.h>

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

// This function is called to register the mf library with Lua.
// It creates a new table with the functions defined in mf_funcs and returns it.
// The functions can then be called from Lua scripts using the mf library.
// For example, to set the frequency of oscillator 0 to 440 Hz, you would
// call `mf.freq_set(0, 440.0)` in Lua.
// // The library can be loaded in Lua using `require "mf"`.
// // The functions can be called as `mf.beat_to_ticks(bpm, beat)`,
// // `mf.wave_set(oscNum, wave)`, etc.
