v_clear()

for y = 0, 240 do
  for x = 0, 360 do
    if x % 2 == 0 then v_pixel(x, y, 4) end
  end
end

-- for y = 0, 240 do
--   for x = 0, 360 do
--     v_pixel(x, y, tick % 4 + 1)
--   end
-- end

-- v_rect(tick % 360, tick % 240, 36, 24, 3)
