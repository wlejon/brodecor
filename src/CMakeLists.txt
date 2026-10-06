add_library(brodecor
    frame_geometry.cpp
    button_state.cpp
    shadow_mesh.cpp
    style.cpp
    decor.cpp
)
add_library(brodecor::brodecor ALIAS brodecor)

target_include_directories(brodecor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(brodecor
    PUBLIC
        bromath
)

target_compile_features(brodecor PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(brodecor PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(brodecor PRIVATE -Wall -Wextra -Wpedantic)
endif()
