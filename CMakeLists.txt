cmake_minimum_required(VERSION 3.1)

project(debugHelper)

option(SINGLE "option for single variable macro" OFF)

if (SINGLE)
  add_compile_options(-std=c++17 -Wall)
  add_definitions(-DSINGLE)
else ()
  add_compile_options(-std=c++2a -Wall)
endif ()

add_executable(test
  tests/test.cpp
)

target_include_directories(test
  PRIVATE
    ${PROJECT_SOURCE_DIR}
)