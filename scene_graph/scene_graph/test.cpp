#include "scene_graph/scene.hpp"
#include <iostream>
#include <cassert>
using namespace luna;
auto main() -> int {
  constexpr auto cNumNodes = 10u;
  auto scene = Scene();
  auto type_jump_table = std::unordered_map<std::size_t, std::function<void(const Node&)>>();
  auto text_jump_table = std::unordered_map<std::int32_t, std::function<void(const Node&)>>();
  auto image_jump_table = std::unordered_map<std::int32_t, std::function<void(const Node&)>>();

    // Provide the scene with some data types.
  type_jump_table[scene.add_node_type("Text")] = [&text_jump_table](const Node& node) {
    std::cout << "Text!" << std::endl;
    text_jump_table[node.data()](node);
  };

  type_jump_table[scene.add_node_type("Image")] = [&image_jump_table](const Node& node) {
    std::cout << "Image!" << std::endl;
    image_jump_table[node.data()](node);
  };

  text_jump_table[scene.add_data_type("SansSerif")] = [](const Node&) {
    std::cout << "SansSerif!" << std::endl;
  };
  
  text_jump_table[scene.add_data_type("ComicSans")] = [](const Node&) {
    std::cout << "ComicSans!" << std::endl;
  };

  image_jump_table[scene.add_data_type("MonaLisa")] = [](const Node&) {
    std::cout << "MonaLisa!" << std::endl;
  };

  // Add some nodes to the root node.
  scene.add_child({"Text", "ComicSans", {{0.2f, 0.5f, 0.7f}}});
  auto node = scene.add_child({"Text", "SansSerif", {{0.9f, 2.f, 10.f}}});

  auto tmp = node;
  for(auto i = 0u; i < cNumNodes; ++i) {
    tmp = scene.add_child({"Image", "MonaLisa"}, tmp);
  }

  auto visit_func = [&type_jump_table](const Node& node) {
    assert(type_jump_table.find(node.type()) != type_jump_table.end());
    type_jump_table[node.type()](node);
  };

  std::cout << "---------------------------" << std::endl;
  scene.traverse(visit_func);
  std::cout << "---------------------------" << std::endl;
  scene.print();
  std::cout << "---------------------------" << std::endl;
  scene.remove(node);
  scene.traverse(visit_func);
  std::cout << "---------------------------" << std::endl;
  scene.print();
  std::cout << "---------------------------" << std::endl;
  return 0;
}