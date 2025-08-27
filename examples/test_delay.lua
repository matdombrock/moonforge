local mfl = require("util.mflib")
local bpm = 120

local delay_ticks = mfl.time.beat_to_ticks(bpm, 2)
local release_speed = 0.005
local attack_speed = 0.05
-- Set the bus delay to the right channel only to make the effect more pronounced
effect_set(1, "DELAY", delay_ticks, 0.5, 0.6)
wave_set(1, "SQUARE")

local release = false
function Loop(tick)
  local tt = mfl.time.track_tick(tick, bpm, 4)

  if mfl.time.on_beat(tt, bpm, 1) then
    release = false
  end
  if mfl.time.on_beat(tt, bpm, 1.25) then
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
