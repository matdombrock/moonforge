-- lowpass_set(1, 8000)

wave_set(1, "SINE")

local mfl = require("util.mflib")
local bpm = 120

local release = false
function Loop(tick)
  local tt = mfl.track_tick(tick, bpm, 4)
  if mfl.on_beat(tt, bpm, 1) then
    release = false
  end
  if mfl.on_beat(tt, bpm, 2) then
    release = true
  end
  if release then
    mfl.fade(1, 0, 10)
  else
    mfl.fade(1, 0.5, 10)
  end
end
