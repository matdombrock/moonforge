
#include <lua.h>
#include <lualib.h>
int mf_init_lua();
int mf_run_lua(lua_State *L);
void mf_setup();
void mf_loop(int tick);
