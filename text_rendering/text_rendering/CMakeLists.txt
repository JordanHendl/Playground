add_executable(text_test test.cpp font.cpp)
target_link_libraries(text_test PUBLIC luna::gfx SDL2::SDL2)
target_include_directories(text_test PUBLIC ${SHADER_DIR})
add_dependencies(text_test shader_compilation)