wave_set(1, "SAW")
wave_set(2, "SAW")
mute_all()
amp_set(1, 0.5)
amp_set(2, 0.5)
freq_set(1, 440)
freq_set(2, 439)

local it = 0
function Loop(arg)
  print("\nloop", it)
  it = it + 1
  print("arg", arg)
  freq_change(1, 1)
end
