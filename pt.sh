# Create the named pipe if it doesn't exist
mkfifo mfpipe

# Bash script to write random values between 0 and 1 every second
while true; do
  value=$(awk -v seed=$RANDOM 'BEGIN { srand(seed); print rand() }')
  echo "$value"
  echo "$value" > mfpipe
  sleep 0.01
done
