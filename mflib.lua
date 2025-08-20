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

return mfl
