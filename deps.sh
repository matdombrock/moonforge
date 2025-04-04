#!/bin/bash
set -e

function lua() {
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

# function portaudio() {
#   rm -rf portaudio
#   target=pa_stable_v190700_20210406
#   curl -L -R -O https://files.portaudio.com/archives/$target.tgz
#   tar zxf $target.tgz # unpacks to ./portaudio
#   rm -rf $target.tgz
#   cd portaudio
#
#   # Use optimized build flags
#   if [[ "$(uname)" == "Darwin" ]]; then
#     # macOS optimizations
#     cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-O3 -DNDEBUG"  -DBUILD_SHARED_LIBS=OFF .
#   else
#     # Linux optimizations
#     cmake -DCMAKE_BUILD_TYPE=Release  -DBUILD_SHARED_LIBS=OFF .
#   fi
#
#   make all
# }
#
function portaudio() {
 

  rm -rf portaudio
 

  target=pa_stable_v190700_20210406
 

  curl -L -R -O https://files.portaudio.com/archives/$target.tgz
 

  tar zxf $target.tgz # unpacks to ./portaudio
 

  rm -rf $target.tgz
 

  cd portaudio
 

  cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF .
 

  make all
 

}
function sdl2() {
  rm -rf SDL2
  target=SDL2-2.28.4
  curl -L -R -O https://www.libsdl.org/release/$target.tar.gz
  tar zxf $target.tar.gz # unpacks to ./SDL2
  rm -rf $target.tar.gz
  cd SDL2

  # Use optimized build flags
  if [[ "$(uname)" == "Darwin" ]]; then
    # macOS optimizations
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-O3 -DNDEBUG -fomit-frame-pointer" -DBUILD_SHARED_LIBS=OFF .
  else
    # Linux optimizations
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-O3 -DNDEBUG -fomit-frame-pointer" -DBUILD_SHARED_LIBS=OFF .
  fi

  make all
}

# lua
portaudio
# sld2
