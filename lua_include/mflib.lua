-- A collection of utility functions for music and sound processing

local mfl = {}

function mfl.test()
  print("mflib test")
end

function mfl.randomseed(seed)
  -- Set the random seed for reproducibility
  seed = seed or os.time()
  math.randomseed(seed)
  print("Random seed set to: " .. seed)
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

-- Returns the current second based on the tick and BPM
function mfl.seconds_to_ticks(seconds)
  -- Convert seconds to ticks
  return math.floor(seconds * (1000 / TICK_WAIT))
end

-- Returns true if the tick is on the given second
function mfl.on_second(tick, second)
  local ticks_per_second = mfl.seconds_to_ticks(1)
  -- Check if the tick is on the specified second
  return tick % ticks_per_second == 0 and math.floor(tick / ticks_per_second) == second
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

-- Calculate the step size for fading an oscillator
function mfl.fade_step_calc(target_amp, current_amp, num_ticks)
  -- Calculate the step size for fading
  if num_ticks <= 0 then
    return 0
  end
  return (target_amp - current_amp) / num_ticks
end

-- Fade the bus amplitude towards the target amplitude by a step size
function mfl.bus_fade_step(target_amp, step)
  local current_amp = bus_amp_get()
  local diff = math.abs(target_amp - current_amp)
  if diff >= math.abs(step) then
    bus_amp_set(current_amp + step)
  else
    bus_amp_set(target_amp)
  end
end

-- Fade an oscillator's amplitude by a step size
function mfl.fade_step(osc_num, target_amp, step)
  -- Fade an oscillator's amplitude by a step size
  local current_amp = amp_get(osc_num)
  local diff = math.abs(target_amp - current_amp)
  if diff >= math.abs(step) then
    amp_set(osc_num, current_amp + step)
  else
    amp_set(osc_num, target_amp)
  end
end

-- Exponetially fade an oscillator towards the target amplitude
function mfl.fade_exp(osc_num, target_amp, speed)
  local current_amp = amp_get(osc_num)
  local fade_factor = 0.01 * speed -- Adjust this for fade speed
  if math.abs(current_amp - target_amp) < 0.0001 then
    amp_set(osc_num, target_amp)
    return
  end
  -- Exponential fade for smoother transition
  local new_amp = current_amp + (target_amp - current_amp) * fade_factor
  amp_set(osc_num, new_amp)
end

-- Supply a wave function where x is in the range [0, 2π]
function mfl.wavetable_make(wave_name, wave_fn)
  local wav = {}
  for i = 1, TABLE_SIZE do
    local x = (i - 1) / TABLE_SIZE * math.pi * 2
    wav[i] = wave_fn(x)
  end
  wavetable_write(wave_name, wav)
end

-- Generate a noise sample
function mfl.noise(amp)
  local n = math.random() * 2 - 1 -- Generate a random float in the range [-1, 1]
  return n * amp
end

-- Set a relative frequency change for an oscillator
function mfl.freq_change(osc_num, change)
  local current_freq = freq_get(osc_num)
  local new_freq = current_freq + change
  freq_set(osc_num, new_freq)
end

-- Set a relative amplitude change for an oscillator
function mfl.amp_change(osc_num, change)
  local current_amp = amp_get(osc_num)
  local new_amp = current_amp + change
  amp_set(osc_num, new_amp)
end

-- Generate a random float between min and max
function mfl.randomF(min, max)
  -- Generate a random float between min and max
  return math.random() * (max - min) + min
end

-- Slowly modulate an oscillator's frequency over time
function mfl.warble(osc_num, tick, time, amp)
  local mod = math.sin(tick / time)
  mfl.freq_change(osc_num, (mod / time) * amp)
end

return mfl
