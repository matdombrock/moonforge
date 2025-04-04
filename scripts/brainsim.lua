enable(2)
enable(3)
enable(4)
enable(5)

wave(3, 1)
wave(3, 2)
wave(5, 3)
wave(5, 4)
wave(5, 5)

env(1, 0, 8, 3)
env(1, 0, 8, 4)

amp(0.6, 3)

note("A3", 1)
if tick % 2048 > 1024 then
  note("C3", 1)
end

local notes = { "C4", "G4", "A4", "C5", "G5", "A5", "C6" }

local lfo1 = 0.5 * math.sin(tick / 999) + 0.5
lfo1 = lfo1 * 8
lfo1 = math.floor(lfo1)
lfo1 = lfo1 + 1
lfo1 = lfo1 * 8

local lfo2 = 0.5 * math.sin(tick / 666) + 0.5
lfo2 = lfo2 * 8
lfo2 = math.floor(lfo2)
lfo2 = lfo2 + 1
lfo2 = lfo2 * 8

local lfo3 = 0.5 * math.sin(tick / 333) + 0.5
lfo3 = lfo3 * 8
lfo3 = math.floor(lfo3)
lfo3 = lfo3 + 1
lfo3 = lfo3 * 8

local lfo = 0.5 + math.sin(lfo1 + lfo2 + lfo3) * 0.5
lfo = lfo * 8
lfo = math.floor(lfo)
lfo = lfo + 1
lfo = lfo * 8

if tick % lfo == 0 then
  note(notes[math.random(1, #notes)], 2)
  note(1, 3)
end

local lfo_detune = 0.5 * math.sin(tick / 256) + 0.5
detune(lfo_detune * 0.005, 1)
detune(lfo_detune * 0.01, 2)

if tick % 32 == 0 then
  note("C3", 4)
end

local lfo_noise1 = 0.5 * math.sin(tick / 1024) + 0.5
local lfo_noise2 = 0.25 * math.sin(tick / 2) + 0.25
amp(math.sin(lfo_noise1 + lfo_noise2), 5)

bus_lowpass(800, 4)
