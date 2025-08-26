wave_set(1, "SINE")

function Loop(tick)
  local amp = math.sin(tick / 30) * 0.5 + 0.5
  amp_set(1, amp)
end
