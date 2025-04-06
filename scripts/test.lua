v_clear()

local m = 512
local n = math.floor((tick / 2) % m)

-- v_rect(0, 0, math.floor(((tick % 360) / 360) * 360), 240, tick % 4 + 1)

for y = 0, n do
  for x = 0, n do
    local c = 2
    if n > (m/2) then
      c = 3
    end
    v_pixel(x, y, c)
  end
end
-- for y = 0, 240 do
--   for x = 0, 360 do
--     v_pixel(x, y, tick % 4 + 1)
--   end
-- end

-- v_rect(tick % 360, tick % 240, 36, 24, 3)
