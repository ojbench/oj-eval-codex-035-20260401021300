cmake_minimum_required(VERSION 3.10)
project(problem_035_skywong_string LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(code
    src/main.cpp
)

target_include_directories(code PRIVATE include)

# Optimize reasonably while keeping debug symbols if needed
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(code PRIVATE -Wall -Wextra -Wpedantic)
endif()

