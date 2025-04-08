cmake_minimum_required(VERSION 3.10)

project(moonforge)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Set optimization flags
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_BUILD_TYPE Release)

# Specify the path to the Lua library
set(LUA_LIB ${CMAKE_SOURCE_DIR}/lua/src/liblua.a)
set(LUA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lua/src)

# Specify the path to the SDL2 library
# First try the lib directory, then fall back to the root if needed
if(EXISTS "${CMAKE_SOURCE_DIR}/SDL2/lib/libSDL2.a")
  set(SDL2_LIB ${CMAKE_SOURCE_DIR}/SDL2/lib/libSDL2.a)
else()
  set(SDL2_LIB ${CMAKE_SOURCE_DIR}/SDL2/libSDL2.a)
endif()
set(SDL2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/SDL2/include)

# Make sure the SDL2 library exists
if(NOT EXISTS "${SDL2_LIB}")
  message(FATAL_ERROR "SDL2 library not found at ${SDL2_LIB}. Please run the deps.sh script first.")
endif()

# Specify the path to the SDL2_ttf library
# First try the lib directory, then fall back to the root if needed
if(EXISTS "${CMAKE_SOURCE_DIR}/SDL2_ttf/lib/libSDL2_ttf.a")
  set(SDL2_TTF_LIB ${CMAKE_SOURCE_DIR}/SDL2_ttf/lib/libSDL2_ttf.a)
else()
  set(SDL2_TTF_LIB ${CMAKE_SOURCE_DIR}/SDL2_ttf/libSDL2_ttf.a)
endif()

# Include directory should be in the standard location
set(SDL2_TTF_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/SDL2_ttf/include)

# Make sure the SDL2_ttf library exists
if(NOT EXISTS "${SDL2_TTF_LIB}")
  message(FATAL_ERROR "SDL2_ttf library not found at ${SDL2_TTF_LIB}. Please run the deps.sh script first.")
endif()

# Add the executable
add_executable(moonforge ./src/main.c)

# Include directories
target_include_directories(moonforge PRIVATE 
    ${LUA_INCLUDE_DIR} 
    ${SDL2_INCLUDE_DIR}
    ${SDL2_TTF_INCLUDE_DIR}
)

# Link libraries
if(APPLE)
    target_link_libraries(moonforge 
        ${LUA_LIB} 
        ${SDL2_LIB}
        ${SDL2_TTF_LIB}
        m 
        pthread
        "-framework Cocoa" 
        "-framework IOKit" 
        "-framework CoreVideo" 
        "-framework CoreAudio" 
        "-framework AudioUnit" 
        "-framework AudioToolbox" 
        "-framework CoreFoundation" 
        "-framework Carbon" 
        "-framework GameController" 
        "-framework Metal" 
        "-framework QuartzCore" 
        "-framework CoreHaptics" 
        "-framework ForceFeedback" 
        "-liconv"
        "-framework FreeType"
    )
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(moonforge 
        ${LUA_LIB} 
        ${SDL2_LIB} 
        ${SDL2_TTF_LIB}
        m 
        pthread 
        asound
        freetype
    )
endif()

# Custom command to create 'out' directory and move binary and scripts
add_custom_command(TARGET moonforge POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/out
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:moonforge> ${CMAKE_SOURCE_DIR}/out/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/scripts ${CMAKE_SOURCE_DIR}/out/scripts
)