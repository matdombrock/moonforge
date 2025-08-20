# Lua API

## Global Constants
- `TABLE_SIZE` - the amount of samples in a wavetable
- `TICK_WAIT` - the time in ms between each tick

## Wave Names
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
- `amp` - target amp (0->1)

### `amp_get(osc_num)` 
Returns the current amplitude for the given oscillator. 
- `osc_num` - target oscillator

### `pan_set(osc_num, pan_l, pan_r)` 
Sets the pan for the given oscillator. Does not enforce inverse values.
- `osc_num` - target oscillator
- `pan_l` - target amplitude for the left channel (0->1)
- `pan_r` - target amplitude for the right channel (0->1)

### `pan_get_l(osc_num)` 
Returns the left channel pan amplitude. 
- `osc_num` - target oscillator

### `pan_get_r(osc_num)` 
Returns the right channel pan amplitude. 
- `osc_num` - target oscillator

### `lowpass_set(osc_num, cutoff)`
Sets the a lowpass filter on the target oscillator.
- `osc_num` - target oscillator
- `cutoff` - cutoff frequency (Hz)

### `lowpass_get(osc_num)`
Returns the current lowpass cutoff for the target oscillator.
- `osc_num` - target oscillator

### `mute_all()`
Mutes all oscillators.

### `custom_wave_set(wave_name, data)`
Sets a custom wavetable.
- `wave_name` - the name of the waveform to use
- `data` - An array of `TABLE_LENGTH` sample values (-1->1)

### `exit()`
Cleanly exit the program.
