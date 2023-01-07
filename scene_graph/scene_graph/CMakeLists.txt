add_executable(scene_graph test.cpp scene.cpp)
target_link_libraries(scene_graph PUBLIC luna::gfx)
target_include_directories(scene_graph PUBLIC ${SHADER_DIR})
add_dependencies(scene_graph shader_compilation)