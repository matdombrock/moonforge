local json = require("util.json")

local function curl_json(url)
  local handle = io.popen('curl -s "' .. url .. '"')
  local result = handle:read("*a")
  handle:close()
  return result
end

local function grab()
  local jdata = curl_json("http://api.open-notify.org/iss-now.json")
  local dec = json.decode(jdata)
  print(dec.message)
  print(dec.iss_position.latitude)
  print(dec.iss_position.longitude)
end

function Loop(tick)
  if tick % 5000 == 0 then
    print("Tick: " .. tick)
    grab()
  end
end
