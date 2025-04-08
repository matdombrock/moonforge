local tt = tick % 128
env(16, 16, 16, 1)
-- print(tt)
if tt == 1 then
  print("note")
  note(69, 1)
end
