#pragma once
#include "vortex/rendering/shader.h"
#include "vortex/scene/gameObject.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

class Scene {
public:
  std::vector<std::shared_ptr<GameObject>> objects;
  std::shared_ptr<GameObject> selected;

  void add(std::shared_ptr<GameObject> obj) { objects.push_back(obj); }

  void remove(const std::string &name) {
    objects.erase(std::remove_if(objects.begin(), objects.end(),
                                 [&](const std::shared_ptr<GameObject> &o) {
                                   return o->name == name;
                                 }),
                  objects.end());
  }

  std::shared_ptr<GameObject> find(const std::string &name) {
    for (auto &o : objects)
      if (o->name == name)
        return o;
    return nullptr;
  }

  void draw(Shader &shader) const {
    for (auto &obj : objects)
      obj->draw(shader);
  }

  void drawShadow(Shader &shadowShader) const {
    for (auto &obj : objects)
      obj->drawShadow(shadowShader);
  }
};
