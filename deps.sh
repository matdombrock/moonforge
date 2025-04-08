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
  mkdir -p SDL2
  
  # Build SDL2 using configure script to ensure sdl2-config is generated
  cd SDL2-src
  
  if [[ "$(uname)" == "Darwin" ]]; then
    # macOS optimizations
    ./configure --prefix=$(pwd)/../SDL2 CFLAGS="-O3 -DNDEBUG"
  else
    # Linux optimizations
    ./configure --prefix=$(pwd)/../SDL2 CFLAGS="-O3 -DNDEBUG"
  fi
  
  make
  make install
  cd ..
  
  # Create compatibility symlink
  if [ ! -f SDL2/libSDL2.a ]; then
    ln -sf SDL2/lib/libSDL2.a SDL2/libSDL2.a
  fi
}

function sdl2_ttf() {
  rm -rf SDL2_ttf
  rm -rf SDL2_ttf-src
  target=SDL2_ttf-2.20.2
  curl -L -R -O https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.2/$target.tar.gz
  tar zxf $target.tar.gz
  rm -rf $target.tar.gz
  mv $target SDL2_ttf-src
  mkdir -p SDL2_ttf
  
  # First make sure FreeType is installed
  if [[ "$(uname)" == "Darwin" ]]; then
    # Check if FreeType is installed on macOS (using Homebrew)
    if ! brew list --formula | grep -q freetype; then
      echo "Installing FreeType dependency with Homebrew"
      brew install freetype
    fi
  else
    # Check if FreeType development files are installed on Linux
    if ! pkg-config --exists freetype2; then
      echo "FreeType development files are required."
      echo "Please install using your package manager:"
      echo "  Debian/Ubuntu: sudo apt-get install libfreetype6-dev"
      echo "  Fedora/RHEL: sudo dnf install freetype-devel"
      echo "  Arch: sudo pacman -S freetype2"
      exit 1
    fi
  fi
  
  # We'll build SDL_ttf directly without cmake
  cd SDL2_ttf-src
  
  # Export SDL paths properly
  export SDL2_CONFIG=$(pwd)/../SDL2/bin/sdl2-config || $(pwd)/../SDL2/sdl2-config
  
  # Configure with SDL2 path
  if [[ "$(uname)" == "Darwin" ]]; then
    # macOS configuration
    ./configure --prefix=$(pwd)/../SDL2_ttf \
                CFLAGS="-O3 -I$(pwd)/../SDL2/include" \
                LDFLAGS="-L$(pwd)/../SDL2"
  else
    # Linux configuration
    ./configure --prefix=$(pwd)/../SDL2_ttf \
                CFLAGS="-O3 -I$(pwd)/../SDL2/include" \
                LDFLAGS="-L$(pwd)/../SDL2"
  fi
  
  make
  make install
  cd ..
  
  # Create symlink for compatibility
  if [ ! -f SDL2_ttf/libSDL2_ttf.a ]; then
    ln -sf SDL2_ttf/lib/libSDL2_ttf.a SDL2_ttf/libSDL2_ttf.a
  fi
}

lua
sdl2
sdl2_ttf
