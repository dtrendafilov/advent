cmake_minimum_required(VERSION 3.28)

project(day10)

set(HEADER_FILES
    precompiled.hxx
)

set(SOURCE_FILES
    solution.cxx
    ${HEADER_FILES}
)

add_executable(day10 ${SOURCE_FILES})
set_property(TARGET day10 PROPERTY CXX_STANDARD 23)
target_precompile_headers(day10 PRIVATE ${HEADER_FILES})

enable_testing()
add_test(NAME day10
    COMMAND day10 --minimal=1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
