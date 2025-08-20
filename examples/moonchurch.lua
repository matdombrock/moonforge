local mfl = require("util.mflib")

local cut = 800 -- Cutoff frequency for lowpass filter
wave_set(1, "CA")
pan_set(1, 1, 0)
lowpass_set(1, cut)

wave_set(2, "CA")
pan_set(2, 0, 1)
lowpass_set(2, cut)

wave_set(3, "CA")
lowpass_set(3, cut)

local function init_wavetable()
  local down = math.random(1, 4)
  local harms = math.random(2, 8) -- Random number of harmonics between 1 and 8
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

local bpm = 60

local release = { false, false }

local off = { 0, 0, 0 }

function Loop(tick)
  local tt = mfl.track_tick(tick, bpm, 4) -- Assuming bpm is 60 and track length is 4 beats
  if mfl.on_beat(tt, bpm, 1) then
    init_wavetable()
    mfl.note_random(1, { "C3", "C4" }) -- Set note for oscillator 1
    release[1] = false
    off[1] = math.random(1, 4)
    off[2] = math.random(1, 4)
    off[3] = math.random(1, 4)
    print("off: " .. off[1] .. ", " .. off[2] .. ", " .. off[3])
  end
  if mfl.on_beat(tt, bpm, 1 + ((1 + off[1]) / 16)) then
    mfl.note_random(1, { "D3", "D4" }) -- Set note for oscillator 1
    release[1] = true
  end
  if mfl.on_beat(tt, bpm, 1 + ((2 + off[1] + off[2]) / 16)) then
    mfl.note_random(2, { "E3", "E4" }) -- Set note for oscillator 1
    release[2] = false
  end
  if mfl.on_beat(tt, bpm, 1 + ((4 + off[1] + off[2] + off[3]) / 16)) then
    mfl.note_random(2, { "G4", "G5" }) -- Set note for oscillator 1
    release[2] = true
  end
  if mfl.on_beat(tt, bpm, 1 + ((6 + off[1] + off[2] + off[3]) / 16)) then
    if math.random() < 0.5 then
      mfl.note_random(2, { "G2", "A3" }) -- Set note for oscillator 1
      release[2] = true
    end
  end
  if mfl.on_beat(tt, bpm, 2 + ((4 + off[1] + off[2] + off[3]) / 16)) then
    mfl.note_random(3, { "G4", "G5" }) -- Set note for oscillator 1
    release[3] = true
  end
  if mfl.on_beat(tt, bpm, 2 + ((6 + off[1] + off[2] + off[3]) / 16)) then
    if math.random() < 0.5 then
      mfl.note_random(3, { "G2", "A3" }) -- Set note for oscillator 1
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
end
