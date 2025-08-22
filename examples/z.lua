amp_set(1, 1)
amp_set(2, 1)
pan_set(1, 1, 0)
pan_set(2, 0, 1)
bus_amp_set(0.8)
function Loop(tick)
  if tick % 1000 == 0 then
    print("Tick: " .. tick)
  end
  -- freq_set(1, 400 + math.sin(tick / 1000) * 300 + math.sin(tick / 200) * 64)
  -- freq_set(2, 400 - math.sin(tick / 1000) * 300 - math.sin(tick / 200) * 64)
  freq_set(1, 440)
  freq_set(2, 880 + math.sin(tick / 400) * 440)
end
