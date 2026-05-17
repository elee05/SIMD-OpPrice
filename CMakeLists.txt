cmake_minimum_required(VERSION 3.14)
project(BlackScholes LANGUAGES CXX)

# Global C++ standard configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# 1. Main Executable Target
# ---------------------------------------------------------------------------
# add_executable(black_scholes 
#     src/bsc.cpp
# )
# target_include_directories(black_scholes PRIVATE include)

# ---------------------------------------------------------------------------
# 2. Test Executable Target
# ---------------------------------------------------------------------------
find_package(GTest REQUIRED)

# Note: We include src/bsc.cpp so the test binary actually contains 
# the implementation of your black_scholes functions.
add_executable(run_tests 
    greekTest.cpp 
    src/bsc.cpp 
)
# Ensure the test suite can also see your header files
target_include_directories(run_tests PRIVATE include)
target_link_libraries(run_tests PRIVATE GTest::gtest GTest::gtest_main)

# ---------------------------------------------------------------------------
# 3. Compiler Warnings (Applied to both targets)
# ---------------------------------------------------------------------------
if(MSVC)
    # target_compile_options(black_scholes PRIVATE /W4)
    target_compile_options(run_tests PRIVATE /W4)
else()
    # target_compile_options(black_scholes PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(run_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()