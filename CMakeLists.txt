cmake_minimum_required(VERSION 3.14)
project(BlackScholes LANGUAGES CXX)

# Global C++ standard configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Main Executable Target
# add_executable(black_scholes
#     src/bsc.cpp
# )
# target_include_directories(black_scholes PRIVATE include)

# 2. Test Executable Target
find_package(GTest REQUIRED)

# Compile sources + tests into one binary. gtest_main supplies main().
add_executable(run_tests
    tests/bsc_test.cpp
    tests/mc_test.cpp
    src/bsc.cpp
    src/mc.cpp
)
target_include_directories(run_tests PRIVATE include)
target_link_libraries(run_tests PRIVATE GTest::gtest GTest::gtest_main)

# Register with CTest so `ctest` picks up the test cases
include(GoogleTest)
enable_testing()
gtest_discover_tests(run_tests)

# 3. Compiler Warnings
if(MSVC)
    target_compile_options(run_tests PRIVATE /W4)
else()
    target_compile_options(run_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()