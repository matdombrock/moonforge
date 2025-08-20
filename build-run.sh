#!/bin/bash

# clang main.c mf.c user.c -o sine_wave -lportaudio -lm

set -e

cd build
cmake ..
make -j$(nproc)

cp ../lua_include/mflib.lua .

./mf $1
