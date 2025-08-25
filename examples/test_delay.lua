local mfl = require("util.mflib")
local bpm = 120

local delay_ticks = mfl.beat_to_ticks(bpm, 2)
-- Set the bus delay to the right channel only to make the effect more pronounced
bus_delay_set_r(delay_ticks, 0.5, 0.2)
wave_set(1, "SQUARE")

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
    mfl.fade(1, 0, 16)
  else
    mfl.fade(1, 0.8, 16)
  end
end
