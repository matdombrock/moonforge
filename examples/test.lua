amp_set(1, 1)
function Loop(tick)
  if tick % 1000 == 0 then
    print("Tick: " .. tick)
  end
end
