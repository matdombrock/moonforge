v_clear()

local slice = 3.14 / 6
local lfo_text_f = 64
local lfo_text = 0.5 + math.sin(tick / lfo_text_f) * 0.5
local lfo_text2 = 0.5 + math.sin(slice + tick / lfo_text_f) * 0.5
local lfo_text3 = 0.5 + math.sin((slice * 2) + tick / lfo_text_f) * 0.5
local lfo_text4 = 0.5 + math.sin((slice * 3) + tick / lfo_text_f) * 0.5
local lfo_text5 = 0.5 + math.sin((slice * 4) + tick / lfo_text_f) * 0.5
local lfo_text6 = 0.5 + math.sin((slice * 5) + tick / lfo_text_f) * 0.5

local lfo = 0.5 + math.sin(tick / 1024) * 0.5
lfo = lfo * (1 + lfo_text)

for y = 0, v_height do
  for x = 0, v_width do
    local c = (tick / 128) + (lfo * 8) + (x - (v_width / 2)) / y % 16
    v_pixel(x, y, c)
  end
end

local notes = {"C4", "D4", "E4", "F4", "G4", "A4", "B4",
               "C5", "D5", "E5", "F5", "G5", "A5", "B5",
               "C6", "D6", "E6", "F6", "G6", "A6", "B6",
               "C7", "D7", "E7", "F7", "G7", "A7", "B7"
}
local function get_note(pos)
  pos = math.floor(pos)
  -- pos = pos % #notes
  if pos > #notes then
    pos = #notes
  end
  if pos < 1 then
    pos = 1
  end
  return notes[pos]
end
local pos = (0.1 + lfo * 0.8) * (#notes / 2)
if (mem_get(1) ~= math.floor(pos)) then
  mem_set(math.floor(pos), 1)
  enable(1)
  note(get_note(pos), 1)
  enable(2)
  pan(-1, 2)
  env(16,8,24,2)
  note(get_note(pos + 2), 2)
  enable(3)
  pan(1, 3)
  env(24,8,16,3)
  note(get_note(pos + 5), 3)
end
bus_amp(0.75)
bus_lowpass(1000,0.8)

local rms_hist = get_bus_rms_hist()
local chunk_size = v_width / #rms_hist[1]
for i = 1, #rms_hist[1] do
  local avg = (rms_hist[1][i] + rms_hist[2][i]) / 2
  local h = avg * v_height
  local x = (i - 1) * chunk_size
  v_rect(x, 20 + v_height - h, chunk_size, h, 2)
end

local rms_l = get("bus_rms", 1)
local rms_r = get("bus_rms", 2)
local height_l = rms_l * v_height
v_rect(2, 0, 16, height_l, 10)
v_rect(4, 2, 16, height_l, 2)
local height_r = rms_r * v_height
v_rect(v_width - 18, 0, 16, height_r, 10)
v_rect(v_width - 20, 2, 16, height_r, 2)

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

local tmasc = {
  {0,0,1,1,1,1,0,0},
  {0,1,0,0,0,0,1,0},
  {0,1,1,0,0,1,1,0},
  {0,1,0,0,0,0,1,0},
  {0,1,0,1,1,0,1,0},
  {1,1,0,0,0,0,1,1},
  {1,0,1,1,1,1,0,1},
  {0,0,1,0,0,1,0,0},
}
local tmasc2 = {
  {0,0,1,1,1,1,0,0},
  {0,1,0,0,0,0,1,0},
  {0,1,1,0,0,1,1,0},
  {0,1,0,1,1,0,1,0},
  {1,1,0,1,1,0,1,1},
  {1,1,0,1,1,0,1,1},
  {0,0,1,1,1,1,0,0},
  {0,0,1,0,0,1,0,0},
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
local col = 16
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

local mt1 = tmasc
local rms_synth2 = get("rms", 2)
if rms_synth2 > 0.35 then
  mt1 = tmasc2
end
v_rect(-24 + lfo * (v_width / 2), v_height - 22, 16, 16, 10)
print_t(-24 + lfo * (v_width / 2) - 8, v_height - 28, 3, 14, mt1)

local mt2 = tmasc
local rms_synth1 = get("rms", 1)
if rms_synth1 > 0.35 then
  mt2 = tmasc2
end
v_rect(lfo * (v_width / 2), v_height - 22, 16, 16, 9)
print_t(lfo * (v_width / 2) - 8, v_height - 28, 3, 15, mt2)

local mt3 = tmasc
local rms_synth3 = get("rms", 3)
if rms_synth3 > 0.3 then
  mt3 = tmasc2
end
v_rect(24 + lfo * (v_width / 2), v_height - 22, 16, 16, 10)
print_t(24 + lfo * (v_width / 2) - 8, v_height - 28, 3, 14, mt3)
