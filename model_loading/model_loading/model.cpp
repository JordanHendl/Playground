#include "model_loading/model.hpp"
#include "model_loading/data_manager.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <map>

#include <iostream>
namespace luna {
constexpr auto cGPU = 0;

auto convert(aiTextureType type) -> TextureType {
  switch(type) {
    case aiTextureType_DIFFUSE : return TextureType::Diffuse;
    case aiTextureType_AMBIENT : return TextureType::Ambient;
    case aiTextureType_SPECULAR : return TextureType::Specular;
    case aiTextureType_HEIGHT : return TextureType::Height;
    case aiTextureType_BASE_COLOR : return TextureType::BaseColor;
    default: return TextureType::Unknown;
  }
}

auto clean_up_path(std::string in) -> std::string {
    // Remove extension.
    auto p = in.find_last_of(".");
    auto no_extension = in.substr(0, p);

    // Get base path
    p = no_extension.find_last_of("/\\");
    return no_extension.substr(p + 1, no_extension.size());
}

template<typename some_map>
auto add_materials(aiMaterial* mat, aiTextureType type, some_map& map) -> void {
  for(auto idx = 0u; idx < mat->GetTextureCount(type); ++idx) {
    auto str = aiString();
    mat->GetTexture(type, idx, &str);

    auto item = clean_up_path(std::string(str.C_Str()));

    auto ptr = luna::db::image(item);
    if(ptr) {
      auto t = convert(type);
      auto iter = map.find(t);
      if(!map.empty())
        assert(iter != map.end());
      map[t] = ptr;
    }
  }
}

inline auto process(const aiMesh* mesh, const aiScene* scene) -> Mesh {
  constexpr auto cNumVertsPerFace = 3;
  auto out = Mesh();
  auto vertices = std::vector<Vertex>();
  auto indices = std::vector<std::uint32_t>();

  vertices.reserve(mesh->mNumVertices);
  indices.reserve(mesh->mNumFaces * cNumVertsPerFace);
  
  for(auto idx = 0u; idx < mesh->mNumVertices; ++idx) {
    auto vertex = Vertex();
    vertex.position = {mesh->mVertices[idx].x, mesh->mVertices[idx].y, mesh->mVertices[idx].z};
    if(mesh->HasNormals()) vertex.normal = {mesh->mNormals[idx].x, mesh->mNormals[idx].y, mesh->mNormals[idx].z};
    if(mesh->mTextureCoords[0]) vertex.tex_coords = {mesh->mTextureCoords[0][idx].x, mesh->mTextureCoords[0][idx].y};
    if(mesh->mTangents) vertex.tangent = {mesh->mTangents[idx].x, mesh->mTangents[idx].y, mesh->mTangents[idx].z};
    if(mesh->mBitangents) vertex.bitangent = {mesh->mBitangents[idx].x, mesh->mBitangents[idx].y, mesh->mBitangents[idx].z};
    vertices.push_back(vertex);
  }
  
  for(auto idx = 0u; idx < mesh->mNumFaces; ++idx) {
    auto face = mesh->mFaces[idx];
    for(auto idx2 = 0u; idx2 < face.mNumIndices; ++idx2) {
      indices.push_back(face.mIndices[idx2]);
    }
  }

  auto material = scene->mMaterials[mesh->mMaterialIndex];
  add_materials(material, aiTextureType_DIFFUSE, out.textures);
  add_materials(material, aiTextureType_AMBIENT, out.textures);
  add_materials(material, aiTextureType_HEIGHT, out.textures);
  add_materials(material, aiTextureType_SPECULAR, out.textures);
  add_materials(material, aiTextureType_EMISSIVE, out.textures);
  add_materials(material, aiTextureType_NORMALS, out.textures);
  add_materials(material, aiTextureType_SHININESS, out.textures);
  add_materials(material, aiTextureType_OPACITY, out.textures);
  add_materials(material, aiTextureType_BASE_COLOR, out.textures);

  // Bone processing. Applying bone values to vertices. 
  for(auto idx = 0u; idx < mesh->mNumBones; ++idx) {
    auto* bone = mesh->mBones[idx];
    auto bone_name = std::string(bone->mName.C_Str());
    for(auto idx2 = 0u; idx2 < bone->mNumWeights; ++idx2) {
      auto& weight = bone->mWeights[idx2];
      auto vertex_id = weight.mVertexId;
      auto& vertex = vertices[vertex_id];
      
      for(auto b = 0u; b < cMaxBones; ++b) {
        if(vertex.bone_weights[b] == 0.0f) {
          vertex.bone_weights[b] = weight.mWeight;
          vertex.bone_ids[b] = idx;
          break;
        }
      }
    }
  }

  // Create and copy data to GPU.
  out.name = mesh->mName.C_Str();
  out.vertices = gfx::Vector<Vertex>(cGPU, vertices.size());
  out.indices = gfx::Vector<std::uint32_t>(cGPU, indices.size());
  out.vertices.upload(vertices.data());
  out.indices.upload(indices.data());

  return out;
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

Model::Model() {}
Model::~Model() {}

auto load_model_file(std::string_view filename) -> std::shared_ptr<Model> {

  auto model = std::shared_ptr<Model>(new Model());
  constexpr auto flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                         aiProcess_FlipUVs | aiProcess_CalcTangentSpace;

  auto importer = Assimp::Importer();
  const auto* scene = importer.ReadFile(filename.data(), flags);
  assert(scene != nullptr);
  assert(!(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE));
  assert(scene->mRootNode != nullptr);
  model->m_name = scene->mRootNode->mName.C_Str();
  model->m_meshes = process(scene->mRootNode, scene);
  return model;
}
}