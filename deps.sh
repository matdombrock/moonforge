#!/bin/sh

DEPS="gcc make cmake portaudio lua"

if command -v apt-get >/dev/null; then
  sudo apt-get update
  sudo apt-get install -y $DEPS
elif command -v dnf >/dev/null; then
  sudo dnf install -y $DEPS
elif command -v yum >/dev/null; then
  sudo yum install -y $DEPS
elif command -v pacman >/dev/null; then
  sudo pacman -Sy --noconfirm $DEPS
else
  echo "Unsupported package manager."
  exit 1
fi
