cmake_minimum_required(VERSION 3.16)

project(solution)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

set(HEADER_FILES
    precompiled.hxx
)

set(SOURCE_FILES
    solution.cxx
    ${HEADER_FILES}
)

add_executable(solution ${SOURCE_FILES})
set_property(TARGET solution PROPERTY CXX_STANDARD 23)
target_precompile_headers(solution PRIVATE ${HEADER_FILES})

enable_testing()
add_test(NAME solution
    COMMAND solution --minimal=1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
