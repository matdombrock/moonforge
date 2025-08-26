local mfl = require("util.mflib")

wave_set(1, "SINE")
bus_amp_set(0.8)

local bpm = 120

local release = false
local release_speed = 0.005
local attack_speed = 0.008

function Loop(tick)
  local tt = mfl.time.track_tick(tick, bpm, 8)
  if mfl.time.on_beat(tt, bpm, 1) then
    release = false
  end
  if mfl.time.on_beat(tt, bpm, 4) then
    release = true
  end
  if release then
    local amp = mfl.util.eerp(amp_get(1), 0, release_speed)
    amp_set(1, amp)
  else
    local amp = mfl.util.eerp(amp_get(1), 0.8, attack_speed)
    amp_set(1, amp)
  end
end
