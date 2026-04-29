cmake_minimum_required(VERSION 3.25)

project(neofluxon VERSION 1.0)

# 1. Standard C++ Configuration
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
    message(STATUS "Enabling high-performance optimizations (-O3, -march=native)")

    add_compile_options(
        -O3                  # Maximum optimization
        -march=native        # Vectorize for your specific 16-thread CPU
        -flto                # Link-time optimization
        -ffast-math          # Faster (but less strict) floating point math
        -fno-plt             # Minor speedup for shared library calls
    )

    add_link_options(-O3 -flto -march=native)
endif()

add_subdirectory(src)
