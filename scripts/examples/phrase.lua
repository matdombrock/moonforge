local phrases = {
  {"C4", "E4", "G4", "F4"},
  {"E4", "G4", "B4", "A4"},
  {"G4", "B4", "D5", "C5"},
  {"B4", "D5", "F5", "E5"},
}

local function random_phrase()
  local phrase = math.random(1, #phrases)
  while phrase == mem_get(1) do
    phrase = math.random(1, #phrases)
  end
  mem_set(phrase, 1)
end

if mem_get(1) == -1 then
  random_phrase()
end

local phrase = phrases[mem_get(1)]

local tt = tick % 256

if tt == 0 then
  note(phrase[1], 1)
end
if tt == 64 then
  note(phrase[2], 1)
end
if tt == 128 then
  note(phrase[3], 1)
end
if tt == 128 + 64 then
  note(phrase[4], 1)
end

if tick % 512 == 0 then
  random_phrase()
end
