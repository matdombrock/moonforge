local mfl = require("util.mflib")

local bpm = 120
local section_len_beats = 4
local sections = {}

local cur_section = 0


local function sequence(osc_num, notes, tick, interval, bmp)
  local step = tick / mfl.beat_to_ticks(bmp, 1 + interval)
  step = math.floor(step)
  local index = (step % #notes) + 1
  local note = notes[index]
  mfl.note_set(osc_num, note)
end

amp_set(1, 0.5)
pan_set(1, 1, 0.25)
delay_set(1, mfl.beat_to_ticks(bpm, 1.5), 0.5, 0.2)

amp_set(2, 0.5)
pan_set(2, 0.25, 1)

amp_set(3, 0.2)
wave_set(3, "SQUARE")
lowpass_set(3, 1000)

sections[1] = function(st)
  amp_set(3, 0.2)
  delay_set(3, mfl.beat_to_ticks(bpm, 1.5), 0, 0)
  sequence(1, { "C4", "D4", "C4", "E4" }, st, 1, bpm)
  sequence(2, { "E4", "F4", "E4", "G4" }, st, 1, bpm)
  sequence(3, { "C2", "D2", "C2", "E2" }, st, 1, bpm)
end

sections[2] = function(st)
  sequence(1, { "C4", "E4", "G4", "B4" }, st, 0.5, bpm)
  sequence(2, { "E4", "G4", "B4", "D4" }, st, 1, bpm)
  sequence(3, { "C2", "D2" }, st, 2, bpm)
end

sections[3] = function(st)
  sequence(1, { "C4", "E4", "G5", "B4" }, st, 0.5, bpm)
  sequence(2, { "E4", "G4", "B4", "D4" }, st, 1, bpm)
  sequence(3, { "G3", "B2" }, st, 2, bpm)
end

sections[4] = function(st)
  sequence(1, { "E4", "G4", "B5", "D4" }, st, 0.5, bpm)
  sequence(2, { "E4", "G4", "B4", "D4" }, st, 1, bpm)
  sequence(3, { "E2", "G2", "B2", "D4" }, st, 1, bpm)
  if mfl.on_beat(st, bpm, 4) then
    delay_set(3, mfl.beat_to_ticks(bpm, 1.5), 0.5, 0.2)
  end
  if mfl.on_beat(st, bpm, 4.25) then
    mfl.note_set(3, "C1")
  end
end

function Loop(tick)
  -- Section tick
  local st = mfl.track_tick(tick, bpm, section_len_beats)
  if st == 0 then
    cur_section = cur_section % #sections
    cur_section = cur_section + 1
    print("Section: " .. cur_section)
  end
  sections[cur_section](st)
end
