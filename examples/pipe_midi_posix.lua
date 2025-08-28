-- This allows for non-blocking reading of MIDI events from a named pipe
-- and controlling sound parameters based on those events.
-- It uses the 'posix' library to handle non-blocking I/O.
-- Install with `luarocks install luaposix` if you don't have it.
-- Run `lua midi.lua` in another terminal to send MIDI events to the pipe.
local mfl = require "util.mflib"
local posix = require "posix"

local O_RDONLY = posix.fcntl.O_RDONLY
local O_NONBLOCK = posix.fcntl.O_NONBLOCK

local fd = posix.open("../mfpipe", O_RDONLY + O_NONBLOCK)
local pipe_buffer = ""

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

local function read_pipe_line()
  if not fd then return nil end
  local chunk = posix.read(fd, 4096)
  if chunk then
    pipe_buffer = pipe_buffer .. chunk
    local s, e = pipe_buffer:find("([^\n]*)\n")
    if s then
      local line = pipe_buffer:sub(s, e - 1)
      pipe_buffer = pipe_buffer:sub(e + 1)
      return line
    end
  end
  return nil
end

function Loop(tick)
  -- print("tick", tick)
  if fd then
    local line = read_pipe_line()
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
    -- print("release", release)
    if release then
      -- print("releasing")
      local amp = amp_get(1)
      amp = mfl.util.eerp(amp, 0, 0.01)
      amp_set(1, amp)
    else
      -- print("not releasing")
      local amp = amp_get(1)
      amp = mfl.util.eerp(amp, 0.8, 0.01)
      amp_set(1, amp)
    end
  end
end
