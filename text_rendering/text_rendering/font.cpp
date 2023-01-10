#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "font.hpp"
#include <algorithm>
#include <istream>
#include <fstream>
#include <cassert>
#include <memory>
#include <iostream>
#include <cstdio>
namespace luna {
namespace gfx {
Font::Font() {}
Font::Font(std::string_view ttf_file, FontInfo in_info) {
  constexpr auto cWOversample = 2;
  constexpr auto cHOversample = 2;
  auto vec = std::vector<unsigned char>();
  auto pixels = std::vector<unsigned char>();
  auto pack = std::vector<stbtt_packedchar>(126);

  auto file = fopen(ttf_file.data(), "rb");
  assert(file);
  std::fseek(file, 0, SEEK_END);
  vec.resize(std::ftell(file));
  std::fseek(file, 0, SEEK_SET);
   
  // Copy stream's contents into char buffer.
  std::fread(vec.data(), vec.size(), 1, file);

  auto info = stbtt_fontinfo();
  auto ctx = stbtt_pack_context();
  pixels.resize(in_info.resolution.x * in_info.resolution.y);
  //assert(stbtt_InitFont(&info, vec.data(), stbtt_GetFontOffsetForIndex(vec.data(),0)));
  assert(stbtt_PackBegin(&ctx, pixels.data(), in_info.resolution.x, in_info.resolution.y, 0, 1, nullptr));
  //stbtt_PackSetOversampling(&ctx, 0, 1);
  assert(stbtt_PackFontRange(&ctx, vec.data(), 0, in_info.size, ' ', pack.size(), pack.data()));
  stbtt_PackEnd(&ctx);

  this->image = std::move(gfx::Image({ttf_file.data(), 
                                      in_info.gpu,
                                      static_cast<std::size_t>(in_info.resolution.x),
                                      static_cast<std::size_t>(in_info.resolution.y),
                                      1,
                                      gfx::ImageFormat::R8}));
  this->image.upload(pixels.data());

  // Now, we need to just translate a stbtt_packedchar to our own struct.
  auto idx = ' ';
  for(auto& p : pack) {
    this->characters[idx++] = {
                                                  {{static_cast<float>(p.x0), static_cast<float>(p.y0)}, {static_cast<float>(p.x1), static_cast<float>(p.y1)}},
                                                  p.xadvance, 
                                                  {p.xoff, p.yoff},
                                                  {p.xoff2, p.yoff2}
                                                 };
  }
}

Font::Font(std::vector<unsigned char> raw_ttf_bytes, FontInfo info ) {

}

Font::~Font() {
}

inline auto ndc(Font::Rect r, std::size_t width, std::size_t height) -> Font::Rect {
  auto w = static_cast<float>(width);
  auto h = static_cast<float>(height);
  return {{r.top_left.x / w, r.top_left.y / h}, {r.bottom_right.x / w, r.bottom_right.y / h}};
}

auto Font::vertices(std::string_view str, vec2 pos, float scale) -> gfx::Vector<TextVertex> {
  auto cpu_vec = std::vector<Font::TextVertex>();
  cpu_vec.reserve(str.size() * 6u);
  auto xpos = pos.x;
  for(auto ch : str) {
    auto iter = this->characters.find(ch);
    if(iter != this->characters.end()) {
      auto& c = iter->second;
      // Convert the rect from bitmap coordinates to NDC coordinates.
      auto& r = ndc(c.rect, this->image.info().width, this->image.info().height);
      auto xpos1 = scale * (xpos);
      auto xpos2 = scale * (r.bottom_right.x - r.top_left.x + xpos);
      auto ypos1 = scale * ((c.offset1.y / this->image.info().height) + pos.y);
      auto ypos2 = scale * ((c.offset2.y / this->image.info().height) + pos.y);
      cpu_vec.push_back({{xpos1, ypos1}, {r.top_left.x, r.top_left.y}});
      cpu_vec.push_back({{xpos2, ypos1}, {r.bottom_right.x, r.top_left.y}});
      cpu_vec.push_back({{xpos1, ypos2}, {r.top_left.x, r.bottom_right.y}});

      cpu_vec.push_back({{xpos2, ypos2}, {r.bottom_right.x, r.bottom_right.y}});
      cpu_vec.push_back({{xpos2, ypos1}, {r.bottom_right.x, r.top_left.y}});
      cpu_vec.push_back({{xpos1, ypos2}, {r.top_left.x, r.bottom_right.y}});
      xpos += (c.advance / this->image.info().width) * scale;
    }
  }

  auto vec = cpu_vec.empty() ? gfx::Vector<Font::TextVertex>() : gfx::Vector<Font::TextVertex>(this->image.info().gpu, cpu_vec.size());
  if(vec.size() > 0) vec.upload(cpu_vec.data());
  return vec;
}
}
}
