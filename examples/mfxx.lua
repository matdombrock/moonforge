-- Initial setup
for i = 1, 8 do
  wave_set(i, "SAW")
  amp_set(i, 0.25)
  freq_set(i, 440 - (i - 8))
end

function Loop(_)
end
