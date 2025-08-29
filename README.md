# ☾ M O O N F O R G E ☽
![mf_logo](doc/mf.png)

MoonForge is a minimalist modular synthesis lab written in C. It's scripted via it's built-in Lua interpreter using the [MF Lua API](doc/API.md). 

While MoonForge can be used as a semi-traditional synthesizer and tracker, it's built for experimental, evolving, generative audio soundscapes and procedural compositions. 

**Features & Goals**

✅ Fully controlled by Lua

✅ Simple API

✅ Wavetable sythesis engine

✅ Dynamic FX chain / routing

✅ Proceduaral / generative audio

✅ Music tracker

✅ Data sonification

⛔ 2D wavetables

⛔ MIDI support (live playing)

✅ Designed for embedded linux

✅ Automatic built-in recording

✅ Fun toy for audio nerds

**Available FX**

✅ All effects are written in C and controlled by Lua.

✅ Lowpass

✅ Delay

⛔ Highpass

⛔ Reverb

⛔ Waveshaping distortion

⛔ Bitcrusher

⛔ Downsample

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

### Recording
MoonForge has built-in support for recording to WAV format. When a track is ended with the `exit()` API call a recording of your audio will be saved to your current directory. 

The default max recording length is 240 seconds. When this limit is reached, the current recording will be written and a new one will start automatically. 
