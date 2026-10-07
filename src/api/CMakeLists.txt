add_library(brodecor_api STATIC
    api.cpp
    native_decor.cpp
)

target_include_directories(brodecor_api PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
)

target_link_libraries(brodecor_api PUBLIC
    brodecor
    bronze_runtime_shared
)

target_compile_features(brodecor_api PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(brodecor_api PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(brodecor_api PRIVATE -Wall -Wextra -Wpedantic)
endif()
