add_executable(text_test test.cpp)
target_link_libraries(text_test PUBLIC luna::gfx)
target_include_directories(text_test PUBLIC ${SHADER_DIR})
add_dependencies(text_test shader_compilation)