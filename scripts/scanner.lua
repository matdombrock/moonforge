local lfo = 0.5 + math.sin(tick / 256) * 0.5
for y = 0, v_height do
  for x = 0, v_width do
    local c = (tick / 128) + (lfo * 32) + (x - (v_width / 2)) / y % 8
    v_pixel(x, y, c)
  end
end
enable(1)
freq((693 + lfo * 693) / 2, 1)
enable(2)
freq((693 - lfo * 369) / 2, 2)
