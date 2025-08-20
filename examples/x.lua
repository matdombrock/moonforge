local mfl = require("mflib")

amp_set(1, 0.5)
freq_set(1, 840)

function Loop(tick)
  if mfl.on_beat(tick, 120, 1) then
    freq_set(1, 440)
  end
  if tick > 1000 then
    exit()
  end
  print("Tick: " .. tick)
end
