#include "vortex/model/model.h"
#include <stdio.h>
#include <vortex/stb_image.h>
Model::Model(const std::string &path) {
  scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                      aiProcess_CalcTangentSpace |
                                      aiProcess_GenSmoothNormals |
                                      aiProcess_JoinIdenticalVertices);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    printf("Model load error: %s\n", importer.GetErrorString());
    return;
  }

  directory = path.substr(0, path.find_last_of('/'));
  processNode(scene->mRootNode, scene);
  printf("Loaded model: %s (%zu meshes)\n", path.c_str(), meshes.size());
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
  }
  for (unsigned int i = 0; i < node->mNumChildren; i++)
    processNode(node->mChildren[i], scene);
}

ModelMesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex v;
    v.position = {mesh->mVertices[i].x, mesh->mVertices[i].y,
                  mesh->mVertices[i].z};
    if (mesh->HasNormals())
      v.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y,
                  mesh->mNormals[i].z};
    if (mesh->mTextureCoords[0])
      v.uv = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
    if (mesh->HasTangentsAndBitangents()) {
      v.tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y,
                   mesh->mTangents[i].z};
      v.bitangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y,
                     mesh->mBitangents[i].z};
    }
    vertices.push_back(v);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace &face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  ModelMesh result;
  result.mesh = std::make_shared<Mesh>(vertices, indices);

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
    result.diffuse = loadTexture(mat, aiTextureType_DIFFUSE);
    result.normalMap = loadTexture(mat, aiTextureType_NORMALS);
    // some formats use HEIGHT for normal maps
    if (!result.normalMap)
      result.normalMap = loadTexture(mat, aiTextureType_HEIGHT);
  }
  glm::vec3 minP = vertices[0].position;
  glm::vec3 maxP = vertices[0].position;
  for (auto &v : vertices) {
    minP = glm::min(minP, v.position);
    maxP = glm::max(maxP, v.position);
  }
  printf("mesh bounds: (%.2f %.2f %.2f) to (%.2f %.2f %.2f)\n", minP.x, minP.y,
         minP.z, maxP.x, maxP.y, maxP.z);
  return result;
}

std::shared_ptr<Texture> Model::loadTexture(aiMaterial *mat, aiTextureType type,
                                            const std::string &fallback) {
  if (mat->GetTextureCount(type) == 0) {
    if (!fallback.empty())
      return std::make_shared<Texture>(fallback.c_str());
    return nullptr;
  }

  aiString str;
  mat->GetTexture(type, 0, &str);
  if (str.C_Str()[0] == '*') {
    int index = atoi(str.C_Str() + 1); // skip the '*'
    const aiTexture *tex = scene->mTextures[index];

    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    if (tex->mHeight == 0) {
      // compressed format (png/jpg) — decode with stb
      int w, h, channels;
      stbi_set_flip_vertically_on_load(true);
      unsigned char *data = stbi_load_from_memory(
          (unsigned char *)tex->pcData, tex->mWidth, &w, &h, &channels, 0);
      if (data) {
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
      } else {
        printf("Failed to decode embedded texture %d\n", index);
      }
    } else {
      // raw ARGB8888
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->mWidth, tex->mHeight, 0,
                   GL_BGRA, GL_UNSIGNED_BYTE, tex->pcData);
      glGenerateMipmap(GL_TEXTURE_2D);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // wrap the raw GL id in a Texture object
    auto t = std::make_shared<Texture>();
    t->ID = id;
    return t;
  }

  // external texture file
  std::string path = directory + "/" + str.C_Str();
  return std::make_shared<Texture>(path.c_str());
}

void Model::draw(Shader &shader, int diffuseSlot, int normalSlot) const {
  for (auto &m : meshes) {
    if (m.diffuse)
      m.diffuse->bind(diffuseSlot);
    if (m.normalMap)
      m.normalMap->bind(normalSlot);
    shader.setInt("tex", diffuseSlot);
    shader.setInt("normalMap", normalSlot);
    m.mesh->draw();
  }
}
