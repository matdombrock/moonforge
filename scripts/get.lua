local rms_l = get("bus_rms", 1)
local rms_r = get("bus_rms", 2)
local rms = (rms_l + rms_r) / 2

v_clear()

local vwh = v_width / 3
local bsz = vwh / 3

local height_l = rms_l * v_height
v_rect(bsz, v_height - height_l, bsz, height_l, 2)

local height_avg = rms * v_height
v_rect(bsz + vwh, v_height - height_avg, bsz, height_avg, 8)

local height_r = rms_r * v_height
v_rect(bsz + vwh * 2, v_height - height_r, bsz, height_r, 5)

for y = 0, math.floor(v_height / 8) do
  if y % 2 == 1 then
    v_rect(0, y * 8, v_width, 8, 1)
  end
end

local lfo_amp = 0.5 * math.sin(tick / 256) + 0.5
amp(lfo_amp, 1)

local lfo_pan = math.sin(tick / 33)
pan(lfo_pan, 1)

wave(3, 1)
