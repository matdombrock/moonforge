# Lua API

Moonforge provides a minimal Lua API which can be abstracted to enable more advanced behaviors.

These are the core API functions which call C code directly.

## Main `Loop(tick)` Function
Every MoonForge Lua script must contain a main `Loop` function. 

This function will be called once per "tick" (about 1ms). 

This function call will be supplied the `tick` parameter which indicates the current tick (loop iteration). 

The most basic MoonForge script possible look like this:
```lua
function Loop(tick)
    -- Do nothing
end
```

This will not produce any audio. To make some noise we need to turn on an oscillator:
```lua
amp_set(1, 0.7) -- Set the amplitude (volume) of the first oscillator to 70% max volume
function Loop(tick)
end
```

Now we should hear a constant sine wave playing at 440Hz. To make it a bit more interesting, let's set the starting frequency to 880Hz and use an LFO to modulate it:
```lua
amp_set(1, 0.7) -- Set the amplitude (volume) of the first oscillator to 70% max volume
freq_set(1, 880)
function Loop(tick)
    local lfo = math.sin(tick / 1000) -- A slow LFO
    -- Normalize the lfo 0->1
    lfo = 0.5 + lfo * 0.5
    local freq = 20 + lfo * 440
    freq_set(1, freq)
end
```

## Global Constants

There are a few constants available to the Lua runtime:
- `VERSION` - mf version number (string)
- `OSC_COUNT` - number of available oscillators
- `TABLE_SIZE` - amount of samples in a wavetable
- `TICK_WAIT` - time in ms between each tick
- `SAMPLE_RATE` - samples per second
- `BLOCK_SIZE` - number of samples in a block
- `TUNING` - tuning of A4 in Hz

## Wave Names

When a function calls for a `wave_name` parameter, it may be one of the following:
- `"SINE"` - sine wave
- `"SQUARE"` - square wave
- `"TRIANGLE"` - triangle wave
- `"SAW"` - saw wave
- `"NOISE'` - white noise
- `"CA"` - custom oscillator a
- `"CB"` - custom oscillator b
- `"CC"` - custom oscillator c
- `"CD"` - custom oscillator d

## Funtions

### `wave_set(osc_num, wave_name)` 
Sets the waveform for the given oscillator number. 
- `osc_num` - target oscillator
- `wave_name` - the name of the waveform to use

### `wave_get(osc_num)` (planned)
Returns the current wavetable name as a string.
- `osc_num` - target oscillator

### `freq_set(osc_num, freq)` 
Sets the frequency for the given oscillator. 
- `osc_num` - target oscillator
- `freq` - target frequency in Hz

### `freq_get(osc_num)` 
Returns the current frequency for the given oscillator. 
- `osc_num` - target oscillator

### `amp_set(osc_num, amp)` 
Sets the amplitude for the given oscillator. 
- `osc_num` - target oscillator
- `amp` - target amp (0->4)

### `amp_get(osc_num)` 
Returns the current amplitude for the given oscillator. 
- `osc_num` - target oscillator

### `pan_set(osc_num, pan_l, pan_r)` 
Sets the pan for the given oscillator. This is effectively just a way to control the amplitude of each channel individually. That being said it does not override the main amplitude level and is applied as an additional multiplier. 
- `osc_num` - target oscillator
- `pan_l` - target amplitude for the left channel (0->1)
- `pan_r` - target amplitude for the right channel (0->1)

### `pan_get(osc_num)` 
Returns a Lua array with 2 values:
`{right, left}`.  
- `osc_num` - target oscillator

### `mute_all()`
Mutes all oscillators. The sets the amplitude for all oscillators to zero. It does not preserve the previous amplitude. If you want to mute a specific oscillator, just set its amp to zero. 

### `bus_amp_set(amp)`
Sets the volume for the main bus. 
- `amp` - target amp (0->4)

### `bus_amp_get()`
Returns the main bus amplitude. 

### `wavetable_write(wave_name, data)`
Writes a wavetable to a custom wavetable slot. It is only possible to write to custom wavetables ("CA", "CB", "CC", "CD").
- `wave_name` - the name of the waveform to use
- `data` - An array of `TABLE_LENGTH` sample values (-1->1)

### `effect_set(osc_num, effect_name, ...)`
Adds/sets an effect on the current effects chain for the given oscillator.
Effects are processed in the order they are added.
The default amount of available effects slots is 4 per bus. 
- `osc_num` - target oscillator, `0` for main bus
- `effect_name` - one of "NONE", "DELAY", "LOWPASS"

**Lowpass params:**
- `cutoff` - the cutoff frequency

**Delay params:**
- `delay_ticks` - the length of the delay in ticks
- `feedback` - delay feedback (0->1)
- `mix` - delay mix (0->1)

### `exit()`
Cleanly exit the program. If recording is enabled, this will save the current recording.
