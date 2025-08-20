local mfl = require("util.mflib")
local bpm = 120

-- 1/4 note delay
local delay_time = mfl.beat_to_ticks(bpm, 1 + 1 / 4) * 1000
print("Delay time: " .. delay_time)
delay_set(1, delay_time, 0.5, 0.2)
wave_set(1, "SQUARE")
lowpass_set(1, 8000)
pan_set(1, 1, 0)
mfl.note_set(2, "C4")

delay_set(2, math.floor(delay_time / 3), 0.5, 0.2)
wave_set(2, "SQUARE")
lowpass_set(2, 8000)
pan_set(2, 0, 1)
mfl.note_set(2, "G4")

local release = false
function Loop(tick)
  local tt = mfl.track_tick(tick, bpm, 4)
  if mfl.on_beat(tt, bpm, 1) then
    -- amp_set(1, 0.5)
    release = false
  end
  if mfl.on_beat(tt, bpm, 2) then
    -- amp_set(1, 0)
    release = true
    local new_delay_time = math.random(60000, 180000) -- Convert to seconds
    print("Delay time: " .. new_delay_time)
    delay_set(1, new_delay_time, 0.5, 0.2)
    delay_set(2, math.ceil(new_delay_time / 3), 0.5, 0.2)
  end

  if release then
    mfl.fade(1, 0, 16)
    mfl.fade(2, 0, 16)
  else
    mfl.fade(1, 0.8, 16)
    mfl.fade(2, 0.8, 16)
  end
end
