cmake_minimum_required(VERSION 3.25)
project(minesweeper C)

set(CMAKE_C_STANDARD 11)

find_package(raylib REQUIRED)

add_executable(minesweeper src/main.c)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(minesweeper raylib GL m pthread dl rt X11)

add_executable(server src/server.c)
target_link_libraries(server raylib GL m pthread dl rt X11)