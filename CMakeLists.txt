cmake_minimum_required(VERSION 3.25)
project(minesweeper C)

set(CMAKE_C_STANDARD 11)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(minesweeper main.c)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(minesweeper SDL2::SDL2 SDL2_ttf::SDL2_ttf SDL2_image::SDL2_image)
