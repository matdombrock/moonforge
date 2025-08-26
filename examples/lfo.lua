amp_set(1, 0.5)
wave_set(1, "SAW")
pan_set(1, 1, 0)
lowpass_set(1, 12000)

amp_set(2, 0.5)
wave_set(2, "SINE")
pan_set(2, 0, 1)

function Loop(tick)
  local lfo_rate_ticks = 1000
  local lfo = 0.5 + math.sin(tick / (lfo_rate_ticks / 2)) * 0.5
  local lfo2_rate_ticks = 10000
  local lfo2 = 0.5 + math.sin(tick / (lfo2_rate_ticks / 2)) * 0.5
  local base = 200 + lfo2 * 200
  local lp_cut = base + lfo * 12000
  local freq = base + lfo * 220
  lowpass_set(1, lp_cut)
  freq_set(2, freq)
  bus_amp_set(lfo2)
end
