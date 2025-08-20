local mfl = require("util.mflib")

amp_set(1, 0.5)
wave_set(1, "SQUARE")
delay_set(1, 84000, 0.1, 0.2)
lowpass_set(1, 8000)

amp_set(2, 0.5)
wave_set(2, "SAW")
delay_set(2, 420000, 0.4, 0.2)
lowpass_set(2, 8000)

local bpm = 80
local section_len = 4
local track_sections = 8

local release = {
  bass = false,
  lead = false,
}

local section = 0

local function bass1(tt)
  if mfl.on_beat(tt, bpm, 1) then
    release.bass = false
    mfl.note_set(1, "E3")
  end
  if mfl.on_beat(tt, bpm, 2) then
    mfl.note_set(1, "F3")
  end
  if mfl.on_beat(tt, bpm, 4) then
    mfl.note_set(1, "G3")
    release.bass = true
  end
  -- LP LFO
  local lfo = math.sin(tt / 10) * 0.5 + 0.5
  lowpass_set(1, 8000 - (lfo * 8000))
end

local function bass2(tt)
  if mfl.on_beat(tt, bpm, 1) then
    release.bass = false
    mfl.note_set(1, "E3")
  end
  if mfl.on_beat(tt, bpm, 2) then
    mfl.note_set(1, "F3")
  end
  if mfl.on_beat(tt, bpm, 4) then
    mfl.note_set(1, "G3")
    release.bass = true
  end
  -- LP LFO
  local lfo = math.sin(tt / 100) * 0.5 + 0.5
  lowpass_set(1, 8000 - (lfo * 8000))
end

local function lead1(tt)
  if mfl.on_beat(tt, bpm, 1) then
    release.lead = false
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
    release.lead = true
  end
end

local function lead2(tt)
  if mfl.on_beat(tt, bpm, 1) then
    release.lead = false
    mfl.note_set(2, "C5")
  end
  if mfl.on_beat(tt, bpm, 1 + (1 / 16)) then
    mfl.note_set(2, "D5")
  end
  if mfl.on_beat(tt, bpm, 1 + (2 / 16)) then
    mfl.note_set(2, "G5")
  end
  if mfl.on_beat(tt, bpm, 2 + (1 / 16)) then
    mfl.note_set(2, "D6")
  end
  if mfl.on_beat(tt, bpm, 2 + (2 / 16)) then
    mfl.note_set(2, "G6")
  end
  if mfl.on_beat(tt, bpm, 2 + (4 / 16)) then
    mfl.note_set(2, "F6")
  end
  if mfl.on_beat(tt, bpm, 3) then
    mfl.note_set(2, "D5")
  end
  if mfl.on_beat(tt, bpm, 4) then
    mfl.note_random(2, { "C4", "D4", "E4", "F4", "G4", "A4", "B4" })
    release.lead = true
  end
  if mfl.on_beat(tt, bpm, 4 + (1 / 16)) then
    mfl.note_random(2, { "C4", "D4", "E4", "F4", "G4", "A4", "B4" })
  end
  if mfl.on_beat(tt, bpm, 4 + (3 / 16)) then
    mfl.note_random(2, { "C4", "D4", "E4", "F4", "G4", "A4", "B4" })
  end
end

local function env()
  -- Handle attack and release
  if release.bass then
    mfl.fade(1, 0, 0.5)
  else
    mfl.fade(1, 0.5, 0.5)
  end
  if release.lead then
    mfl.fade(2, 0, 0.5)
  else
    mfl.fade(2, 0.5, 100)
  end
end

function Loop(tick)
  local tt = mfl.track_tick(tick, bpm, section_len)
  if tt == 0 then
    section = section + 1
    if section >= track_sections then
      section = 0
    end
  end
  if section < 4 then
    bass1(tt)
    lead1(tt)
  elseif section < 8 then
    bass2(tt)
    lead2(tt)
  end
  env()
end
