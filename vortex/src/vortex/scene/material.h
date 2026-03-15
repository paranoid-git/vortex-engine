#pragma once
#include "vortex/rendering/shader.h"
#include "vortex/rendering/texture.h"
#include <memory>
#include <string>

struct Material {
  std::shared_ptr<Shader> shader;
  std::shared_ptr<Texture> diffuse;
  std::shared_ptr<Texture> normalMap;

  // tint and shininess
  glm::vec3 color = glm::vec3(1.0f);
  float shininess = 32.0f;

  void bind(int diffuseSlot = 0, int normalSlot = 1) const {
    if (diffuse)
      diffuse->bind(diffuseSlot);
    if (normalMap)
      normalMap->bind(normalSlot);
    if (shader) {
      shader->use();
      shader->setInt("tex", diffuseSlot);
      shader->setInt("normalMap", normalSlot);
      shader->setVec3("matColor", color);
      shader->setFloat("shininess", shininess);
    }
  }
};
