amp_set(1, 0.4)
wave_set(1, "SINE")

function Loop(tick)
  local amp = math.sin(tick / 60) * 0.5 + 0.5
  amp_set(1, amp)
  -- freq_set(1, 440 + amp * 220)
end
