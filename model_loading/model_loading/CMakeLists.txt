add_executable(model_test test.cpp model.cpp data_manager.cpp)
target_link_libraries(model_test PUBLIC luna::gfx SDL2::SDL2 assimp::assimp)
target_include_directories(model_test PUBLIC ${SHADER_DIR})
add_dependencies(model_test shader_compilation)