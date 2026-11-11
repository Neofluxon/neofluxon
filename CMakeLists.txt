cmake_minimum_required(VERSION 3.31)

project(neofluxon VERSION 0.1.0)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(NEOFLUXON_DISPLAY_NAME "Neofluxon")

math(EXPR NEOFLUXON_VERSION_HEX
     "0x${PROJECT_VERSION_MAJOR} * 0x10000
          + 0x${PROJECT_VERSION_MINOR} * 0x100
          + 0x${PROJECT_VERSION_PATCH}")
set(NEOFLUXON_VERSION_STR
    "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

message(STATUS "Neofluxon version: ${NEOFLUXON_VERSION_STR} (${NEOFLUXON_VERSION_HEX})")

add_compile_definitions(
    NEOFLUXON_NAME="${PROJECT_NAME}"
    NEOFLUXON_APP_NAME="${NEOFLUXON_DISPLAY_NAME}"
    NEOFLUXON_VERSION_HEX=${NEOFLUXON_VERSION_HEX}
    NEOFLUXON_VERSION_STR="${NEOFLUXON_VERSION_STR}"
)

include(GNUInstallDirs)

# 1. Standard C++ Configuration
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
    message(STATUS "Enabling high-performance optimizations (-O3, -march=native)")

    add_compile_options(
        -O3                  # Maximum optimization
        -ffast-math          # Faster (but less strict) floating point math
        -fno-plt             # Minor speedup for shared library calls
    )

    add_link_options(-O3 -flto -march=native)
endif()

add_subdirectory(src)
