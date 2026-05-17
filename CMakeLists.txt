cmake_minimum_required(VERSION 3.14)
project(BlackScholes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add executable
add_executable(black_scholes 
    src/bsc.cpp
)

# Include directory
target_include_directories(black_scholes PRIVATE include)

# warnings
if(MSVC)
    target_compile_options(black_scholes PRIVATE /W4)
else()
    target_compile_options(black_scholes PRIVATE -Wall -Wextra -Wpedantic)
endif()