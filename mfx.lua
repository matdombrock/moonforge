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
local bpm = 120

-- Main loop
function Loop(tick)
  local tt = tick % beat_to_ticks(bpm, track_len)
  if beat_to_ticks(bpm, 1) == tt then
    print("1")
    freq_change(1, 100)
  end
  if beat_to_ticks(bpm, 4) == tt then
    print("4")
    freq_change(1, -100)
  end
end
