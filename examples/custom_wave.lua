local mfl = require("mflib")

mfl.make_wave("CA", function(x)
  local sin = math.sin(x)
  -- Add some randomness to the square wave
  sin = sin + mfl.noise(1)
  if sin > 1 then
    sin = 1
  elseif sin < -1 then
    sin = -1
  end
  return sin
end)

wave_set(1, "CA")
amp_set(1, 0.5)
freq_set(1, 440)

function Loop(tick)
end
