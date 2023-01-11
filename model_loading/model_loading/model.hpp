#pragma once
#include "luna-gfx/gfx.hpp"
#include "luna-gfx/ext/ext.hpp"
#include <string_view>
#include <cstdint>
#include <cassert>
namespace luna {
struct Vertex {

};

struct Mesh {
  Mesh(const Mesh& cpy) = delete;
  auto operator=(const Mesh& cpy) -> Mesh& = delete;

  Mesh(Mesh&& mv) = default;
  auto operator=(Mesh&& mv) -> Mesh& = default;
  gfx::Vector<Vertex> vertices;
  gfx::Vector<std::uint32_t> indices;
};

class Model {
public:
  Model();
  Model(std::string_view path);
  ~Model();

  auto begin() {return this->m_meshes.begin();}
  auto end() {return this->m_meshes.end();}
private:
  std::vector<Mesh> m_meshes;
};
}