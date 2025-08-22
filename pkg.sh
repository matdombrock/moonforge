#!/bin/bash

set -e

mkdir -p build
cd build
cmake ..
make -j$(nproc)

cd ..
mkdir -p dist
rm -rf ./dist/*
cp ./build/mf ./dist/mf
mkdir -p ./dist/util
cp ./lua_include/* ./dist/util/
mkdir -p ./dist/examples
cp -r ./examples/*.lua ./dist/examples/
