#include "scene_graph/scene.hpp"
#include "scene_graph/scene_test.hpp"
#include "scene_graph/render_test.hpp"
#include <iostream>
#include <cassert>
using namespace luna;
auto main() -> int {
  test_scene_graph();
  test_rendering();
  return 0;
}