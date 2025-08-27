amp_set(1, 1)
wave_set(1, "SQUARE")
effect_set(1, "LOWPASS", 200)
effect_set(1, "LOWPASS", 200)

function Loop(tick)
  local lpc = math.sin(tick / 100) * 1000 + 1200
  local lpc2 = math.sin(tick / 1200) * 1000 + 1200
  effect_set(1, "LOWPASS", lpc)
  effect_set(1, "LOWPASS", lpc2)
end
