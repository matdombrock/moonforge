local mfl = require("util.mflib")

-- Create a custom wave "CA" using a mathematical function
mfl.wavetable_make("CA", function(x)
  return math.sin(x) * math.sin(x * 2) * math.sin(x * 3)
end)

amp_set(1, 1)     -- Set the amplitude of oscillator 1 to 1
wave_set(1, "CA") -- Set the wave of oscillator 1 to the custom wave "CA"

function Loop(tick)
end
