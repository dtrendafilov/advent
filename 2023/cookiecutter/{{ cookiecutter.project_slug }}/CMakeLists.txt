cmake_minimum_required(VERSION 3.28)

project({{ cookiecutter.project_slug }})

set(HEADER_FILES
    precompiled.hxx
)

set(SOURCE_FILES
    solution.cxx
    ${HEADER_FILES}
)

add_executable({{ cookiecutter.project_slug }} ${SOURCE_FILES})
target_precompile_headers({{ cookiecutter.project_slug }} PRIVATE ${HEADER_FILES})

enable_testing()
add_test(NAME {{ cookiecutter.project_slug }}
    COMMAND {{ cookiecutter.project_slug }} --minimal=1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
