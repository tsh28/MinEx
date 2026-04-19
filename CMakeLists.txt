cmake_minimum_required(VERSION 3.16)
project(MinEx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Core matching engine library
add_library(mx_engine
    src/engine/engine.cpp
)
target_include_directories(mx_engine PUBLIC include)

# CLI simulator that drives the engine
add_executable(sim_cli
    src/tools/sim_cli.cpp
)
target_link_libraries(sim_cli PRIVATE mx_engine)

# Unit tests (GoogleTest)
enable_testing()
find_package(GTest REQUIRED)
add_executable(unit_tests
    tests/unit/SinkTest.cpp
)
target_link_libraries(unit_tests PRIVATE mx_engine GTest::gtest_main)
target_include_directories(unit_tests PRIVATE include)
add_test(NAME unit_tests COMMAND unit_tests)
