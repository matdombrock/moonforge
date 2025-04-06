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

local sin = 0
local sin2 = 0
local sin3 = 0

local sins = 0
local sin2s = 0
local sin3s = 0

local sinm = 0

for x = 0, 360 do
  sin = math.sin(x / 360 * math.pi * 2 + tick / 32)
  sin2 = math.sin(x / 360 * math.pi * 2 + tick / 18)
  sin3 = (sin + sin2) / 2
  if x == 1 then
    sins = sin
    sin2s = sin2
    sin3s = sin3
  end
  if x == 180 then
    sinm = sin3
  end
  freq(440 + sin * 440, 1)
  freq(440 + sin2 * 440, 2)
  freq(440 + sin3 * 440, 3)
  for y = 0, 240 do
    local ss = sin * 40 + 40
    if y > ss and y < ss + 5 then
      v_pixel(x, y, 7)
    end
    local ss2 = sin2 * 40 + 180
    if y > ss2 and y < ss2 + 5 then
      v_pixel(x, y, 6)
    end
    local ss3 = sin3 * 40 + 120
    if y > ss3 and y < ss3 + 5 then
      v_pixel(x, y, 5)
    end
  end
end

local ym = math.floor(120 + sinm * 40) + 3

v_line(180, ym, 0, math.floor(sins * 40 + 40) + 3, 8)
v_line(180, ym, 0, math.floor(sin2s * 40 + 180) + 3, 8)
v_line(180, ym, 0, math.floor(sin3s * 40 + 120) + 3, 8)

v_line(180, ym, 360, math.floor(sin * 40 + 40) + 3, 8)
v_line(180, ym, 360, math.floor(sin2 * 40 + 180) + 3, 8)
v_line(180, ym, 360, math.floor(sin3 * 40 + 120) + 3, 8)

v_rect(180 - 3, ym - 2, 8, 8, 8)

pan(-1, 1)
pan(0, 3)
pan(1, 2)

bus_amp(0.65)

