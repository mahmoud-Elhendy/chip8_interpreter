cmake_minimum_required(VERSION 3.5)
project(chip8)
set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

add_executable(chip8 main.cpp renderer.cpp keyshandler.cpp chip8.cpp miniaudio.c)

target_link_libraries(chip8 ${SDL2_LIBRARIES} dl pthread)