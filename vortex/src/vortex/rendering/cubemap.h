#pragma once
#include <string>
#include <vector>
#include <vortex/glad/glad.h>

class Cubemap {
public:
  unsigned int ID;
  // faces order: right, left, top, bottom, front, back
  Cubemap(const std::vector<std::string> &faces);
  void bind(unsigned int slot = 0) const;
};
