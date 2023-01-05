add_executable(scene_graph test.cpp scene.cpp)
target_link_libraries(scene_graph PUBLIC luna::math)

#target_include_directories(scene_graph PUBLIC ${})