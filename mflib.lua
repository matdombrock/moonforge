local mfl = {}

function mfl.test()
  print("mflib test")
end

-- Returns a tick modulo relative to the track length
function mfl.track_tic(tick, bpm, track_len)
  local tt = tick % beat_to_ticks(bpm, track_len + 1)
  return tt
end

function mfl.on_beat(tick, bpm, beat)
  if beat_to_ticks(bpm, beat) == tick then
    return true
  end
  return false
end

function mfl.midi_to_freq(note)
  -- Convert MIDI note number to frequency
  return 440 * 2 ^ ((note - 69) / 12)
end

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

return mfl
