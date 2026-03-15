#pragma once
#include "vortex/model/mesh.h"
#include "vortex/scene/material.h"
#include "vortex/scene/transform.h"
#include <memory>
#include <string>
#include <vector>
#include <vortex/model/model.h>
class GameObject {
public:
  std::string name;
  Transform transform;
  Material material;
  bool visible = true;
  bool selected = false;

  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Model> model; // for loaded models

  // children for hierarchy
  std::vector<std::shared_ptr<GameObject>> children;
  GameObject *parent = nullptr;

  GameObject(const std::string &name = "GameObject") : name(name) {}

  void addChild(std::shared_ptr<GameObject> child) {
    child->parent = this;
    children.push_back(child);
  }

  // world space matrix — multiplies up through parents
  glm::mat4 getWorldMatrix() const {
    if (parent)
      return parent->getWorldMatrix() * transform.getMatrix();
    return transform.getMatrix();
  }

  glm::mat3 getWorldNormalMatrix() const {
    return glm::transpose(glm::inverse(glm::mat3(getWorldMatrix())));
  }

  void draw(Shader &shader) const {
    printf("GameObject::draw %s visible=%d\n", name.c_str(), visible);
    if (!visible)
      return;
    if (!mesh && !model) {
      printf("  skipping — no mesh or model\n");
      return;
    }
    shader.setMat4("model", getWorldMatrix());
    shader.setMat3("normalMatrix", getWorldNormalMatrix());
    if (model) {
      printf("  drawing model with %zu meshes\n", model->meshes.size());
      model->draw(shader);
    } else if (mesh) {
      printf("  drawing mesh\n");
      mesh->draw();
    }
    for (auto &child : children)
      child->draw(shader);
  }

  // shadow pass — just needs model matrix
  void drawShadow(Shader &shadowShader) const {
    if (!visible || !mesh)
      return;

    shadowShader.setMat4("model", getWorldMatrix());
    if (model) {
      for (auto &m : model->meshes)
        m.mesh->draw();
    } else if (mesh) {
      mesh->draw();
    }
    for (auto &child : children)
      child->drawShadow(shadowShader);
  }
};
