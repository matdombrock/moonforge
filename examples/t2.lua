local mfl = require("mflib")

-- Initial setup
wave_set(1, "SAW")
amp_set(1, 0.5)
freq_set(1, 440)
pan_set(1, 0.5, 1)

wave_set(2, "SAW")
amp_set(2, 0.5)
mfl.note_set(2, "C3")
pan_set(2, 1, 0.5)

local track_len = 4
local bpm = 60

local release = false

-- Main loop
function Loop(tick)
  local tt = mfl.track_tick(tick, bpm, track_len)
  if mfl.on_beat(tt, bpm, 1) then
    release = false
    mfl.note_set(1, "E4")
  end
  if mfl.on_beat(tt, bpm, 2) then
    release = true
  end
  if mfl.on_beat(tt, bpm, 3) then
    release = false
    -- mfl.note_set(1, "C#6")
    mfl.note_random(1, { "C5", "C6", "D4", "D5", "D6", "E5", "E6" })
  end
  if mfl.on_beat(tt, bpm, 4) then
    release = true
  end
  if release then
    mfl.fade(1, 0, 0.5)
  else
    mfl.fade(1, 0.5, 0.5)
  end
end
