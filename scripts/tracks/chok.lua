enable(2)
enable(3)

enable(4)
wave(2, 4)
amp(0.5, 4)
lowpass(1800, 1, 4)
env(16, 64, 16, 4)

enable(5)
wave(1, 5)
amp(0.5, 5)
env(16, 16, 16, 5)

enable(6)
wave(1, 6)
amp(0.5, 6)
detune(0.01, 6)

local tt = tick % (32 * 64) + 1

local function beat(n)
  return n == math.floor(tt / 64)
end

local function chr()
 return math.random() > 0.15
end

local function ch1()
  if chr() then return end
  note("C4", 1)
  note("E4", 2)
  note("G4", 3)
end
local function ch2()
  if chr() then return end
  note("E4", 1)
  note("G4", 2)
  note("B4", 3)
end
local function ch3()
  if chr() then return end
  note("G4", 1)
  note("B4", 2)
  note("D5", 3)
end
local function ch4()
  if chr() then return end
  note("B4", 1)
  note("D5", 2)
  note("F5", 3)
end
local function ch5()
  if chr() then return end
  note("C5", 1)
  note("E5", 2)
  note("G5", 3)
end
local function ch6()
  if chr() then return end
  note("D4", 1)
  note("F4", 2)
  note("B4", 3)
end

local function bs1()
  note("C3", 4)
end
local function bs2()
  note("E3", 4)
end

if beat(1) then
  ch1()
  bs1()
end
if beat(3) then
  ch2()
end
if beat(5) then
  ch3()
  bs2()
end
if beat(7) then
  ch4()
end
if beat(8) then
  ch5()
end

if beat(9) then
  ch1()
  bs1()
end
if beat(10) then
  ch5()
end
if beat(11) then
  ch2()
end
if beat(13) then
  ch3()
  bs2()
end
if beat(15) then
  ch4()
end
if beat(16) then
  ch5()
end

if beat(17) then
  ch1()
  bs1()
end
if beat(19) then
  ch2()
end
if beat(21) then
  ch3()
end
if beat(23) then
  ch4()
end
if beat(24) then
  if math.random() > 0.5 then ch5() end
  bs2()
end

if beat(25) then
  ch1()
end
if beat(26) then
  ch6()
end
if beat(27) then
  ch2()
end
if beat(29) then
  ch3()
end
if beat(31) then
  ch4()
end
if beat(32) then
  ch5()
end

local notes = { 84, 86, 88, 89, 91 }
local function rand_note()
  local note = notes[math.random(1, #notes)]
  while note == mem_get(1) do
    note = notes[math.random(1, #notes)]
  end
  mem_set(note, 1)
  return note
end

local lead_time = 64
if tt > (8 * 64) then
  lead_time = 32
end

if tt % lead_time == 1 then
  if math.random() > 0.5 then
    return
  end
  local n = rand_note()
  note(n, 5)
  note(n - 12, 6)
end

local lfo_amp = 0.5 * math.sin(tick / 2) + 0.5
amp(0.5 + lfo_amp * 0.5, 5)
