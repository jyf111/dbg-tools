cmake_minimum_required(VERSION 3.12)

project(dbg_tools LANGUAGES CXX)

include(GNUInstallDirs)

add_library(${PROJECT_NAME} INTERFACE)

target_compile_features(${PROJECT_NAME}
  INTERFACE
    cxx_std_20
)

target_include_directories(${PROJECT_NAME}
  INTERFACE
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

option(ENABLE_TESTS "enable tests" OFF)
option(BUILD_EXAMPLES "build examples" OFF)

# tests
if (ENABLE_TESTS)
  find_package(Catch2 3 REQUIRED)
  add_executable(unit_test tests/unit_test.cpp)
  target_link_libraries(unit_test PRIVATE Catch2::Catch2WithMain)
  target_include_directories(unit_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
  include(CTest)
  include(Catch)
  catch_discover_tests(unit_test)
  enable_testing()
endif()

# examples
if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# install
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# clang-format target
file(GLOB_RECURSE
  FILES_NEED_FORMAT
  CONFIGURE_DEPENDS
  "examples/*.cpp"
  "include/*.h"
  "tests/*.cpp"
)

add_custom_target(format
  COMMAND clang-format --style=file -i ${FILES_NEED_FORMAT}
  COMMENT "Running clang-format."
  VERBATIM
)
