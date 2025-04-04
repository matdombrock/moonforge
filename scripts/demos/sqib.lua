enable(2)
enable(3)
local n = tick

local function rng()
  mem_set(math.random(1, 8), 1)
  mem_set(math.random(-1, 1), 2)
  mem_set(math.random(32, 256), 3)
end

if mem_get(1) == -1 then
  rng()
end


if (tick % 32) == 0 then
  rng()
end

if (tick % 256) > 248 then
  rng()
end


if mem_get(1) == -1 then
  mem_set(1, 1)
end
n = math.floor(n / (16 * mem_get(1)))
n = n % 12
n = n * 5
n = n + 50
n = math.floor(n)
note(n, 1)

local o = mem_get(2)
if o > 0 then o = 3 else o = -3 end
local lfo_x = 0.5 * math.sin(tick / mem_get(3)) + 0.5
if math.random() > lfo_x then n = n + o end
n = n - 12
note(n, 2)
local lfo_amp = 0.5 * math.sin(tick / (mem_get(3) * 3)) + 0.5
amp(0.5 + lfo_amp * 0.5, 1)
amp(lfo_amp, 2)

local lfo_freq = 0.5 * math.sin(tick / mem_get(3)) + 0.5
freq(220 + lfo_freq * 220, 3)
local lfo_amp2 = math.sin(tick / (mem_get(3) * 8))
amp(lfo_amp2, 3)
