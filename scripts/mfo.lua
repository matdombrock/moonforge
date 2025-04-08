v_clear()

local slice = 3.14 / 6
local lfo_text = 0.5 + math.sin(tick / 32) * 0.5
local lfo_text2 = 0.5 + math.sin(slice + tick / 32) * 0.5
local lfo_text3 = 0.5 + math.sin((slice * 2) + tick / 32) * 0.5
local lfo_text4 = 0.5 + math.sin((slice * 3) + tick / 32) * 0.5
local lfo_text5 = 0.5 + math.sin((slice * 4) + tick / 32) * 0.5
local lfo_text6 = 0.5 + math.sin((slice * 5) + tick / 32) * 0.5

local lfo = 0.5 + math.sin(tick / 500) * 0.5
lfo = lfo * (1 + lfo_text)

for y = 0, v_height do
  for x = 0, v_width do
    local c = (tick / 128) + (lfo * 32) + (x - (v_width / 2)) / y % 16
    v_pixel(x, y, c)
  end
end

enable(1)
note(44 + math.floor(lfo * 9) * 5, 1)
pan(-1, 1)

enable(2)
note(99 - math.floor(lfo * 9) * 5, 2)
pan(1, 2)

enable(3)
freq(210 + lfo * 32, 3)
amp(0.5, 3)
pan(-1, 3)

enable(4)
freq(420 - lfo * 32, 4)
amp(0.5, 4)
pan(1, 4)

local tx = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
}

local tm = {
  {1,1,1,1,1,1,1,1},
  {0,1,0,0,1,0,0,1},
  {1,1,0,0,1,0,0,1},
  {1,1,0,0,1,0,0,1},
  {1,1,0,0,1,0,0,1},
  {1,1,0,0,1,0,0,1},
  {1,1,0,0,1,0,0,1},
  {1,1,0,1,1,0,1,1},
}

local to = {
  {1,1,1,1,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,1,1,1,0,0,0},
}

local tn = {
  {1,1,1,1,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
}

local tf = {
  {1,1,1,1,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,0,0,0,0},
  {1,1,1,1,0,0,0,0},
  {1,1,0,0,0,0,0,0},
  {1,1,0,0,0,0,0,0},
  {1,1,0,0,0,0,0,0},
  {1,1,0,0,0,0,0,0},
}

local tr = {
  {1,1,1,1,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,1,1,0,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,1,0,0,0},
}

local tg = {
  {1,1,1,1,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,0,0,0,0,0,0},
  {1,1,0,0,0,0,0,0},
  {1,1,0,1,1,0,0,0},
  {1,1,0,1,1,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,1,1,1,0,0,0},
}

local te = {
  {1,1,1,1,1,0,0,0},
  {1,1,0,0,0,0,0,0},
  {1,1,0,0,0,0,0,0},
  {1,1,1,1,0,0,0,0},
  {1,1,0,0,0,0,0,0},
  {1,1,0,0,0,0,0,0},
  {1,1,0,0,1,0,0,0},
  {1,1,1,1,1,0,0,0},
}

local tmoon = {
  {0,1,1,1,1,0,0,0},
  {1,1,1,1,1,0,0,0},
  {1,0,0,1,1,1,0,0},
  {0,0,0,1,1,1,0,0},
  {0,0,0,1,1,1,0,0},
  {1,0,0,1,1,1,0,0},
  {1,1,1,1,1,0,0,0},
  {0,1,1,1,1,0,0,0},
}

local tforge = {
  {0,0,0,1,1,1,0,0},
  {0,0,1,1,1,1,0,0},
  {0,1,1,1,1,1,0,0},
  {1,1,1,1,1,0,0,0},
  {1,1,1,1,1,1,0,0},
  {1,1,1,0,1,1,1,0},
  {0,0,0,0,0,1,1,1},
  {0,0,0,0,0,0,1,1},
}

local function print_t(x, y, s, c, t)
  for xx = 1, #t do
    for yy = 1, #t[xx] do
      local v = t[yy][xx]
      if v == 1 then v_rect(s * xx + x, s * yy + y, s, s, c) end
    end
  end
end

local pw = 42
local ph = 24 + -lfo_text * 8
local col = 14
local col2 = 2

print_t(pw, ph, 4, col, tm)
print_t(pw - 2, ph - 2, 4, col2, tm)
ph = 24 + -lfo_text2 * 8
print_t(pw + 36, ph, 4, col, to)
print_t(pw + 36 - 2, ph - 2, 4, col2, to)
ph = 24 + -lfo_text3 * 8
print_t(pw + 60, ph, 4, col, to)
print_t(pw + 60 - 2, ph - 2, 4, col2, to)
ph = 24 + -lfo_text4 * 8
print_t(pw + 84, ph, 4, col, tn)
print_t(pw + 84 - 2, ph - 2, 4, col2, tn)
ph = 24 + -lfo_text6 * 8
print_t(pw + 108, ph, 4, 9, tmoon)
print_t(pw + 108 - 2, ph - 2, 4, 12, tmoon)

ph = 24 + lfo_text * 8
print_t(pw, ph + 38, 4, 9, tforge)
print_t(pw - 2, ph + 38 - 2, 4, 12, tforge)
ph = 24 + lfo_text2 * 8
print_t(pw + 36, ph + 38, 4, col, tf)
print_t(pw + 36 - 2, ph + 38 - 2, 4, col2, tf)
ph = 24 + lfo_text3 * 8
print_t(pw + 60, ph + 38, 4, col, to)
print_t(pw + 60 - 2, ph + 38 - 2, 4, col2, to)
ph = 24 + lfo_text4 * 8
print_t(pw + 84, ph + 38, 4, col, tr)
print_t(pw + 84 - 2, ph + 38 - 2, 4, col2, tr)
ph = 24 + lfo_text5 * 8
print_t(pw + 108, ph + 38, 4, col, tg)
print_t(pw + 108 - 2, ph + 38 - 2, 4, col2, tg)
ph = 24 + lfo_text6 * 8
print_t(pw + 132, ph + 38, 4, col, te)
print_t(pw + 132 - 2, ph + 38 - 2, 4, col2, te)
