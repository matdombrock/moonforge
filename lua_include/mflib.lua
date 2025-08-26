local mfl = {
  util = {},
  random = {},
  time = {},
  frequency = {},
  sequencer = {},
  wavetable = {},
  modulate = {}
}

--------------
--- Util
--------------

-- Linear interpolation
function mfl.util.lerp(from, to, time)
  return from + (to - from) * time
end

-- Exponential interpolation
function mfl.util.eerp(from, to, time)
  return from + (to - from) * (1 - math.exp(-time))
end

--------------
--- Random
--------------

-- Set the random seed for reproducibility
function mfl.random.seed(seed)
  seed = seed or os.time()
  math.randomseed(seed)
  print("Random seed set to: " .. seed)
end

-- Generate a noise sample
function mfl.random.noise(amp)
  local n = math.random() * 2 - 1 -- Generate a random float in the range [-1, 1]
  return n * amp
end

-- Generate a random float between min and max
function mfl.random.float(min, max)
  -- Generate a random float between min and max
  return math.random() * (max - min) + min
end

-- Return a random value from a list
function mfl.random.choice(list)
  if #list == 0 then return nil end
  local index = math.random(1, #list)
  return list[index]
end

--------------
--- Time
--------------

function mfl.time.beat_to_ticks(bpm, beat)
  beat = beat - 1
  local ticks_per_second = 1000 * TICK_WAIT
  local seconds_per_beat = 60 / bpm
  return math.floor(ticks_per_second * seconds_per_beat * beat)
end

-- Returns a tick modulo relative to the track length
function mfl.time.track_tick(tick, bpm, track_len)
  local tt = tick % mfl.time.beat_to_ticks(bpm, track_len + 1)
  return tt
end

-- Returns true if the tick is on the given beat
function mfl.time.on_beat(tick, bpm, beat)
  if mfl.time.beat_to_ticks(bpm, beat) == tick then
    return true
  end
  return false
end

-- Returns the current second based on the tick and BPM
function mfl.time.seconds_to_ticks(seconds)
  -- Convert seconds to ticks
  return math.floor(seconds * (1000 / TICK_WAIT))
end

-- Returns true if the tick is on the given second
function mfl.time.on_second(tick, second)
  local ticks_per_second = mfl.time.seconds_to_ticks(1)
  -- Check if the tick is on the specified second
  return tick % ticks_per_second == 0 and math.floor(tick / ticks_per_second) == second
end

--------------
--- Notes & Frequency
--------------

-- MIDI note to frequency conversion
function mfl.frequency.from_midi(note)
  -- Convert MIDI note number to frequency
  return 440 * 2 ^ ((note - 69) / 12)
end

-- Music note to frequency conversion
function mfl.frequency.from_note(note)
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
function mfl.frequency.note_set(osc_num, note)
  -- Set the frequency of an oscillator based on a note name
  local freq = mfl.frequency.from_note(note)
  if freq then
    freq_set(osc_num, freq)
  else
    print("Invalid note: " .. note)
  end
end

-- Detune an oscillator's frequency by a given frequency offset
function mfl.frequency.detune(osc_num, freq_offset)
  local current_freq = freq_get(osc_num)
  freq_set(osc_num, current_freq + freq_offset)
end

--------------
--- Sequencers
--------------

-- Arpeggiate through a list of notes based on the current tick and interval
-- Example: mfl.arp({"C4", "E4", "G4"}, tick, 0.25, bpm)
function mfl.sequencer.arp(notes, tick, interval, bmp)
  local step = tick / mfl.beat_to_ticks(bmp, 1 + interval)
  step = math.floor(step)
  local index = ((step - 1) % #notes) + 1
  return notes[index]
end

--------------
--- Wavetables
--------------

-- Supply a wave function where x is in the range [0, 2π]
function mfl.wavetable.make(wave_name, wave_fn)
  local wav = {}
  for i = 1, TABLE_SIZE do
    local x = (i - 1) / TABLE_SIZE * math.pi * 2
    wav[i] = wave_fn(x)
  end
  wavetable_write(wave_name, wav)
end

--------------
--- Modulation
--------------

-- Slowly modulate an oscillator's frequency over time
function mfl.warble(osc_num, tick, time, amp)
  local mod = math.sin(tick / time)
  mfl.freq_change(osc_num, (mod / time) * amp)
end

return mfl
