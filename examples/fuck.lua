function Loop(tick)
  print("tick", tick)
  local pipe = io.popen("timeout 0.001 head -n 1 ../mfpipe 2>/dev/null", "r")
  if pipe then
    local line = pipe:read("*l")
    pipe:close()
    if line and line ~= "" then
      print(line)
    end
  end
end
