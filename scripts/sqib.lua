local n = tick
n = math.floor(n / 16)
n = n % 16
n = n + 80
n = math.floor(n)
note(n, 1)
