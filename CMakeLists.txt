cmake_minimum_required(VERSION 3.1)

project(debugHelper LANGUAGES CXX)

option(SINGLE "option for single variable macro" OFF)

add_executable(test tests/test_catch2.cpp)

if (SINGLE)
  target_compile_options(test
  PUBLIC
    -std=c++17
  )
  target_compile_definitions(test
  PUBLIC
    -DSINGLE
  )
else()
  target_compile_options(test
  PUBLIC
    -std=c++2a
  )
endif()

target_include_directories(test
PRIVATE
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/include
)