#include "mf_lua.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
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
