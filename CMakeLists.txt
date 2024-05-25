cmake_minimum_required(VERSION 3.25)
project(minesweeper C)

set(CMAKE_C_STANDARD 11)

add_executable(minesweeper main.c)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(minesweeper SDL2 SDL2_ttf SDL2_image)
