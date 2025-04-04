local lfo1 = 0.5 * math.sin(tick / 2048) + 0.5
lfo1 = math.floor(lfo1 * 16)
env(1, 16 + lfo1, 16, 1)
local lfo2 = 0.5 * math.sin(tick / 3333) + 0.5
lfo2 = math.floor(lfo2 * 16)
enable(2)
env(1, 16 + lfo2, 16, 2)

local notes = { "D3", "D4", "A4", "D5", "R" }

local function reset()
  for i = 1, #notes do
    mem_set(0, i)
  end
end

if tick % 64 == 0 then
  local n = math.random(1, #notes)
  local c = 0
  while mem_get(n) == 1 do
    n = math.random(1, #notes)
    c = c + 1
    if c == 4 then
      reset()
    end
  end
  mem_set(1, n)
  local nt = notes[n]
  note(nt, 1)
  if n == 1 then
    wave(4, 1)
  else
    wave(1, 1)
  end
end

if tick % 96 == 0 then
  local n = math.random(1, #notes)
  local c = 0
  while mem_get(n) == 1 do
    n = math.random(1, #notes)
    c = c + 1
    if c == 4 then
      reset()
    end
  end
  mem_set(1, n)
  local nt = notes[n]
  note(nt, 2)
  if n == 1 then
    wave(4, 2)
  else
    wave(1, 2)
  end
end

bus_lowpass(1200, 0.1)
pan(-1, 1)
pan(1, 2)
bus_amp(2)

speed(0.75)
