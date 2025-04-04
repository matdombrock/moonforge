enable(2)
enable(3)

env(1, 0, 16, 1)
env(1, 0, 16, 2)
env(1, 0, 16, 3)

wave(3, 4)

local notes = { 84, 86, 88, 89, 91 }
local function rand_note()
  local note = notes[math.random(1, #notes)]
  while note == mem_get(1) do
    note = notes[math.random(1, #notes)]
  end
  mem_set(note, 1)
  return note
end

if tick % 128 == 0 then
  note(rand_note() - 12, 1)
end
if tick % 64 == 0 then
  note(rand_note(), 2)
  note(rand_note(), 3)
end
if tick % 96 == 0 then
  note(rand_note(), 3)
end


