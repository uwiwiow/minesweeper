cmake_minimum_required(VERSION 3.25)
project(buscaMinas C)

set(CMAKE_C_STANDARD 11)

add_executable(buscaMinas main.c)

target_link_libraries(buscaMinas SDL2 SDL2_ttf)
