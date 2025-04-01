local tt = tick % 128

-- Kick drum
-- A simple kick drum is made by detuning a sine wave oscillator
env(8, 0, 16, 1)
if tt == 1 then
  amp(1.8, 1)
  note("A3", 1)
elseif tt < 32 then
  detune(-tt / 32, 1)
  amp(1.5-tt / 32, 1)
elseif tt == 32 then
  amp(0, 1)
end

-- Snare drum
-- A simple snare drum is made with a noise oscillator
enable(2)
env(16, 0, 16, 2)
wave(5, 2)
highpass(250, 4, 2)
if tt == 1 then
  amp(0, 2)
elseif tt == 64 then
  note("C4", 2)
  amp(1, 2)
elseif tt == 96 then
  amp(0, 2)
end
-- Hi-hat
-- A simple hi-hat is also made with a noise oscillator
enable(3)
env(8, 0, 16, 3)
wave(5, 3)
highpass(12000, 4, 3)
local hh_speed = 32
if (tick % 512) > 256 then
  hh_speed = 16
end
if tt % hh_speed == 1 then
  note("C4", 3)
  amp(1.8, 3)
elseif tt % hh_speed == 9 then
  amp(0, 3)
end


