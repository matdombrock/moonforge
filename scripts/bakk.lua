wave(3, 1)
env(64, 128, 64, 1)

enable(2)
env(1, 24, 8, 2)
amp(0.8, 2)
wave(5, 2)
lowpass(800, 1, 2)

enable(3)
env(64, 256, 64, 3)
wave(3, 3)

enable(4)
env(96, 256, 96, 4)
wave(3, 4)
lowpass(800, 4)

enable(5)
env(32,16, 4, 5)
wave(5, 5)
lowpass(1900, 1, 5)

local max_time = 256
local tt = (tick % max_time)

local notes = { 84, 86, 88 }
local function rand_note()
  local note = notes[math.random(1, #notes)]
  while note == mem_get(1) do
    note = notes[math.random(1, #notes)]
  end
  mem_set(note, 1)
  return note
end

if mem_get(2) == -1 then
  mem_set(1, 2)
end

if tt == mem_get(2) then
  local n = rand_note() - 12
  local mod = 0
  if math.random() > 0.5 then
    mod = 12
    wave(2, 1)
  else
    wave(3, 1)
  end
  note(n + mod, 1)
  note(n, 2)
  mem_set(math.random(32, 32 + tt + (max_time / 16)) % max_time, 2)
end

if tt == 0 then
  local n = rand_note()
  note(n - 24, 3)
  note(n - 24 - 12, 4)
end

local lfo_detune1 = 0.5 * math.sin(tick / 3) + 0.5
local lfo_detune2 = 0.5 * math.sin(tick / 128) + 0.5
local lfo_detune3 = 0.5 * math.sin(tick / 2048) + 0.5
detune((lfo_detune1 * 0.01) + (lfo_detune3 * 0.05), 1)
detune((lfo_detune2 * 0.001) + (lfo_detune3 * 0.05), 3)
detune((lfo_detune2 * 0.001) + (lfo_detune3 * 0.05) + 0.005, 4)

if (tick + 8) % 64 == 0 then
  note(66, 5)
end
local lfo_train = 0.5 * math.sin(4.6 + (tick / (max_time * 8))) + 0.05
amp(0.5 + lfo_train * 0.5, 5)


local lfo_lp1 = 0.5 * math.sin(tick / max_time) + 0.5
local lfo_lp2 = 0.5 * math.sin(tick / 3) + 0.5
bus_lowpass(1200 + (lfo_lp1 * 800) + (lfo_lp2 * 400), 1.5 + lfo_train * 1.5)

speed(0.25)
