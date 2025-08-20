local mfl = require("mflib")

-- Initial setup
mute_all()
wave_set(1, "SAW")
wave_set(2, "SAW")
amp_set(1, 0.5)
amp_set(2, 0.5)
freq_set(1, 440)
freq_set(2, 439)

local track_len = 4
local bpm = 420

-- Main loop
function Loop(tick)
  local tt = mfl.track_tic(tick, bpm, track_len)
  if mfl.on_beat(tt, bpm, 1) then
    amp_set(1, 0.5)
    amp_set(2, 0.5)
    freq_change(1, 100)
  end
  if mfl.on_beat(tt, bpm, 2) then
    mute_all()
  end
  if mfl.on_beat(tt, bpm, 3) then
    amp_set(1, 0.5)
    amp_set(2, 0.5)
    freq_change(1, -100)
  end
  if mfl.on_beat(tt, bpm, 4) then
    mute_all()
  end
end
