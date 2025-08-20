cmake_minimum_required(VERSION 3.10)
project(mf C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(mf main.c mf.c as.c)

# Link PortAudio and math library directly
target_link_libraries(mf PRIVATE portaudio lua m)
