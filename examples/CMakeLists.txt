cmake_minimum_required(VERSION 3.12)

project(DBG_EXAMPLES LANGUAGES CXX)

add_executable(demo demo.cpp)
include_directories(/usr/local/include/dbg_tools/)
target_compile_options(demo PRIVATE -std=c++20)
