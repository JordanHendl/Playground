#pragma once
#include "luna-gfx/gfx.hpp"
#include "luna-gfx/ext/ext.hpp"
#include <string_view>
#include <cstdint>
#include <cassert>
#include <unordered_map>

namespace luna {
constexpr auto cMaxBones = 4u;
class Model;
struct Vertex {
  vec3 position;
  vec3 normal;
  vec2 tex_coords;
  vec3 tangent;
  vec3 bitangent;
  std::int32_t bone_ids[cMaxBones];
  float bone_weights[cMaxBones];
};

struct AnimationInfo {};
struct ModelLoadInfo;
auto load_model_file(std::string_view filename) -> ModelLoadInfo;

enum class TextureType {
  Unknown,
  Ambient,
  Diffuse,
  Specular,
  Normal,
  Height
};

struct Mesh {
  // RAII Object.
  Mesh() = default;
  ~Mesh() = default;
  Mesh(const Mesh& cpy) = delete;
  auto operator=(const Mesh& cpy) -> Mesh& = delete;

  Mesh(Mesh&& mv) = default;
  auto operator=(Mesh&& mv) -> Mesh& = default;

  std::string name;
  std::unordered_map<TextureType, gfx::ImageView> textures;
  gfx::Vector<Vertex> vertices;
  gfx::Vector<std::uint32_t> indices;

  private:
  friend auto load_model_file(std::string_view filename) -> ModelLoadInfo;
};

class Model {
public:
  // RAII Object.
  Model(const Model& cpy) = delete;
  auto operator=(const Model& cpy) -> Model& = delete;

  Model();
  Model(Model&& mv) = default;
  ~Model();

  inline auto name() const -> std::string_view {return this->m_name;}
  auto begin() {return this->m_meshes.begin();}
  auto end() {return this->m_meshes.end();}
  auto operator=(Model&& mv) -> Model& = default;
private:
  friend auto load_model_file(std::string_view filename) -> ModelLoadInfo;
  std::string m_name;
  std::vector<Mesh> m_meshes;
};

struct ModelLoadInfo {
  Model model;
  AnimationInfo animation;
};


}