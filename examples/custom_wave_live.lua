local mfl = require("mflib")
-- local wav = {}
-- for i = 1, TABLE_SIZE do
--   local cos = math.cos(i / TABLE_SIZE * math.pi * 2)
--   local sin = math.sin((i * 4) / TABLE_SIZE * math.pi * 2)
--   wav[i] = (cos + sin) / 2
-- end
-- custom_wave_set("CA", wav)

wave_set(1, "CA")
amp_set(1, 0.5)
freq_set(1, 440)

local track_len = 8
local bpm = 120

function Loop(tick)
  local tt = mfl.track_tick(tick, bpm, track_len)
  if mfl.on_beat(tt, bpm, 1) then
    mfl.make_wave("CA", function(x)
      return math.sin(x)
    end)
  end
  if mfl.on_beat(tt, bpm, 4) then
    mfl.make_wave("CA", function(x)
      return x > math.pi and -1 or 1
    end)
  end
end
