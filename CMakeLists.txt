cmake_minimum_required(VERSION 3.24)
project(brodecor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(_bd_top_level OFF)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(_bd_top_level ON)
endif()

option(BRODECOR_BUILD_TESTS "Build brodecor tests" ${_bd_top_level})

# Coverage support
option(BRODECOR_COVERAGE "Instrument the build for gcov coverage (GCC/Clang)" OFF)
if(BRODECOR_COVERAGE AND _bd_top_level)
    if(MSVC)
        message(FATAL_ERROR "BRODECOR_COVERAGE requires GCC or Clang (gcov instrumentation)")
    endif()
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

if(MSVC)
    add_compile_options(/MP /permissive- /utf-8)
endif()

# bromath dependency (standalone checkout at ../bromath or specified BROMATH_DIR)
if(NOT TARGET bromath)
    if(DEFINED BROMATH_DIR AND EXISTS "${BROMATH_DIR}/CMakeLists.txt")
        # explicit BROMATH_DIR supplied
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../bromath/CMakeLists.txt")
        set(BROMATH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../bromath" CACHE PATH "Path to standalone bromath repo")
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/../bromath/CMakeLists.txt")
        set(BROMATH_DIR "${CMAKE_SOURCE_DIR}/../bromath" CACHE PATH "Path to standalone bromath repo")
    endif()

    if(NOT BROMATH_DIR OR NOT EXISTS "${BROMATH_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "brodecor: bromath not found at ${BROMATH_DIR}. Please check it out at ../bromath or pass -DBROMATH_DIR=<path>")
    endif()

    message(STATUS "brodecor: bromath from ${BROMATH_DIR}")
    add_subdirectory("${BROMATH_DIR}" "${CMAKE_BINARY_DIR}/bromath" EXCLUDE_FROM_ALL)
endif()

add_subdirectory(src)

if(BRODECOR_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
