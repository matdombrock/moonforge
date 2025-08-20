#!/bin/bash

# clang main.c mf.c user.c -o sine_wave -lportaudio -lm

set -e

cd build
cmake ..
make -j$(nproc)

cd ..
mkdir -p dist
rm -f ./dist/*
cp ./build/mf ./dist/mf
cp ./lua_include/mflib.lua ./dist/mflib.lua

cd dist
./mf $1
