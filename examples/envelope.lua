local mfl = require("util.mflib")

-- lowpass_set(1, 8000)

wave_set(1, "SINE")
bus_amp_set(0.8)

local bpm = 120

local release = false
function Loop(tick)
  local tt = mfl.track_tick(tick, bpm, 8)
  if mfl.on_beat(tt, bpm, 1) then
    release = false
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
