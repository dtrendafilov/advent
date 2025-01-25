cmake_minimum_required(VERSION 3.28)

project(day03)

set(HEADER_FILES
    precompiled.hxx
)

set(SOURCE_FILES
    solution.cxx
    ${HEADER_FILES}
)

add_executable(day03 ${SOURCE_FILES})
target_precompile_headers(day03 PRIVATE ${HEADER_FILES})

enable_testing()
add_test(NAME day03
    COMMAND day03 --minimal=1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
