#pragma once
#include "vortex/model/mesh.h"
#include "vortex/rendering/shader.h"
#include "vortex/rendering/texture.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <memory>
#include <string>
#include <vector>

struct ModelMesh {
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Texture> diffuse;
  std::shared_ptr<Texture> normalMap;
};

class Model {
public:
  std::vector<ModelMesh> meshes;
  std::string directory;

  Model(const std::string &path);

  void draw(Shader &shader, int diffuseSlot = 0, int normalSlot = 1) const;

private:
  void processNode(aiNode *node, const aiScene *scene);
  ModelMesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::shared_ptr<Texture> loadTexture(aiMaterial *mat, aiTextureType type,
                                       const std::string &fallback = "");

  const aiScene *scene = nullptr; // store for embedded texture access
  Assimp::Importer importer;      // must be member — owns the scene lifetime
};
