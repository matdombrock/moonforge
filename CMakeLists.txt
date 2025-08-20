cmake_minimum_required(VERSION 3.10)
project(mf C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SRC_FILES src/*.c)
add_executable(mf ${SRC_FILES})

target_link_libraries(mf PRIVATE portaudio lua m)
