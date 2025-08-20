local mfl = require("util.mflib")

wave_set(1, "SQUARE")
amp_set(1, 0.5)
freq_set(1, 440)

function Loop(tick)
  lowpass_set(1, 10000 - math.sin(tick / 100) * 9000)
end
