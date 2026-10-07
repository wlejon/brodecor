function(bro_add_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE brodecor)
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    if(MSVC)
        target_compile_options(${name} PRIVATE /W4 /permissive- /utf-8)
    else()
        target_compile_options(${name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME ${name} COMMAND ${name})
endfunction()

bro_add_test(test_types)
bro_add_test(test_frame_geometry)
bro_add_test(test_hit_testing)
bro_add_test(test_button_state)
bro_add_test(test_shadow_mesh)
bro_add_test(test_style)
bro_add_test(test_decorator_engine)

if(BRODECOR_ENABLE_API)
    add_executable(test_decor_api test_decor_api.cpp)
    target_link_libraries(test_decor_api PRIVATE brodecor_api brodecor bronze_runtime_shared)
    target_include_directories(test_decor_api PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/api
        ${PROJECT_SOURCE_DIR}/include
    )
    if(MSVC)
        target_compile_options(test_decor_api PRIVATE /W4 /permissive- /utf-8)
    else()
        target_compile_options(test_decor_api PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME test_decor_api COMMAND test_decor_api)
endif()

