#pragma once
#include <string.h>
#include "../lua/src/lua.h"
#include "../lua/src/lauxlib.h"
#include "../lua/src/lualib.h"

#include "globals.h"
#include "util.h"
#include "notes.h"

// LUA BINDS
int luaB_debug(lua_State *L) {
    const char *msg = luaL_checkstring(L, 1);
    debug("lua: %s\n", msg);
    return 0;
}

int luaB_get_target(lua_State *L, int pnum) {
    int target = luaL_optinteger(L, pnum, 1);
    target -= LUA_INDEX; // Lua indices start at 1 but C indices start at 0
    if (target < 0) {
        printf("lua warn: Lua indices start at 1. Got: %d\n", target);
        target = 0;
    }
    target = target > OSC_COUNT ? OSC_COUNT : target;
    return target;
}

int luaB_freq(lua_State *L) {
    float val = luaL_checknumber(L, 1);
    int target = luaB_get_target(L, 2); 
    _synth[target].freq = val;
    debug("lua: freq(%f, %d)\n", val, target);
    return 0;
}

int luaB_note(lua_State *L) {
    const char *note = luaL_checkstring(L, 1);
    int target = luaB_get_target(L, 2);
    for (int i = 0; i < sizeof(notes) / sizeof(Note); i++) {
        if (strcmp(notes[i].name, note) == 0) {
            _synth[target].freq = notes[i].freq;
            debug("lua: note: %s, freq: %f, target: %d\n", note, notes[i].freq, target);
            break;
        }
    }
    return 0;
}

int luaB_detune(lua_State *L) {
    float val = luaL_checknumber(L, 1); 
    int target = luaB_get_target(L, 2);
    _synth[target].detune = val;
    debug("lua: detune(%f, %d)\n", val, target);
    return 0;
}
int luaB_amp(lua_State *L) {
    float val = luaL_checknumber(L, 1);
    int target = luaB_get_target(L, 2);
    _synth[target].amp = val;
    debug("lua: amp(%f, %d)\n", val, target);
    return 0;
}
int luaB_wave(lua_State *L) {
    int val = luaL_checkinteger(L, 1); 
    int target = luaB_get_target(L, 2);
    _synth[target].wave = val;
    debug("lua: wave(%d, %d)\n", val, target);
    return 0;
}
int luaB_mono(lua_State *L) {
    for (int i = 1; i < OSC_COUNT; i++) {
        _synth[i].amp = 0;
    }
    debug("lua: mono()\n");
    return 0;
}
void luaB_binds(lua_State *L) {
    lua_register(L, "dbg", luaB_debug);  // Using dbg which is shorter and not a reserved nam
    lua_register(L, "freq", luaB_freq);
    lua_register(L, "note", luaB_note);
    lua_register(L, "detune", luaB_detune);
    lua_register(L, "amp", luaB_amp);
    lua_register(L, "wave", luaB_wave);
    lua_register(L, "mono", luaB_mono);
}

void luaB_run() {
    if (_sys.sample_num % LUA_RATE == 0) {
        debug("g_sys_sample_num: %d\n", _sys.sample_num);
        // Init a new lua lua_State
        lua_State *L = luaL_newstate();
        luaB_binds(L);
        luaL_openlibs(L);  // Load standard libraries
        
        // Pass system variables to Lua
        lua_pushnumber(L, _sys.sample_num);
        lua_setglobal(L, "sample_num");
        float seconds = (float)_sys.sample_num / (float)SAMPLE_RATE;
        lua_pushnumber(L, seconds);
        lua_setglobal(L, "seconds");
        
        if (luaL_dofile(L, _sys.filepath) != LUA_OK) {
            fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1); // remove error message from stack
        }
    }
}
