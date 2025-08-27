local mfl = require("util.mflib")

wave_set(1, "SAW")
effect_set(1, "LOWPASS", 700)

wave_set(2, "SAW")
effect_set(2, "LOWPASS", 600)
wave_set(3, "SAW")
effect_set(3, "LOWPASS", 600)
effect_set(3, "DELAY", 500, 0.5, 0.25)

wave_set(4, "SINE")
pan_set(4, 1, 0.25)

wave_set(5, "SINE")
pan_set(5, 0.25, 1)

local section_len = 8
local bpm = 120
local release_speeds = {
  0.005,
  0.005,
  0.005,
  0.005,
  0.005,
}
local attack_speeds = {
  0.005,
  0.005,
  0.005,
  0.005,
  0.005,
}
local release = {
  false,
  false,
  false,
  false,
  false,
}
local mix = {
  0.8,
  0.7,
  0.7,
  0.8,
  0.8,
}
local section = 0
local it_count = 0

local function handle_release()
  for i = 1, #release do
    if release[i] then
      local amp = math.max(0, amp_get(i) - release_speeds[i])
      amp_set(i, amp)
    else
      local amp = math.min(mix[i], amp_get(i) + attack_speeds[i])
      amp_set(i, amp)
    end
  end
end

local function sequence(osc_num, notes, tick, interval, bmp)
  local step = tick / mfl.time.beat_to_ticks(bmp, 1 + interval)
  step = math.floor(step)
  local index = (step % #notes) + 1
  local note = notes[index]
  if note == "R" then
    release[osc_num] = true
  else
    release[osc_num] = false
    freq_set(osc_num, mfl.frequency.from_note(note))
  end
end

local function bass(tt)
  if section == 1 or section == 2 then
    local notes = { "A2", "R", "C2", "R", "G2", "R", "D2", "R" }
    sequence(1, notes, tt, 1, bpm)
  end
  local lfo = math.sin(tt / 250) * 0.5 + 0.5
  effect_set(1, "LOWPASS", (lfo * 10000) + 1200)
end

local function pads(tt)
  if section == 1 then
    release[4] = true
    release[5] = true
  end
  if section == 2 then
    local notes_a = { "C4", "E4", "G4", "B4" }
    local notes_b = { "E4", "G4", "B4", "D4" }
    sequence(4, notes_a, tt, 2, bpm)
    sequence(5, notes_b, tt, 2, bpm)
  end
  if section == 3 then
    local notes_a = { "F4", "A4", "C5", "E5" }
    local notes_b = { "A4", "C4", "E5", "G5" }
    sequence(4, notes_a, tt, 2, bpm)
    sequence(5, notes_b, tt, 2, bpm)
  end
  if section == 4 then
    local notes_a = { "E4", "G4", "B4", "D5" }
    local notes_b = { "C4", "E4", "G4", "B4" }
    sequence(4, notes_a, tt, 2, bpm)
    sequence(5, notes_b, tt, 2, bpm)
  end
  if release[4] == false and section == 3 then
    local s2 = math.sin(tt / 600) * 0.5 + 0.5
    local amp = math.sin(tt / (30 * s2)) * 0.5 + 0.5
    amp_set(4, amp * 0.8)
    amp_set(5, amp * 0.8)
  end
end


local function lead(tt)
  local det_f = 0.5
  if section == 1 then
    release_speeds[2] = 0.005
    release_speeds[3] = 0.005
    local notes = { "E5", "D5", "C5", "D5", "E5", "E5", "E5", "R",
      "D5", "D5", "D5", "R", "E5", "G5", "G5", "R" }
    sequence(2, notes, tt, 0.5, bpm)
    sequence(3, notes, tt, 0.5, bpm)
  end
  if section == 2 then
    local notes = {
      "E5", "D5", "C5", "D5", "E5", "E5", "E5", "R",
      "D5", "D5", "D5", "R", "E5", "G5", "G5", "R",
      "F5", "D5", "D5", "D5", "F5", "E5", "E5", "R",
      "D5", "D5", "D5", "R", "E5", "G5", "G5", "R",
    }
    sequence(2, notes, tt, 0.25, bpm)
    sequence(3, notes, tt, 0.25, bpm)
  end
  if section == 3 then
    local notes_b = { "A5", "C5", "E6", "E5" }
    sequence(2, notes_b, tt, 2, bpm)
    sequence(3, notes_b, tt, 2, bpm)
  end
  if section == 4 then
    release[2] = true
    release[3] = true
    release_speeds[2] = 0.00025
    release_speeds[3] = 0.00025
    det_f = 0.25
  end
  mfl.modulate.warble(2, tt, 1000, 1)
  local det = freq_get(3)
  det = det - det_f
  freq_set(3, det)
end

function Loop(tick)
  local tt = mfl.time.track_tick(tick, bpm, section_len)
  if tt == 0 then
    section = section + 1
    if section > 4 then
      section = 1
    end
    it_count = it_count + 1
    if it_count == 5 then
      exit()
    end
  end
  bass(tt)
  pads(tt)
  lead(tt)
  handle_release()
end
