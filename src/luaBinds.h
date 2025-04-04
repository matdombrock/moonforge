#pragma once
#include <string.h>
#include <math.h>
#include <time.h>
#include "../lua/src/lua.h"
#include "../lua/src/lauxlib.h"
#include "../lua/src/lualib.h"

#include "globals.h"
#include "util.h"
#include "notes.h"

// Lua util
int luaB_get_target(lua_State *L, int pnum) {
    int target = luaL_optinteger(L, pnum, 1);
    target -= LUA_INDEX; // Lua indices start at 1 but C indices start at 0
    if (target < 0) {
        target = 0;
    }
    target = target > OSC_COUNT ? OSC_COUNT : target;
    return target;
}

// LUA BINDS
int luaB_debug(lua_State *L) {
    const char *msg = luaL_checkstring(L, 1);
    debug("lua: %s\n", msg);
    return 0;
}

int luaB_enable(lua_State *L) {
    int target = luaB_get_target(L, 1);
    _synth[target].enabled = 1;
    debug("lua: activate(%d)\n", target);
    return 0;
}

int luaB_disable(lua_State *L) {
    int target = luaB_get_target(L, 1);
    _synth[target].enabled = 0;
    debug("lua: deactivate(%d)\n", target);
    return 0;
}

int luaB_freq(lua_State *L) {
    float val = luaL_checknumber(L, 1);
    int target = luaB_get_target(L, 2); 
    if (val < 0) {
        val = 0.0f;
    }
    _synth[target].freq = val;
    _synth[target].env_pos = 0;
    debug("lua: freq(%f, %d)\n", val, target);
    return 0;
}
int luaB_note(lua_State *L) {
    int target = luaB_get_target(L, 2);

    if (lua_type(L, 1) == LUA_TSTRING) {
        const char *note = luaL_checkstring(L, 1); 
        Note n = note_by_name(note);
        _synth[target].freq = n.freq;
        if (strcmp(note, "R") == 0) {
            _synth[target].freq = 0;
        }
        _synth[target].env_pos = 0;
        debug("lua: note: %s, freq: %f, target: %d\n", note, n.freq, target);
        return 0;
    } 
    else if (lua_type(L, 1) == LUA_TNUMBER) {
        int note = luaL_checkinteger(L, 1);
        if (note < 0 || note > 127) {
            note = 0;
        }
        float freq = 440.0f * powf(2.0f, (note - 69) / 12.0f);
        if (note == 0) {
            freq = 0;
        }
        _synth[target].freq = freq;
        _synth[target].env_pos = 0; 
        debug("lua: note: %d, freq: %f, target: %d\n", note, freq, target);
        return 0;
    }

    luaL_error(L, "Invalid argument for note. Expected string or number.");
    return 0;
}
int luaB_detune(lua_State *L) {
    float val = luaL_checknumber(L, 1); 
    int target = luaB_get_target(L, 2);
    if (val < 0) {
        val = 0.0f;
    }
    _synth[target].detune = val;
    debug("lua: detune(%f, %d)\n", val, target);
    return 0;
}
int luaB_amp(lua_State *L) {
    float val = luaL_checknumber(L, 1);
    int target = luaB_get_target(L, 2);
    val = val < 0 ? 0 : val; 
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
int luaB_solo(lua_State *L) {
    int target = luaB_get_target(L, 1);
    for (int i = 0; i < OSC_COUNT; i++) {
        _synth[i].enabled = 0;
    }
    _synth[target].enabled = 1;
    debug("lua: solo(%d)\n", target);
    return 0;
}
int luaB_pan(lua_State *L) {
    float val = luaL_checknumber(L, 1);
    int target = luaB_get_target(L, 2);
    _synth[target].pan = val;
    debug("lua: pan(%f, %d)\n", val, target);
    return 0;
}
int luaB_env(lua_State *L) {
    // Input is in ticks, convert to seconds
    float attack = luaL_checknumber(L, 1) / 128.0f;
    float sustain = luaL_checknumber(L, 2) / 128.0f;
    float release = luaL_checknumber(L, 3) / 128.0f;
    attack = attack < 0 ? 0 : attack; 
    sustain = sustain < 0 ? 0 : sustain;
    release = release < 0 ? 0 : release;
    int target = luaB_get_target(L, 4);
    _synth[target].env[0] = attack;
    _synth[target].env[1] = sustain;
    _synth[target].env[2] = release;
    _synth[target].env_enabled = 1;
    debug("lua: env(%f, %f, %f, %d)\n", attack, sustain, release, target);
    return 0;
}
int luaB_lowpass(lua_State *L){
    float cutoff = luaL_checknumber(L, 1);
    float resonance = luaL_optnumber(L, 2, 1.0f);
    int target = luaB_get_target(L, 3); 
    if (cutoff < 10.0f) {
        cutoff = 10.0f;
    }
    if (resonance < 0.1f) {
        resonance = 0.1f;
    }
    _synth[target].lp_cutoff = cutoff;
    _synth[target].lp_resonance = resonance;
    _synth[target].lp_enabled = 1;
    debug("lua: lowpass(%f, %f, %d)\n", cutoff, resonance, target);
    return 0;
}
int luaB_highpass(lua_State *L){
    float cutoff = luaL_checknumber(L, 1);
    float resonance = luaL_optnumber(L, 2, 1.0f);
    int target = luaB_get_target(L, 3);
    _synth[target].hp_cutoff = cutoff;
    _synth[target].hp_resonance = resonance;
    _synth[target].hp_enabled = 1;
    debug("lua: highpass(%f, %f, %d)\n", cutoff, resonance, target);
    return 0;
}
int luaB_speed(lua_State *L) {
    float val = luaL_checknumber(L, 1);
    if (val <= 0) {
        val = 0.001f;
    }
    if (val > 1) {
        val = 1.0f;
    }
    _sys.speed = val;
    debug("lua: speed(%f)\n", val);
    return 0;
}
int luaB_bus_lowpass(lua_State *L) {
    float cutoff = luaL_checknumber(L, 1);
    float resonance = luaL_checknumber(L, 2);
    _bus.lp_cutoff = cutoff;
    _bus.lp_resonance = resonance;
    debug("lua: bus_lowpass(%f, %f)\n", cutoff, resonance);
    return 0;
}
int luaB_bus_amp(lua_State *L) {
    float val = luaL_checknumber(L, 1);
    if (val < 0) {
        val = 0.0f;
    }
    _bus.amp = val;
    debug("lua: bus_amp(%f)\n", val);
    return 0;
}
// Memory functions
int luaB_mem_set(lua_State *L) {
    float val = luaL_checknumber(L, 1);
    int index = luaL_optinteger(L, 2, 1);
    index -= LUA_INDEX;
    _sys.memory[index] = val;
    debug("lua: mem_set(%d, %f)\n", index, val);
    return 0;
}
int luaB_mem_get(lua_State *L) {
    int index = luaL_checkinteger(L, 1);
    index -= LUA_INDEX;
    if (floor(_sys.memory[index]) == _sys.memory[index]) {
        lua_pushinteger(L, _sys.memory[index]);
        debug("lua: mem_get(%d)\n", index);
        return 1;
    }
    float val = _sys.memory[index];
    lua_pushnumber(L, val);
    debug("lua: mem_get(%d)\n", index);
    return 1;
}
void luaB_binds(lua_State *L) {
    lua_register(L, "dbg", luaB_debug);  // Using dbg which is shorter and not a reserved name
    lua_register(L, "enable", luaB_enable);
    lua_register(L, "disable", luaB_disable);
    lua_register(L, "freq", luaB_freq);
    lua_register(L, "note", luaB_note);
    lua_register(L, "detune", luaB_detune);
    lua_register(L, "amp", luaB_amp);
    lua_register(L, "wave", luaB_wave);
    lua_register(L, "solo", luaB_solo);
    lua_register(L, "pan", luaB_pan);
    lua_register(L, "env", luaB_env);
    lua_register(L, "lowpass", luaB_lowpass);
    lua_register(L, "highpass", luaB_highpass);
    lua_register(L, "speed", luaB_speed);
    lua_register(L, "bus_lowpass", luaB_bus_lowpass);
    lua_register(L, "bus_amp", luaB_bus_amp);
    lua_register(L, "mem_set", luaB_mem_set);
    lua_register(L, "mem_get", luaB_mem_get);
}

// Global Lua state to avoid constant reallocation
static lua_State *L_global = NULL;

// Initialize the global Lua state
void luaB_init() {
    if (L_global == NULL) {
        L_global = luaL_newstate();
        luaB_binds(L_global);
        luaL_openlibs(L_global);  // Load standard libraries
    }
}

// Clean up the global Lua state
void luaB_cleanup() {
    if (L_global != NULL) {
        lua_close(L_global);
        L_global = NULL;
    }
}

void luaB_run() { 
    // This is why ticks never == 0
    // There is a bit of a rounding error
    float seconds = (float)_sys.sample_num / (float)SAMPLE_RATE;
    int tick = floor(seconds * _sys.speed * 128);
    if (tick <= _sys.tick_num) return;

    struct timespec ts;
    debug("\nlua start\n");
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
    }

    _sys.tick_num = tick;
    // Offset tick by -1
    // We want to start at 0
    tick -= 1;
    debug("tick: %d\n", tick);
    debug("seconds: %f\n", seconds);

    // Initialize Lua state if needed
    if (L_global == NULL) {
        luaB_init();
    }

    // Reset Lua state for new execution
    lua_settop(L_global, 0);  // Clear the stack

    // Pass system variables to Lua
    lua_pushnumber(L_global, seconds);
    lua_setglobal(L_global, "seconds");
    lua_pushnumber(L_global, tick);
    lua_setglobal(L_global, "tick");
    lua_pushstring(L_global, &_sys.keypress);
    lua_setglobal(L_global, "keypress");

    if (luaL_dofile(L_global, _sys.filepath) != LUA_OK) {
        fprintf(stderr, "Lua error: %s\n", lua_tostring(L_global, -1));
        if (_sys.stop_on_error) {
            fprintf(stderr, "Stopping execution due to Lua error.\n");
            lua_close(L_global);
            L_global = NULL;
            exit(1);
        }
        lua_pop(L_global, 1); // remove error message from stack
    }

    struct timespec ts2;
    if (clock_gettime(CLOCK_REALTIME, &ts2) == -1) {
        perror("clock_gettime");
    }
    
    // Calculate the time difference in nano seconds
    long diff_ns;
    if (ts2.tv_sec == ts.tv_sec) {
        diff_ns = ts2.tv_nsec - ts.tv_nsec;
    } else {
        diff_ns = (ts2.tv_sec - ts.tv_sec) * 1000000000L + (ts2.tv_nsec - ts.tv_nsec);
    }
    
    // Divide by 1000 to get microseconds
    _sys.luatimes[_sys.luatimes_index % LUA_TIME_WINDOW] = diff_ns / 1000.0f;
    _sys.luatimes_index++;
    
    // Calculate rolling average
    float sum_time = 0.0f;
    for (int i = 0; i < LUA_TIME_WINDOW; i++) {
        sum_time += _sys.luatimes[i];
    }
    _sys.luatime = sum_time / (float)LUA_TIME_WINDOW;
    
    // Log in microseconds
    debug("lua time: %fµs \n", _sys.luatime);
}
