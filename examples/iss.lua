-- ISS position sonification
-- Fetches ISS position from http://api.open-notify.org/iss-now.json
-- Maps latitude to freq of osc1, longitude to freq of osc2
-- The difference between lat and lon controls the panning and freq of osc3
-- The closer lat and lon are, the lower the freq of osc3 and the more centered
-- They are, the further apart lon and lat are, the higher the freq of osc3
-- and the more panned left/right it is
-- Requires internet access and curl installed
-- Requires util.json module for JSON parsing
-- NOTE: This is a super hacky way of getting around the lack of async HTTP requests
-- I dont want to integrate a full HTTP library into this because its too heavy
-- A real end user could use a lua library like lua-http or lua-curl

local json = require("util.json")

local FREQ_RANGE = 666
local LERP_SPEED = 0.0015

local LPC = 1300
local WAVE = "SQUARE"

amp_set(1, 0.5)
pan_set(1, 1, 0)
wave_set(1, WAVE)
lowpass_set(1, LPC)

amp_set(2, 0.5)
pan_set(2, 0, 1)
wave_set(2, WAVE)
lowpass_set(2, LPC)

amp_set(3, 0.75)
wave_set(3, "SINE")
lowpass_set(3, LPC)

bus_lowpass_set(LPC / 3)
bus_amp_set(1.5)

local last_ts = 0

local function curl_json(url, out_path)
  print(">>>>>>> curl start")
  local handle = io.popen('curl -s "' .. url .. '" > ' .. out_path .. ' &')
  local result = handle:read("*a")
  handle:close()
  return result
end

local function grab(file_path)
  local file = io.open(file_path, "r") -- Open file for reading
  if file then
    local content = file:read("*a")    -- Read entire file
    file:close()
    if #content == 0 then
      print("Waiting for data...")
      return nil
    end
    local dec = json.decode(content)
    if dec.message and dec.message == "success" then
      if dec.timestamp == last_ts then
        return nil
      end
      last_ts = dec.timestamp
      print(dec.message)
      print(dec.iss_position.latitude)
      print(dec.iss_position.longitude)
      print(dec.timestamp)
      return dec
    end
  else
    print("Failed to open file.")
  end
  return nil
end

-- Make an empty file at startup
local f = io.open("iss.json", "w")
f:write("{}")
f:close()

local fa = 440
local fb = 440
local fal = fa
local fbl = fb
local diff = 0.5 -- Init to a reasonable value
local diff_live = diff

function Loop(tick)
  -- Interpolate fa
  if fa ~= fal then
    fal = fal + (fa - fal) * LERP_SPEED
    freq_set(1, fal)
  end
  -- Interpolate fb
  if fb ~= fbl then
    fbl = fbl + (fb - fbl) * LERP_SPEED
    freq_set(2, fbl)
  end
  -- Interpolate diff
  if diff ~= diff_live then
    diff = diff + (diff_live - diff) * LERP_SPEED
  end

  local diff_r = diff * FREQ_RANGE / 2 + 200
  local sin = math.sin(tick / (diff_r + 1)) / 4
  freq_set(3, sin)
  local pan = (math.sin(tick / (diff_r + 1)) + 1) / 2
  pan_set(3, 1 - pan, pan)
  local pan = pan_get(3)
  print("Pan3: " .. pan[1] .. ", " .. pan[2])

  if tick % 1000 == 0 then
    print("Tick: " .. tick)
    local data = grab("iss.json")
    if data then
      local lat = tonumber(data.iss_position.latitude)
      local lon = tonumber(data.iss_position.longitude)
      -- Normalize lat/lon to 0-1
      lat = (lat + 90) / 180
      lon = (lon + 180) / 360
      print("Normalized: " .. lat .. ", " .. lon)
      diff_live = math.abs(lat - lon)
      print("Diff: " .. diff)
      if lat and lon then
        fa = lat * FREQ_RANGE + 20
        fb = lon * FREQ_RANGE + 20
      end
    end
  end
  if tick % 5000 == 0 then
    curl_json("http://api.open-notify.org/iss-now.json", "iss.json")
  end
end
