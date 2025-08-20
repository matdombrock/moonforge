# MOON FORGE

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

## Lua API

### Wave Names
- `"SINE"` - sine wave
- `"SQUARE"` - square wave
- `"TRIANGLE"` - triangle wave
- `"SAW"` - saw wave
- `"NOISE'` - white noise
- `"CA"` - custom oscillator a
- `"CB"` - custom oscillator b
- `"CC"` - custom oscillator c
- `"CD"` - custom oscillator d

### `wave_set(osc_num, wave_name)` 
Sets the waveform for the given oscillator number. 
- `osc_num` - target oscillator
- `wave_name` - the name of the waveform to use

