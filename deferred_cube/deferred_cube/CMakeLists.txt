add_executable(deferred_cube luna_deferred_test.cpp)
target_link_libraries(deferred_cube luna::gfx ${CONAN_LIBS})
target_include_directories(deferred_cube PUBLIC ${SHADER_DIR})
add_dependencies(deferred_cube shader_compilation)