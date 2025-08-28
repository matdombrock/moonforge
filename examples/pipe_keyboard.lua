-- Get some keyboard input from a named pipe and play notes accordingly.
-- Create the named pipe with `mkfifo ../mfpipe` in the terminal.
-- Run the ./pipe_keyboard.sh script to send keyboard input to the pipe.
local mfl = require("util.mflib")
local pipe = io.open("../mfpipe", "r")

amp_set(1, 0.6)
wave_set(1, "SAW")
amp_set(2, 0.6)
wave_set(2, "SQUARE")
effect_set(0, "LOWPASS", 800)

local freq = 440

local keymap = {
  a = 0,
  w = 1,
  s = 2,
  e = 3,
  d = 4,
  f = 5,
  t = 6,
  g = 7,
  y = 8,
  h = 9,
  u = 10,
  j = 11,
  k = 12,
  l = 14,
}

local release = false

local sustain_counter = 0

function Loop(tick)
  print("tick", tick)
  if pipe then
    local line = pipe:read("*l")
    if line then
      release = false
      sustain_counter = 0
      print(line)
      local ft = 0
      local note = keymap[line]
      if note then
        ft = mfl.frequency.from_midi(64 + note)
      end
      print("ft", ft or "oh no")
      freq = ft or freq
      freq_set(1, freq)
      freq_set(2, freq * 0.5)
    else
      sustain_counter = sustain_counter + 1
      if sustain_counter > 150 then
        release = true
        sustain_counter = 0
      end
    end
  end
  if release then
    local amp = amp_get(1)
    amp = mfl.util.eerp(amp, 0, 0.01)
    amp_set(1, amp)
    amp_set(2, amp)
  else
    local amp = amp_get(1)
    amp = mfl.util.eerp(amp, 0.9, 0.01)
    amp_set(1, amp)
    amp_set(2, amp)
  end
end
