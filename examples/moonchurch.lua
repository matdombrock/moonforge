local mfl = require("util.mflib")

mfl.randomseed()

local rec_time = 9     -- Record time in seconds
local fade_time = 8    -- Fade out time in seconds
local fade_time_ticks = mfl.seconds_to_ticks(fade_time)
local fade_out = false -- Flag to fade out the end
local fade_step = 0
local bpm = 60
local release = { false, false } -- Release flags for oscillators
local off = { 0, 0, 0 }

local cut = 800 -- Cutoff frequency for lowpass filter
local delay_len = 70000
local delay_feedback = 0.1
local delay_mix = 0.2
wave_set(1, "CA")
pan_set(1, 1, 0)
lowpass_set(1, cut)
delay_set(1, delay_len, delay_feedback, delay_mix)

wave_set(2, "CA")
pan_set(2, 0, 1)
lowpass_set(2, cut)
delay_set(2, delay_len, delay_feedback, delay_mix)

wave_set(3, "CA")
lowpass_set(3, cut)
delay_set(3, delay_len, delay_feedback, delay_mix)

wave_set(4, "NOISE")
amp_set(4, 0.025)
pan_set(4, 1, 0)
lowpass_set(4, cut)
delay_set(4, delay_len, delay_feedback, delay_mix * 1.5)

wave_set(5, "NOISE")
amp_set(5, 0.025)
pan_set(5, 0, 1)
lowpass_set(5, cut)
delay_set(5, delay_len, delay_feedback, delay_mix * 1.5)

bus_amp_set(1.4)

local function init_wavetable()
  local down = math.random(1, 4)
  local harms = math.random(2, 8)
  print("Harmonics: " .. harms)
  print("Down: " .. down)
  mfl.wavetable_make("CA", function(x)
    x = x / down
    local sineAcc = 0
    for i = 0, harms do
      sineAcc = sineAcc + (math.sin(x * (i * 2)) * (1 - (i / harms)))
    end
    sineAcc = sineAcc / math.ceil(harms / 2)
    return sineAcc
  end)
end

function Loop(tick)
  bpm = bpm + math.sin(tick / 10000) * 0.01
  local tt = mfl.track_tick(tick, bpm, 4)
  if mfl.on_beat(tt, bpm, 1) then
    init_wavetable()
    mfl.note_random(1, { "C3", "C4" })
    release[1] = false
    off[1] = math.random(1, 4)
    off[2] = math.random(1, 4)
    off[3] = math.random(1, 4)
    print("off: " .. off[1] .. ", " .. off[2] .. ", " .. off[3])
  end
  if mfl.on_beat(tt, bpm, 1 + ((1 + off[1]) / 16)) then
    mfl.note_random(1, { "D3", "D4" })
    release[1] = true
  end
  if mfl.on_beat(tt, bpm, 1 + ((2 + off[1] + off[2]) / 16)) then
    mfl.note_random(2, { "E3", "E4" })
    release[2] = false
  end
  if mfl.on_beat(tt, bpm, 1 + ((4 + off[1] + off[2] + off[3]) / 16)) then
    mfl.note_random(2, { "G4", "G5" })
    release[2] = true
  end
  if mfl.on_beat(tt, bpm, 1 + ((6 + off[1] + off[2] + off[3]) / 16)) then
    if math.random() < 0.5 then
      mfl.note_random(2, { "G2", "A3" })
      release[2] = true
    end
  end
  if mfl.on_beat(tt, bpm, 2 + ((4 + off[1] + off[2] + off[3]) / 16)) then
    mfl.note_random(3, { "G4", "G5" })
    release[3] = true
  end
  if mfl.on_beat(tt, bpm, 2 + ((6 + off[1] + off[2] + off[3]) / 16)) then
    if math.random() < 0.5 then
      mfl.note_random(3, { "G2", "A3" })
      release[3] = true
    end
  end
  if release[1] then
    mfl.fade(1, 0, 0.1)
  else
    mfl.fade(1, 0.85, 10)
  end
  if release[2] then
    mfl.fade(2, 0, 0.15)
  else
    mfl.fade(2, 0.85, 10)
  end
  if release[3] then
    mfl.fade(3, 0, 0.05)
  else
    mfl.fade(3, 0.85, 10)
  end
  mfl.warble(1, tt, 222, 0.9)
  mfl.warble(2, tt, 300, 0.6)
  mfl.warble(3, tt, 333, 1.6)

  -- Noise LFO
  amp_set(4, math.sin(1000 - (tick / 1000)) * 0.2)
  amp_set(5, math.sin((tt / 1000)) * 0.1)

  -- Fade out at end
  if mfl.on_second(tick, rec_time - fade_time) then
    fade_out = true
    fade_step = mfl.fade_step_calc(0, bus_amp_get(), fade_time_ticks)
    print("Fading out...", fade_step)
  end
  if fade_out then
    mfl.bus_fade_step(0, fade_step)
  end
  if mfl.on_second(tick, rec_time) then
    print("Recording finished, exiting...")
    exit()
  end
end
