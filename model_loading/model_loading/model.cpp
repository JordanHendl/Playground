#include "model_loading/model.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <algorithm>
#include <iterator>
#include <cassert>
namespace luna {
constexpr auto cGPU = 0;

inline auto process(const aiMesh* mesh, const aiScene* scene) -> Mesh {
  return {};
}

inline auto process(const aiNode* root, const aiScene* scene) -> std::vector<Mesh> {
  auto ret = std::vector<Mesh>();

  const auto* node = root;

  for(auto index = 0u; index < node->mNumMeshes; index++) {
    ret.push_back(process(scene->mMeshes[node->mMeshes[index]], scene));
  }

  for(auto index = 0u; index < node->mNumChildren; index++) {
    auto r = process(node->mChildren[index], scene);
    ret.insert(std::end(ret), std::make_move_iterator(std::begin(r)), std::make_move_iterator(std::end(r)));
  }
  return ret;
}

Model::Model(std::string_view path) {
  constexpr auto flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                         aiProcess_FlipUVs | aiProcess_CalcTangentSpace;

  auto importer = Assimp::Importer();
  const auto* scene = importer.ReadFile(path.data(), flags);

  assert(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode);
  this->m_meshes = process(scene->mRootNode, scene);
}

Model::~Model() {

}
}
