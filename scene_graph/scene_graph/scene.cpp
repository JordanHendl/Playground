#include "scene.hpp"
#include <queue>
#include <set>
#include <iostream>
#include <cassert>
#include <utility>
namespace luna {

auto Node::local_transform() -> mat4 {
  return luna::translate(mat4(1.0f), this->m_transform.position);// * luna::scale(mat4(1.0f), this->m_transform.scale);
}

auto Scene::add_node_type(std::string_view name) -> std::int32_t {
  static auto type_iter = 0;
  auto tmp = type_iter;
  this->m_type_map.insert({std::string(name), type_iter});
  type_iter += 1;
  return tmp;
}

auto Scene::add_data_type(std::string_view name) -> std::int32_t {
  static auto type_iter = 0;
  auto tmp = type_iter;
  this->m_data_map.insert({std::string(name), type_iter});
  type_iter += 1;
  return tmp;
}

auto Scene::add_child(std::string_view name, NodeInsertInfo insert_info, std::size_t parent_idx) -> std::size_t {
  auto idx = 0u;
  auto type_iter = this->m_type_map.find(insert_info.node_type);
  auto data_iter = this->m_data_map.find(insert_info.node_data);
  if(type_iter == this->m_type_map.end()) return 0u;
  auto data = (data_iter == this->m_data_map.end()) ? -1 : data_iter->second;
  
  // Need to search for an invalid spot in our vector before we grow it.
  for(auto& node : this->m_nodes) {
    // This means that we found an invalid node...
    if(node.m_type < 0) {
      node = Node(idx, parent_idx, type_iter->second, data);
      node.set_transform(insert_info.transform);
      this->m_name_map.insert({std::string(name), idx});
      this->m_nodes[parent_idx].m_children.push_back(idx);
      return idx;
    }
    idx++;
  }

  // We have no open slots, so we need to grow our vector.
  idx = this->m_nodes.size();
  this->m_nodes.push_back(Node(idx, parent_idx, type_iter->second, data));
  this->m_nodes.back().set_transform(insert_info.transform);
  this->m_nodes[parent_idx].m_children.push_back(this->m_nodes.size() - 1);
  this->m_name_map.insert({std::string(name), idx});
  return this->m_nodes.size() - 1;
}

auto Scene::remove(std::size_t idx) -> void {
  auto& node = this->m_nodes[idx];
  auto& parent = this->m_nodes[node.parent()];
  
  auto queue = std::queue<std::size_t>();
  auto visited = std::set<std::size_t>();
  
  // We need to traverse the node being removed and invalidate all children of it.
  queue.push(idx);
  while(!queue.empty()) {
    auto& node = this->m_nodes[queue.front()];
    for(auto child : node.m_children) {
      if(visited.find(child) == visited.end()) {
        queue.push(child);
      }
    }
    
    visited.insert(queue.front());
    node.invalidate();
    queue.pop();
  }

  // Now, we need to remove this node from the parent's children vector.
  for(auto i = 0u; i < parent.m_children.size(); i++) {
    if(parent.m_children[i] == node.idx()) {
      parent.m_children.erase(parent.m_children.begin() + i);
      node.invalidate();
      return;
    }
  }
}

auto Scene::remove(std::string_view name) -> void {
  auto iter = this->m_name_map.find(std::string(name));
  if(iter == this->m_name_map.end()) return;
  auto idx = iter->second;
  this->remove(idx);
  this->m_name_map.erase(iter); 
}

auto Scene::get(std::string_view name) -> std::optional<std::reference_wrapper<Node>> {
  auto iter = this->m_name_map.find(std::string(name));
  if(iter != this->m_name_map.end()) {
    return this->m_nodes[iter->second];
  }
  return {};
}

auto Scene::traverse(std::function<void(const Node&)> visit_func) -> void {
  constexpr auto cRootNode = 0;
  auto queue = std::queue<std::size_t>();
  auto visited = std::set<std::size_t>();
  auto calculate_transform = [](auto a, auto b) {
    auto t = b;
    t.transform = a.transform * b.transform;
  };

  // Start with the root node.
  queue.push(cRootNode);
  while(!queue.empty()) {
    auto& node = this->m_nodes[queue.front()];
    const auto& t = node.transform();
    for(auto child : node.m_children) {
      if(visited.find(child) == visited.end()) {
        auto out_transform = node.m_transform.transform * this->m_nodes[child].local_transform();
        this->m_nodes[child].m_transform.transform = out_transform;
        queue.push(child);
        
      }
    }
    
    // Mark that we've visited this node, and call the callback function.
    visited.insert(queue.front());
    if(node.idx() != 0 && visit_func) visit_func(node); 
    queue.pop();
  }
}

auto Scene::print() -> void {
  auto print_func = [&](const Node& n) {
    std::cout << "{" << n.idx() << ", " << n.parent() << ", " << n.type() << ", " << n.data() << "}\n";
    std::cout << "Children: {";
    if(n.m_children.empty()) std::cout << "}\n";
    for(auto& c : n.m_children) {
      if(c == n.m_children.back()) std::cout << c << "}\n";
      else std::cout << c << ", ";
    }

    const auto& t = n.transform().transform;
    std::cout << "Transform: " << std::endl;
    for(auto i = 0u; i < 4; i++) {
      for(auto j = 0u; j < 4; j++) {
        if(j == 3) std::cout << t[j][i] << "\n";
        else std::cout << t[j][i] << ", ";
      }
    }

    auto valid_str = n.valid() ? "valid" : "invalid";
    std::cout << valid_str << std::endl;
    std::cout << std::endl;
  };

  for(const auto& n : this->m_nodes) {
    print_func(n);
  }
}
}