enable(2)
enable(3)
enable(4)
enable(5)
enable(6)
enable(7)
enable(8)

local tt = tick / 36.9

local lfo = 0.5 * math.sin(tt / 333) + 0.5
local lfo2 = 0.5 * math.sin(tt / 666) + 0.5
local lfo3 = 0.5 * math.sin(tt / 999) + 0.5

local div = 3.69 / (lfo3 + 0.369)

freq((369 + lfo * 369) / div, 1)
freq((369 + lfo2 * 369) / div, 2)
freq((369 + lfo3 * 369) / div, 3)

freq((333 + lfo * 333) / div, 4)
freq((666 + lfo2 * 666) / div, 5)
freq((999 + lfo3 * 999) / div, 6)

freq(369 * (lfo + lfo2 + lfo3 / 3), 7)
freq(2 * (369 * (lfo + lfo2 + lfo3 / 3)), 8)
amp(-0.5 + lfo, 8)
