local json = require("util.json")
local mfl = require("util.mflib")

local FREQ_RANGE = 800
local LERP_SPEED = 0.0015

amp_set(1, 0.5)
pan_set(1, 1, 0)

amp_set(2, 0.5)
pan_set(2, 0, 1)

amp_set(3, 0.5)

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
      print("No content")
      return
    end
    local dec = json.decode(content)
    if dec.message and dec.message == "success" then
      print(dec.message)
      print(dec.iss_position.latitude)
      print(dec.iss_position.longitude)
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
local diff = 0
local diff_live = 0

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
  local diff_r = diff * FREQ_RANGE
  local sin = math.sin(tick / (diff_r + 1)) / 4
  local pan = (math.sin(tick / (diff_r + 1)) + 1) / 2
  pan_set(3, 1 - pan, pan)
  freq_set(3, sin)
  if tick % 1000 == 0 then
    print("Tick: " .. tick)
    print("grabbing")
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
    print("Tick: " .. tick)
    curl_json("http://api.open-notify.org/iss-now.json", "iss.json")
  end
end
