local json = require("util.json")
local mfl = require("util.mflib")

amp_set(1, 0.5)
amp_set(2, 0.5)

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

function Loop(tick)
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
      if lat and lon then
        freq_set(1, lat * 1200 + 20)
        freq_set(2, lon * 1200 + 20)
      end
    end
  end
  if tick % 5000 == 0 then
    print("Tick: " .. tick)
    curl_json("http://api.open-notify.org/iss-now.json", "iss.json")
  end
end
