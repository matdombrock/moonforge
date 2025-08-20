local mfl = require("mflib")

local wav = {}
for i = 1, 200 do
  wav[i] = 0
end
-- custom_wave_set("CA", { 0 })
custom_wave_set("CA", wav)


-- Initial setup
wave_set(1, "SQUARE")
amp_set(1, 0.5)

wave_set(2, "SAW")
amp_set(2, 0.5)
pan_set(2, 1, 0.5)

wave_set(3, "SAW")
amp_set(3, 0.5)
pan_set(3, 0.5, 1)

local track_len = 4
local bpm = 60

local release_bass = false

local function bass(tt)
  if mfl.on_beat(tt, bpm, 1) then
    release_bass = false
    mfl.note_set(1, "E3")
  end
  if mfl.on_beat(tt, bpm, 2) then
    release_bass = true
  end
  if mfl.on_beat(tt, bpm, 3) then
    release_bass = false
    -- mfl.note_set(1, "C#6")
    mfl.note_random(1, { "C3", "D3", "E3", "F3", "G3", "A3", "B3" })
  end
  if mfl.on_beat(tt, bpm, 4) then
    release_bass = true
  end
  -- Handle attack and release
  if release_bass then
    mfl.fade(1, 0, 0.5)
  else
    mfl.fade(1, 0.5, 0.5)
  end
end

local release_lead = false

local function leadA(tt)
  if mfl.on_beat(tt, bpm, 1) then
    release_lead = false
    mfl.note_set(2, "C5")
  end
  if mfl.on_beat(tt, bpm, 1 + (1 / 16)) then
    mfl.note_set(2, "D5")
  end
  if mfl.on_beat(tt, bpm, 1 + (2 / 16)) then
    mfl.note_set(2, "G5")
  end
  if mfl.on_beat(tt, bpm, 2) then
    mfl.note_set(2, "D5")
  end
  if mfl.on_beat(tt, bpm, 3) then
    mfl.note_random(2, { "C4", "D4", "E4", "F4", "G4", "A4", "B4" })
  end
  if mfl.on_beat(tt, bpm, 3 + (1 / 2)) then
    release_lead = true
  end
  -- Handle attack and release
  if release_lead then
    mfl.fade(2, 0, 0.2)
  else
    mfl.fade(2, 0.5, 0.5)
  end
end

local function leadB(tt)
  -- Handle attack and release
  if release_lead then
    mfl.fade(2, 0, 0.2)
  else
    mfl.fade(2, 0.5, 0.5)
  end
end

local function leadC(tt)
  if mfl.on_beat(tt, bpm, 1) then
    release_lead = false
    mfl.note_set(2, "C5")
  end
  if mfl.on_beat(tt, bpm, 1 + (1 / 4)) then
    mfl.note_set(2, "D5")
  end
  if mfl.on_beat(tt, bpm, 1 + (2 / 4)) then
    mfl.note_set(2, "G5")
  end
  if mfl.on_beat(tt, bpm, 2) then
    mfl.note_set(2, "D5")
  end
  if mfl.on_beat(tt, bpm, 3) then
    mfl.note_random(2, { "C4", "D4", "E4", "F4", "G4", "A4", "B4" })
  end
  if mfl.on_beat(tt, bpm, 3 + (1 / 2)) then
    release_lead = true
  end
  -- Handle attack and release
  if release_lead then
    mfl.fade(2, 0, 0.2)
  else
    mfl.fade(2, 0.5, 0.5)
  end
end

local function lead2A(tt)
  -- Handle attack and release
  if release_lead then
    mfl.fade(3, 0, 0.2)
  else
    mfl.fade(3, 0.5, 0.5)
  end
end

local function lead2C(tt)
  if mfl.on_beat(tt, bpm, 1) then
    mfl.note_set(3, "C5")
  end
  if mfl.on_beat(tt, bpm, 1 + (1 / 16)) then
    mfl.note_set(3, "D5")
  end
  if mfl.on_beat(tt, bpm, 1 + (2 / 16)) then
    mfl.note_set(3, "G5")
  end
  if mfl.on_beat(tt, bpm, 2) then
    mfl.note_set(3, "D5")
  end
  if mfl.on_beat(tt, bpm, 3) then
    mfl.note_random(3, { "C4", "D4", "E4", "F4", "G4", "A4", "B4" })
  end
  -- Handle attack and release
  if release_lead then
    mfl.fade(3, 0, 0.2)
  else
    mfl.fade(3, 0.5, 0.5)
  end
end

-- Main loop
local section = 0
function Loop(tick)
  local tt = mfl.track_tick(tick, bpm, track_len)
  local ttl = mfl.track_tick(tick, bpm, track_len * 2)
  if ttl == 0 then
    section = section + 1
    section = section > 3 and 1 or section
    print("Section: " .. section)
  end
  bass(tt)
  if section == 1 then
    leadA(tt)
    lead2A(tt)
  elseif section == 2 then
    leadB(tt)
    lead2A(tt)
  elseif section == 3 then
    leadC(tt)
    lead2C(tt)
  end
end
