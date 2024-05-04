cmake_minimum_required(VERSION 3.28)

project(day13)

set(HEADER_FILES
    precompiled.hxx
)

set(SOURCE_FILES
    solution.cxx
    ${HEADER_FILES}
)

add_executable(day13 ${SOURCE_FILES})
target_precompile_headers(day13 PRIVATE ${HEADER_FILES})

enable_testing()
add_test(NAME day13
    COMMAND day13 --minimal=1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
