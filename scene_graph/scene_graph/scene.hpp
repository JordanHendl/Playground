#pragma once
#include "luna-gfx/ext/math.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <optional>
namespace luna {
class Node;

struct Transformation {
  vec3 position = {0, 0, 0};
  vec3 rotation = {0, 0, 0};
  vec3 scale = {1, 1, 1};
  mat4 transform;
};

struct NodeInsertInfo {
  std::string node_type;
  std::string node_data;
  Transformation transform;
};

class Node {
public:
  inline auto set_parent(std::size_t idx) -> void {this->m_parent = idx;}
  inline auto set_transform(Transformation t) -> void {this->m_transform = t;}
  inline auto parent() const -> const std::size_t {return this->m_parent;}
  inline auto transform() const -> const Transformation& {return this->m_transform;}
  inline auto transform() -> Transformation& {return this->m_transform;}
  inline auto type() const -> std::int32_t {return this->m_type;}
  inline auto data() const -> std::int32_t {return this->m_data;}
  inline auto idx() const -> std::size_t {return this->m_idx;}
private:
  friend class Scene;
  auto local_transform() -> mat4;
  auto invalidate() -> void {this->m_children.clear(); this->m_type = -1;}
  auto valid() const -> bool {return this->m_type >= 0;}
  Node() : m_idx(0), m_parent(0), m_type(-1), m_data(-1) {};
  Node(std::size_t idx, std::size_t parent_idx, std::int32_t type, std::int32_t data) : m_idx(idx), m_parent(parent_idx), m_type(type), m_data(data) {}

  Transformation m_transform;
  std::size_t m_idx;
  std::size_t m_parent;
  std::int32_t m_data;
  std::int32_t m_type;
  std::vector<std::size_t> m_children;
};

class Scene {
public:
  Scene() {this->m_nodes.push_back({0, 0, 0, 0});}
  ~Scene() = default;
  auto traverse(std::function<void(const Node&)> visit_func = {}) -> void;
  auto add_node_type(std::string_view name) -> std::int32_t;
  auto add_data_type(std::string_view name) -> std::int32_t;
  auto add_child(std::string_view name, NodeInsertInfo insert_info, std::size_t parent_idx = 0) -> std::size_t;
  auto get(std::string_view name) -> std::optional<std::reference_wrapper<Node>>;
  auto get(std::size_t index) -> Node& {return this->m_nodes[index];}
  auto remove(std::string_view name) -> void;
  auto remove(std::size_t index) -> void;
  auto parent() const -> const Node& {return this->m_nodes[0];}
  auto print() -> void;
private:
  std::unordered_map<std::string, std::size_t> m_name_map;
  std::unordered_map<std::string, std::int32_t> m_data_map;
  std::unordered_map<std::string, std::int32_t> m_type_map;
  std::vector<Node> m_nodes;
};
}