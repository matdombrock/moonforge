# ☾ M O O N F O R G E ☽
![mf_logo](doc/mf.png)

MoonForge is a minimalist modular synthesis lab written in C. It's scripted via it's built-in Lua interpreter using the [MF Lua API](doc/API.md). 

While MoonForge can be used as a semi-traditional synthesizer and tracker, it's built for experimental, evolving, generative audio soundscapes and procedural compositions. 

Possible uses cases:
- DSP lab / education tool
- Modular synthesizer
- Music Tracker
- Data / Algorithm sonification
- Toy for a very specific kind of nerd

Note: MoonForge is not in any way a scientific instrument and generally is not intended to be used as such. Lua is only able to affect the audio data at block boundaries and thus is not capable of particularly precise, low-level DSP. 

## Build & Package
### Requirements:
- [Portaudio](https://www.portaudio.com/)
- [Lua](https://www.lua.org/)
- [A unix system](https://btxx.org/public/images/unix.gif)
- A C compiler for your OS

### Windows?
No.

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
