#!/bin/bash

set -e

cd build
cmake ..
make -j$(nproc)

cd ..
mkdir -p dist
rm -rf ./dist/*
cp ./build/mf ./dist/mf
mkdir -p ./dist/util
rm -rf ./dist/util/*
cp ./lua_include/mflib.lua ./dist/util/mflib.lua
mkdir -p ./dist/examples
rm -rf ./dist/examples/*
cp -r ./examples/*.lua ./dist/examples/
