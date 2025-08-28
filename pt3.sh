#!/bin/bash

PIPE="mfpipe"

# Create the named pipe if it doesn't exist
if [[ ! -p "$PIPE" ]]; then
    mkfifo "$PIPE"
fi

# Save current terminal settings
oldstty=$(stty -g)

# Set terminal to raw mode
stty raw -echo

# Read characters and write to pipe
while true; do
    c=$(dd bs=1 count=1 2>/dev/null)
    # Exit on 'q' key
    if [[ "$c" == "q" ]]; then
        break
    fi
    echo -n "$c" > "$PIPE"
    echo -n "c$c"
done

# Restore terminal settings on exit
trap "stty $oldstty" EXIT
