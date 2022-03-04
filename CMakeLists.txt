cmake_minimum_required(VERSION 3.1)

project(debugHelper)

add_compile_options(-std=c++2a -Wall)

add_executable(test
  tests/test.cpp
)

target_include_directories(test
  PRIVATE
    ${PROJECT_SOURCE_DIR}
)