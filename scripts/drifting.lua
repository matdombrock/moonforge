lowpass(300, 3, 1)
local lfo_env = 0.5 * math.sin(tick / 1024) + 0.5
lfo_env = math.floor(lfo_env * 16)
env(3 + lfo_env, 8 + lfo_env, 5 + lfo_env, 1)
enable(2)
enable(3)
enable(4)
enable(5)
enable(6)
wave(5, 6)
amp(0.5, 6)
enable(7)
wave(5, 7)
amp(0.75, 7)
env(1, 0, 8, 7)

local phrases = {
  {"C5", "E5", "G5", "B5"},
  {"E5", "G5", "B5", "D5"},
  {"G5", "B5", "D6", "G6"},
  {"B5", "D6", "F6", "B6"},
}

if tick % 2048 > 1024 then
  phrases = {
    {"D5", "E5", "F5", "F6"},
    {"F5", "G5", "C5", "C6"},
    {"G5", "A5", "F6", "A5"},
    {"D5", "D6", "F6", "A5"},
  }
end

local function random_phrase()
  local phrase = math.random(1, #phrases)
  while phrase == mem_get(1) do
    phrase = math.random(1, #phrases)
  end
  mem_set(phrase, 1)
end

if mem_get(1) == -1 then
  random_phrase()
end

local phrase = phrases[mem_get(1)]

local tt = tick % 256

if tt == 0 then
  note(phrase[1], 1)
  local n = string.sub(phrase[1], 1, 1)
  note(n .. "4", 2)
  local n2 = string.sub(phrase[2], 1, 1)
  note(n2 .. "4", 3)
  local n3 = string.sub(phrase[3], 1, 1)
  note(n3 .. "4", 4)
  local n4 = string.sub(phrase[4], 1, 1)
  note(n4 .. "4", 5)
  note(1, 7)
  lowpass(1800, 1, 7)
end
if tt == 64 then
  note(phrase[2], 1)
  if math.random() > 0.1 then note(1, 7) end
  lowpass(800, 1, 7)
end
if tt == 128 then
  note(phrase[3], 1)
  if math.random() > 0.3 then note(1, 7) end
  lowpass(1200, 1, 7)
end
if tt == 128 + 64 then
  note(phrase[4], 1)
  if math.random() > 0.5 then note(1, 7) end
  lowpass(500, 1, 7)
end

if tt == 128 + 96 then
  local lfo_chance = 0.5 * math.sin(tick / 2048) + 0.5
  if math.random() > (lfo_chance * 0.5) then note(phrase[4], 1) end
  if math.random() > 0.5 then note(1, 7) end
  lowpass(1500, 1, 7)
end

if tick % 512 + 128 == 0 then
  random_phrase()
end

local lfo_amp_dis = 0.25 * math.sin(tick / 256) + 0.25

local lfo_amp_chords = 0.5 * math.sin(tick / 128) + 0.5
amp(lfo_amp_chords, 3)
amp(lfo_amp_chords * lfo_amp_dis, 4)
amp(lfo_amp_chords, 5)

local lfo_lead = 0.5 * math.sin(tick / 4) + 0.7
amp(lfo_lead, 1)
detune(lfo_lead * -0.01, 1)

lowpass(600 - 400 * lfo_amp_chords, 2, 6)
