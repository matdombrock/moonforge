-- Read MIDI events from a specified device and write them to a named pipe.
-- To list devices use `aseqdump -l`

local pipe = "mfpipe"
local midi_dev = "36:0" -- Change this to your MIDI device ID

-- Parse the line into a lua table
-- Example line:
-- Source  Event                  Ch  Data
-- 36:0   Control change          0, controller 74, value 12
local function parse_line(line)
  -- Pattern to match: source, event, channel, and data part (allowing leading whitespace)
  local source, event, ch, data_str = line:match("^%s*([^%s]+)%s+([^%d]+)%s+(%d+),%s*(.+)$")

  if not source or not event or not ch or not data_str then
    print("Failed to parse line: " .. line)
    return nil -- Invalid format
  end

  -- Trim whitespace from event name
  event = event:match("^%s*(.-)%s*$")

  -- Parse the data part (note X, velocity Y)
  local data = {}
  for key, value in data_str:gmatch("(%w+)%s+(-?%d+)") do
    data[key] = tonumber(value)
  end

  return {
    source = source,
    event = event,
    ch = tonumber(ch),
    data = data
  }
end

local function serialize(tbl)
  local function serializeValue(value)
    if type(value) == 'table' then
      local result = '{'
      for k, v in pairs(value) do
        result = result .. '[' .. serializeValue(k) .. '] = ' .. serializeValue(v) .. ', '
      end
      return result .. '}'
    elseif type(value) == 'string' then
      return '"' .. value:gsub('"', '\\"') .. '"'
    else
      return tostring(value)
    end
  end
  return serializeValue(tbl)
end

-- Open the pipe for writing MIDI events to
local midi_pipe = io.open(pipe, "w")
local handle = io.popen("aseqdump -p '" .. midi_dev .. "'")
if not handle then
  print("Failed to open MIDI device")
  return
end

local msg_count = 0

for line in handle:lines() do
  print("\n>> " .. line) -- Process MIDI input here
  local parsed = parse_line(line)
  if parsed then
    print("source", parsed.source)
    print("event", parsed.event)
    print("ch", parsed.ch)
    for k, v in pairs(parsed.data) do
      print("data ", k, v)
    end
    if handle and midi_pipe then
      midi_pipe:write(serialize(parsed) .. "\n")
      -- midi_pipe:write(serialize(parsed))
      midi_pipe:flush()
    else
      print("MIDI pipe not available")
    end
    msg_count = msg_count + 1
    print("msg count: ", msg_count)
  end
  print("<<")
end

print("MIDI input loop ended")

if handle then
  handle:close()
end
