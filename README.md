# ☾ M O O N F O R G E ☽
![mf_logo](doc/mf.png)

Moonforge is a minimalist audio lab written in C. It's scripted via it's built-in Lua interpreter using the [MF Lua API](doc/API.md). 

## Build & Package
### Requirements:
- Portaudio
- Lua

### Build
```bash
./pkg.sh
```

Final packaged build files will be in `./dist`.

## Run

### Run Syntax:
```bash
./mf <path/to/file.lua>
```

### Run Example:
```bash
./mf examples/test.lua
```

## Lua Scripting
All Lua scripts must contain a `Loop(tick)` function. The `tick` value represents the current iteration of the main loop. 

A basic script might look like this:
```lua
-- Setup
amp_set(1, 1) -- Set the amp of the first osc to 1
-- Main loop
function Loop(tick)
    print(tick)
end
```

For scripting examples see the [examples](examples) directory.

## Lua API
See the [MF Lua API Docs](doc/API.md) for core API functions.

## Higher Level Libraries
This project also includes [`mflib.lua`](lua_include/mflib.lua) which contains wrapper functions around the core API which make it easier to work with. 

The `mflib.lua` library is written in pure Lua, so anything it does can be done manually. Users are encouraged to write their own wrapper libraries (for fun). 
