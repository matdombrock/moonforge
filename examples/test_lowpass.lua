amp_set(1, 0.9)
wave_set(1, "SQUARE")
function Loop(tick)
  local lfo_rate_ticks = 1000
  local lfo = 0.5 + math.sin(tick / lfo_rate_ticks) * 0.5
  -- lowpass_set(1, 20 + lfo * 20000)
end
