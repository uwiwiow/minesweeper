cmake_minimum_required(VERSION 3.25)
project(minesweeper C)

set(CMAKE_C_STANDARD 11)

find_package(raylib REQUIRED)

add_executable(minesweeper main.c)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# STATIC stuff (Windows)
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(BUILD_FOR_WIN TRUE)
endif()
option(STATIC_BUILD "Build a static binary." ${BUILD_FOR_WIN})

if (STATIC_BUILD)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" CONFIG)
    set(BUILD_SHARED_LIBS OFF)
endif()

target_link_libraries(minesweeper raylib GL m pthread dl rt X11)