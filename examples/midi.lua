local mfl = require "util.mflib"

local pipe = io.open("../mfpipe", "r")

amp_set(1, 0.9)

local freq = 440
local note = 60
local release = false

local function deserialize(str)
  local func = load('return ' .. str)
  if not func then
    error 'Invalid serialized string'
  end
  return func()
end

local function print_line(line, data)
  print(line)
  print("source", data.source)
  print("event", data.event)
  print("ch", data.ch)
  for k, v in pairs(data.data) do
    print("data ", k, v)
  end
end

function Loop(tick)
  print("tick", tick)
  if pipe then
    local line = pipe:read("*l")
    if line then
      local data = deserialize(line)
      print_line(line, data)
      if data.event == "Note on" then
        release = false
        note = data.data.note or 60
        freq = mfl.frequency.from_midi(note)
      end
      if data.event == "Note off" then
        print("note off", data.data.note)
        local off_note = data.data.note or 60
        if off_note == note then
          release = true
        end
      end
    end
    freq_set(1, freq)
    print("release", release)
    if release then
      print("releasing")
      local amp = amp_get(1)
      amp = mfl.util.eerp(amp, 0, 0.01)
      amp_set(1, amp)
    else
      print("not releasing")
      local amp = amp_get(1)
      amp = mfl.util.eerp(amp, 0.8, 0.01)
      amp_set(1, amp)
    end
  end
end
