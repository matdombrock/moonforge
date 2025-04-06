v_clear()

for y = 0, 240 do
  for x = 0, 360 do
    v_pixel(x, y, 3)
  end
end
for y = 0, 240 do
  for x = 0, 360 do
    v_pixel(x, y, tick % 4 + 1)
  end
end

v_rect(tick % 360, tick % 240, 36, 24, 3)
