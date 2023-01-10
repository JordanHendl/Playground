#pragma once
#include <string_view>
#include <vector>
#include <map>
#include "luna-gfx/gfx.hpp"
#include "luna-gfx/ext/ext.hpp"
namespace luna {
namespace gfx {
using ivec2 = vec2_t<int>;
struct FontInfo {
  FontInfo() {};
  int gpu = 0;
  ivec2 resolution = {1024, 1024};
  float size = 48.f;
};

class Font {
public:
  struct Rect {
    vec2 top_left;
    vec2 bottom_right;
  };

  struct Character {
    Rect rect;
    float advance;
    vec2 offset1;
    vec2 offset2;
  };

  struct TextVertex {
    vec2 pos;
    vec2 tex_coord;
  };

  Font();
  Font(std::string_view ttf_file, FontInfo info = {});
  Font(std::vector<unsigned char> raw_ttf_bytes, FontInfo info = {});
  Font(Font&& mv) = default;
  ~Font();

  auto atlas() -> ImageView {return this->image;}
  auto vertices(std::string_view str, vec2 pos = {-1.f, -0.9f}, float scale = 1.0f) -> gfx::Vector<TextVertex>;
  auto operator=(Font&& mv) -> Font& = default;
private:
  gfx::Image image;
  std::map<char, Character> characters;
};
}
}