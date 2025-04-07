#!/bin/bash
set -e

function lua() {
  rm -rf lua*
  target=lua-5.4.7
  curl -L -R -O https://www.lua.org/ftp/$target.tar.gz
  tar zxf $target.tar.gz
  rm -rf $target.tar.gz
  cd $target
  
  # Apply optimization flags for Lua compilation
  if [[ "$(uname)" == "Darwin" ]]; then
    # On macOS, use Apple Clang optimizations
    sed -i '' 's/^CFLAGS= -O2/CFLAGS= -O3 -DNDEBUG -fomit-frame-pointer/g' Makefile
  else
    # On Linux
    sed -i 's/^CFLAGS= -O2/CFLAGS= -O3 -DNDEBUG -fomit-frame-pointer/g' Makefile
  fi
  
  make all test
  cd -
  mv $target ./lua
}

function sdl2() {
  rm -rf SDL2
  rm -rf SDL2-src
  target=SDL2-2.28.4
  curl -L -R -O https://www.libsdl.org/release/$target.tar.gz
  tar zxf $target.tar.gz # unpacks to ./SDL2
  rm -rf $target.tar.gz
  mv $target SDL2-src
  mkdir SDL2
  cd SDL2
  
  # Build SDL2 with optimizations for both audio and video
  if [[ "$(uname)" == "Darwin" ]]; then
    # macOS optimizations
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-O3 -DNDEBUG" -S ../SDL2-src -B .
  else
    # Linux optimizations
    cmake -DCMAKE_BUILD_TYPE=Release -S ../SDL2-src -B .
  fi
  
  make all
  cp ./include-config-*/SDL2/SDL_config.h ./include/SDL2/SDL_config.h
}

lua
sdl2
