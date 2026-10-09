cmake_minimum_required(VERSION 3.24)
project(brodecor VERSION 0.1.0 LANGUAGES CXX)

if(MSVC)
    # The DLL CRT, so the tests share one heap with bronze_runtime_shared.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

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

# Dependencies (cmake/bro_deps.cmake): an outer project's target wins (under bro
# these are all bro's), else the ../<name> working tree beside the top-level
# project, else the pinned commit, fetched at configure.
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/bro_deps.cmake)

# bromath, header-only math.
bro_dependency(bromath GITHUB wlejon/bromath REF 8b511fbd69c014bf0584d2b42e6c96a8cf720f6a)

add_subdirectory(src)

# --- Standalone Bronze JavaScript API ---
option(BRODECOR_ENABLE_API "Build Bronze JavaScript API" ${_bd_top_level})
if(BRODECOR_ENABLE_API)
    # bronze, the JavaScript runtime the api binds (it brings its code
    # generator, brass).
    bro_dependency(bronze GITHUB wlejon/bronze REF 15ce4cf5fc25d8afe03882985ae95adcba46a6a9 TARGET bronze_runtime_shared
                   OPTIONS BRONZE_BUILD_SHARED_RUNTIME=ON BRONZE_BUILD_TESTS=OFF)

    add_subdirectory(src/api)
endif()

if(BRODECOR_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
