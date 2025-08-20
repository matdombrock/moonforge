-- A collection of utility functions for music and sound processing

local mfl = {}

function mfl.test()
  print("mflib test")
end

function mfl.beat_to_ticks(bpm, beat)
  beat = beat - 1
  local ticks_per_second = 1000 / TICK_WAIT
  local seconds_per_beat = 60 / bpm
  return math.floor(ticks_per_second * seconds_per_beat * beat)
end

-- Returns a tick modulo relative to the track length
function mfl.track_tick(tick, bpm, track_len)
  local tt = tick % mfl.beat_to_ticks(bpm, track_len + 1)
  return tt
end

-- Returns true if the tick is on the given beat
function mfl.on_beat(tick, bpm, beat)
  if mfl.beat_to_ticks(bpm, beat) == tick then
    return true
  end
  return false
end

-- MIDI note to frequency conversion
function mfl.midi_to_freq(note)
  -- Convert MIDI note number to frequency
  return 440 * 2 ^ ((note - 69) / 12)
end

-- Music note to frequency conversion
function mfl.note_to_freq(note)
  -- Convert note name (with optional #/b) to frequency
  local base_freqs = {
    C = 261.63,
    ["C#"] = 277.18,
    ["Db"] = 277.18,
    D = 293.66,
    ["D#"] = 311.13,
    ["Eb"] = 311.13,
    E = 329.63,
    F = 349.23,
    ["F#"] = 369.99,
    ["Gb"] = 369.99,
    G = 392.00,
    ["G#"] = 415.30,
    ["Ab"] = 415.30,
    A = 440.00,
    ["A#"] = 466.16,
    ["Bb"] = 466.16,
    B = 493.88
  }

  local base_note = note:match("^[A-G][#b]?")
  local octave = tonumber(note:match("%d+$"))

  if base_note and octave and base_freqs[base_note] then
    return base_freqs[base_note] * (2 ^ (octave - 4))
  else
    return nil -- Invalid note
  end
end

-- Set the frequency of an oscillator based on a note name
function mfl.note_set(osc_num, note)
  -- Set the frequency of an oscillator based on a note name
  local freq = mfl.note_to_freq(note)
  if freq then
    freq_set(osc_num, freq)
  else
    print("Invalid note: " .. note)
  end
end

-- Set a random note from the provided list
function mfl.note_random(osc_num, notes)
  -- Set the frequency of an oscillator to a random note from a list
  if #notes > 0 then
    local random_index = math.random(1, #notes)
    mfl.note_set(osc_num, notes[random_index])
  else
    print("No notes provided for random selection.")
  end
end

-- Fade an oscillator towards the target amplitude
function mfl.fade(osc_num, target_amp, speed)
  -- Fade an oscillator's amplitude to a target value
  local current_amp = amp_get(osc_num)
  if current_amp < target_amp then
    -- Fade in
    amp_set(osc_num, math.min(current_amp + speed / 1000, target_amp))
  else
    -- Fade out
    amp_set(osc_num, math.max(current_amp - speed / 1000, target_amp))
  end
end

-- Supply a wave function where x is in the range [0, 2π]
function mfl.make_wave(wave_name, wave_fn)
  local wav = {}
  for i = 1, TABLE_SIZE do
    local x = (i - 1) / TABLE_SIZE * math.pi * 2
    wav[i] = wave_fn(x)
  end
  custom_wave_set(wave_name, wav)
end

-- Generate a noise sample
function mfl.noise(amp)
  local n = math.random() * 2 - 1 -- Generate a random float in the range [-1, 1]
  return n * amp
end

function mfl.freq_change(osc_num, change)
  local current_freq = freq_get(osc_num)
  local new_freq = current_freq + change
  freq_set(osc_num, new_freq)
end

function mfl.amp_change(osc_num, change)
  local current_amp = amp_get(osc_num)
  local new_amp = current_amp + change
  amp_set(osc_num, new_amp)
end

return mfl
