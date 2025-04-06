v_clear()

-- local c = math.floor((tick / 32) % 4) + 2

local tt = math.floor(tick  / 2)

-- for y = 0, 240 do
--   for x = 0, 360 do
--     -- if x % 2 == 0 then v_pixel(x, y, 4) end
--     -- if tick % 2 == 1 then v_pixel(x, y, 4) end
--     -- if vframe then v_pixel(x, y, 4) end
--     v_pixel(x, y,tt + (x * 2) + (y * 7) % 8)
--   end
-- end

-- for y = 0, 240 do
--   for x = 0, 360 do
--     v_pixel(x, y, tick % 4 + 1)
--   end
-- end

-- v_rect(tick % 360, tick % 240, 36, 24, 3)
--
--

enable(2)
enable(3)

for x = 0, 360 do
  local sin = math.sin(x / 360 * math.pi * 2 + tick / 32)
  local sin2 = math.sin(x / 360 * math.pi * 2 + tick / 18)
  local sin3 = (sin + sin2) / 2
  freq(440 + sin * 440, 1)
  freq(440 + sin2 * 440, 2)
  freq(440 + sin3 * 440, 3)
  for y = 0, 240 do
    local ss = sin * 40 + 120
    if y > ss and y < ss + 2 then
      v_pixel(x, y, 4)
    end
    local ss2 = sin2 * 40 + 120
    if y > ss2 and y < ss2 + 2 then
      v_pixel(x, y, 3)
    end
    local ss3 = sin3 * 40 + 120
    if y > ss3 and y < ss3 + 2 then
      v_pixel(x, y, 2)
    end
  end
end

bus_amp(0.65)
