#include "model.hpp"
#include <iostream>
#include <cassert>
auto main(int argc, char* argv[]) -> int {
  const auto path = "G:/My Drive/assets/models/survival_backpack/Survival_BackPack_2.fbx";
  auto data = luna::load_model_file(path);
  return 0;
}