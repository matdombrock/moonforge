freq=2
amplitude=1
dt=0.01
t=0

while true; do
  value=$(awk -v t="$t" -v freq="$freq" -v amp="$amplitude" 'BEGIN { print amp * sin(2 * 3.141592653589793 * freq * t) }')
  echo "$value"
  echo "$value" > mfpipe
  t=$(awk -v t="$t" -v dt="$dt" 'BEGIN { print t + dt }')
  sleep $dt
done
