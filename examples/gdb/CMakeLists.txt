cmake_minimum_required(VERSION 3.10)

project(gdb_examples LANGUAGES CXX)

add_executable(gdb_demo demo.cpp)
target_include_directories(gdb_demo PRIVATE ../../include/)
target_compile_options(gdb_demo PRIVATE -std=c++20 -g)
