amp_set(1, 0.9)
wave_set(1, "SINE")

function Loop(tick)
  local amp = math.sin(tick / 30) * 0.5 + 0.5
  amp_set(0, amp * 0.8)
end
