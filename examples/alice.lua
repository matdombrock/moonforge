local mfl = require("util.mflib")

wave_set(1, "SQUARE")
lowpass_set(1, 4000)

amp_set(2, 0)
wave_set(2, "SAW")

amp_set(3, 0)
wave_set(3, "SAW")

amp_set(4, 0.8)
wave_set(4, "SINE")

bus_delay_set_r(32, 0.9, 0.2)
bus_delay_set_l(64, 0.9, 0.2)

local bpm = 240

local section = 1
local section_count = -1
local section_length = 3

local function arp(notes, tick, interval, bmp)
  local step = tick / mfl.beat_to_ticks(bmp, 1 + interval)
  step = math.floor(step)
  local index = ((step - 1) % #notes) + 1
  return notes[index]
end



function Loop(tick)
  local tt = mfl.track_tick(tick, bpm, 4)
  if tt == 0 then
    section_count = section_count + 1
    print("Tick: " .. tick .. " TT: " .. tt .. " Section Count: " .. section_count)
  end
  if section_count == section_length then
    section = (section) % 4 + 1
    section_count = 0
    print("Section: " .. section)
  end
  if section == 1 then
    section_length = 3
    local arp_notes = { "C4", "E4", "G4", "B4" }
    local note = arp(arp_notes, tick, 0.25, bpm)
    mfl.note_set(4, note)
    if mfl.on_beat(tt, bpm, 1) then
      amp_set(1, 0.5)
      mfl.note_set(1, "G2")
    end
    if mfl.on_beat(tt, bpm, 2) then
      amp_set(1, 0)
    end
  end

  if section == 2 then
    section_length = 3
    local arp_notes = { "A3", "C4", "E4", "G4" }
    local note = arp(arp_notes, tick, 0.25, bpm)
    mfl.note_set(4, note)
    if mfl.on_beat(tt, bpm, 1) then
      amp_set(1, 0.5)
      mfl.note_set(1, "F2")
    end
    if mfl.on_beat(tt, bpm, 2) then
      amp_set(1, 0)
    end
  end

  if section == 3 then
    section_length = 2
    local arp_notes = { "F3", "A3", "C4", "E4" }
    local note = arp(arp_notes, tick, 0.25, bpm)
    mfl.note_set(4, note)
    if mfl.on_beat(tt, bpm, 1) then
      amp_set(1, 0.5)
      amp_set(2, 0.3)
      amp_set(3, 0.3)
      mfl.note_set(1, "C3")
      mfl.note_set(2, "C4")
      mfl.note_set(3, "E4")
    end
    if mfl.on_beat(tt, bpm, 2) then
      amp_set(1, 0)
      amp_set(2, 0)
      amp_set(3, 0)
    end
  end

  if section == 4 then
    section_length = 2
    local arp_notes = { "F3", "A3", "C4", "E4" }
    local note = arp(arp_notes, tick, 0.25, bpm)
    mfl.note_set(4, note)
    if mfl.on_beat(tt, bpm, 1) then
      amp_set(1, 0.5)
      amp_set(2, 0.3)
      amp_set(3, 0.3)
      mfl.note_set(1, "D3")
      mfl.note_set(2, "C4")
      mfl.note_set(3, "E4")
    end
    if mfl.on_beat(tt, bpm, 2) then
      amp_set(1, 0)
    end
    if mfl.on_beat(tt, bpm, 3) then
      print("3")
      amp_set(2, 0)
      amp_set(3, 0)
    end
  end
end
