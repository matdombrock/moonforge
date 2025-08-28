local pipe = io.open("../mfpipe", "r")

amp_set(1, 0.9)

local freq = 440

function Loop(tick)
  print("tick", tick)
  if pipe then
    local line = pipe:read("*l")
    if line then
      print(line)
      local ft = tonumber(line * 800)
      print("ft", ft or "oh no")
      freq = ft or freq
    end
  end
  freq_set(1, freq)
end
