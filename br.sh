#!/bin/bash

# clang main.c mf.c user.c -o sine_wave -lportaudio -lm

set -e

cd build
cmake ..
make -j$(nproc)

cp ../mflib.lua .

./mf $1
