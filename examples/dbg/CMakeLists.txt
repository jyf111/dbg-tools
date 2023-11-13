cmake_minimum_required(VERSION 3.10)

project(dbg_examples LANGUAGES CXX)

add_executable(dbg_demo demo.cpp)
target_include_directories(dbg_demo PRIVATE ../../include/)
target_compile_options(dbg_demo PRIVATE -std=c++20)
