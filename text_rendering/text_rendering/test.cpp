#include "text_rendering/sdl_text_test.hpp"
#include "text_rendering/render_test.hpp"
#include "font.hpp"
#include <iostream>
#include <cassert>
auto main(int argc, char* argv[]) -> int {
  constexpr auto file_path = "C:/Windows/Fonts/arial.ttf";
  auto f = luna::gfx::Font(file_path/*, {}*/);
  //test_sdl_text();
  render_test();
  return 0;
}