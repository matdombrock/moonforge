local pipe = io.open("../mfpipe", "r")

function Loop(tick)
  print("tick", tick)
  if pipe then
    local line = pipe:read("*l")
    if line then
      print(line)
    end
  end
end
