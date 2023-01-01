add_library(luna_test_node SHARED main.cpp)
target_link_libraries(luna_test_node luna::gfx ${CONAN_LIBS})
target_include_directories(luna_test_node PUBLIC ${SHADER_DIR})
add_dependencies(luna_test_node shader_compilation)