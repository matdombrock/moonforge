cmake_minimum_required(VERSION 3.10)

project(moonforge)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Set optimization flags
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_BUILD_TYPE Release)

# if(APPLE)
#     # Include directories
#     include_directories(/opt/homebrew/Cellar/portaudio/19.7.0/include)
#
#     # Link directories
#     link_directories(/opt/homebrew/Cellar/portaudio/19.7.0/lib)
# endif()

# Specify the path to the Lua library
set(LUA_LIB ${CMAKE_SOURCE_DIR}/lua/src/liblua.a)
set(LUA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lua/src)

# Specify the path to the PortAudio library
set(PORTAUDIO_LIB ${CMAKE_SOURCE_DIR}/portaudio/libportaudio.a)
set(PORTAUDIO_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/portaudio/include)

# Specify the path to the SDL2 library
set(SDL2_LIB ${CMAKE_SOURCE_DIR}/SDL2/libSDL2.a)
set(SDL2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/SDL2/include)
# Add the executable
add_executable(moonforge ./src/main.c)

# Include directories
target_include_directories(moonforge PRIVATE ${LUA_INCLUDE_DIR} ${PORTAUDIO_INCLUDE_DIR} ${SDL2_INCLUDE_DIR})

# Link libraries
if(APPLE)
    target_link_libraries(moonforge 
        ${LUA_LIB} 
        ${PORTAUDIO_LIB} 
        ${SDL2_LIB} 
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
    )
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(moonforge ${LUA_LIB} ${PORTAUDIO_LIB} ${SDL2_LIB} m pthread asound)
endif()

# Custom command to create 'out' directory and move binary and scripts
add_custom_command(TARGET moonforge POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/out
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:moonforge> ${CMAKE_SOURCE_DIR}/out/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/scripts ${CMAKE_SOURCE_DIR}/out/scripts
)
