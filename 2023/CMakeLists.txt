cmake_minimum_required(VERSION 3.28)

project(aoc2003)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

set(CMAKE_COMPILE_WARNING_AS_ERROR Off)
set(CMAKE_EXPORT_COMPILE_COMMANDS On)

set(CMAKE_CXX_STANDARD 23)

enable_testing()

add_subdirectory(day10)

