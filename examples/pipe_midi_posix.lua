-- This allows for non-blocking reading of MIDI events from a named pipe
-- and controlling sound parameters based on those events.
-- It uses the 'posix' library to handle non-blocking I/O.
-- Install with `luarocks install luaposix` if you don't have it.
-- Run `lua midi.lua` in another terminal to send MIDI events to the pipe.
local mfl = require "util.mflib"
local posix = require "posix"

local O_RDONLY = posix.fcntl.O_RDONLY
local O_NONBLOCK = posix.fcntl.O_NONBLOCK

local mfpipe = posix.open("../mfpipe", O_RDONLY + O_NONBLOCK)
local pipe_buffer = ""

wave_set(1, "SAW")
wave_set(2, "SQUARE")

-- State variables
local freq = 440
local freq_target = 440
local note = 60
local release = true
local velocity = 0
local slew_rate = 0.07
local max_slew_rate = 0.07
local lp_floor = 200
local lp_ceil = 12000
local lpc = lp_ceil
local detune = 0
local delay_time = 1

-- Deserialize a Lua table from a string
local function deserialize(str)
  local func = load('return ' .. str)
  if not func then
    error 'Invalid serialized string'
  end
  return func()
end

-- Print MIDI event details
local msg_count = 0
local function print_line(line, data)
  print()
  print(line)
  print("source", data.source)
  print("event", data.event)
  print("ch", data.ch)
  for k, v in pairs(data.data) do
    print("data ", k, v)
  end
  msg_count = msg_count + 1
  print("msg count:", msg_count)
end

-- Read lines from the pipe without blocking
local function read_pipe_line()
  if not mfpipe then return {} end
  local chunk = posix.read(mfpipe, 4096)
  if chunk then
    pipe_buffer = pipe_buffer .. chunk
  end
  local lines = {}
  while true do
    local s, e = pipe_buffer:find("([^\n]*)\n")
    if not s then break end
    local line = pipe_buffer:sub(s, e - 1)
    table.insert(lines, line)
    pipe_buffer = pipe_buffer:sub(e + 1)
  end
  return lines
end

-- Normalize MIDI CC value (0-127) to 0-1
local function cc_normalize(value)
  return value / 127
end

function Loop(tick)
  if mfpipe then
    while true do
      local lines = read_pipe_line()
      if #lines == 0 then break end
      for _, line in ipairs(lines) do
        local midi = deserialize(line)
        print_line(line, midi)
        if midi.event == "Note on" then
          release = false
          note = midi.data.note or 60
          freq_target = mfl.frequency.from_midi(note)
          velocity = cc_normalize(midi.data.velocity or 100) * 0.5
        end
        if midi.event == "Note off" then
          local off_note = midi.data.note or 60
          if off_note == note then
            release = true
          end
        end
        if midi.event == "Control change" then
          if midi.data.controller == 70 then
            lpc = cc_normalize(midi.data.value) * (lp_ceil - lp_floor) + lp_floor
            print("lpc", lpc)
          end
          if midi.data.controller == 71 then
            slew_rate = cc_normalize(midi.data.value) * max_slew_rate
            print("slew_rate", slew_rate)
          end
          if midi.data.controller == 72 then
            detune = cc_normalize(midi.data.value) * 220
            print("detune", detune)
          end
          if midi.data.controller == 73 then
            delay_time = cc_normalize(midi.data.value) * 1000
            print("delay_time", delay_time)
          end
        end
        if midi.event == "Pitch bend" then
          print("pitch bend", midi.data.value)
          local bend = (midi.data.value - 8192) / 8192
          bend = bend * 0.5
          bend = bend + 1.5
          print("bend", bend)
          freq_target = mfl.frequency.from_midi(note) * bend
        end
      end
    end
  end
  -- Slew frequency towards target
  freq = mfl.util.eerp(freq, freq_target, slew_rate)
  freq_set(1, freq)
  freq_set(2, (freq * 0.5) - detune)
  -- Handle amplitude envelope
  if release then
    local amp = amp_get(1)
    amp = mfl.util.eerp(amp, 0, 0.01)
    amp_set(1, amp)
    amp_set(2, amp)
  else
    local amp = amp_get(1)
    amp = mfl.util.eerp(amp, velocity, 0.01)
    amp_set(1, amp)
    amp_set(2, amp)
  end
  -- Update effects
  effect_set(0, "LOWPASS", lpc)
  effect_set(1, "DELAY", delay_time, 0.4, 0.5)
end
