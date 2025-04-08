-- Set the waveform for the first voice to square
wave(2)
-- Create an LFO that oscillates between -1 and 1 every second
local lfo = math.sin((tick * 6.28) / 128)
-- Set the pan of the first voice to the LFO value
pan(lfo, 1)
-- We can use the same LFO again to detune the oscillator
-- Set the detune of the first voice to the LFO value divided by 2
detune(1 + lfo, 1)
bus_amp(0.75)
