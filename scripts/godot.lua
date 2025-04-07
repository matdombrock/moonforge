enable(2)
enable(3)

env(1, 0, 16, 1)
env(1, 0, 16, 2)
env(1, 0, 16, 3)

wave(3, 4)

local notes = { 84, 86, 88, 89, 91 }
local function rand_note()
  local note = notes[math.random(1, #notes)]
  while note == mem_get(1) do
    note = notes[math.random(1, #notes)]
  end
  mem_set(note, 1)
  return note
end

if tick % 128 == 0 then
  note(rand_note() - 12, 1)
end
if tick % 64 == 0 then
  note(rand_note(), 2)
  note(rand_note(), 3)
end
if tick % 96 == 0 then
  note(rand_note(), 3)
end

-- Lead junk

local lead_time = 2048
local lead_time_h = 1024
local lead_start = 512
if tick % (4096 * 2) > 4096 then
  lead_time = 4096
  lead_time_h = 2048
  lead_start = 1024 + 512
end

if tick % 32 == 0 then
  local mod = 0
  if tick % lead_time_h > lead_start then
    mod = math.random(1, 3) * 12
    note(rand_note() - mod, 4)
    enable(4)
  else
    disable(4)
  end
end
local lfo_amp_lead1 = 0.5 * math.sin(tick / (tick % lead_start) + 1 ) + 0.5
local lfo_amp_lead2 = 0.5 * math.sin(tick / 2 + math.floor(tick / lead_time)) + 0.5
local lfo_amp_lead = 0
if tick % lead_time > lead_time_h then
  lfo_amp_lead = lfo_amp_lead1
else
  lfo_amp_lead = lfo_amp_lead2
end
amp(lfo_amp_lead - 0.65, 4)



v_clear()
local downscale = 8
local lfo_vis = 3 + math.floor(0.5 * math.sin(tick / 256) + 0.5) * 8
local lfo_vis2 = math.floor(lfo_amp_lead * 8)
if tick % 3 ~= 0 then lfo_vis2 = 0 end
if tick % lead_time_h > lead_start  then
  downscale = 18 - lfo_vis2 + 2
  local tt = math.floor(tick / 16)
  for y = 0, 240 / downscale do
    for x = 0, 360 / downscale do
      v_rect(-4 + x * downscale, y * downscale, downscale, downscale, (lfo_vis2 + tt + x * y % (lfo_vis2 + 1)) - (8 - lfo_vis2 * 2))
    end
  end
else
  local tt = math.floor(tick / 128)
  local tt2 = math.floor(tick / 1024)
  for y = 0, 240 / downscale do
    for x = 0, 360 / downscale do
      if x % 2 == 0 and y % 3 == 0 then
        v_rect(-4 + x * downscale, y * downscale, downscale, downscale, tt + tt2 + x + y % 9)
      else
        v_rect(-4 + x * downscale, y * downscale, downscale, downscale, tt + tt2 + x * y % 9)
      end
    end
  end
end
