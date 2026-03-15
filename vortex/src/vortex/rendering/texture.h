#pragma once
#include <string>
#include <vortex/glad/glad.h>

class Texture {
public:
  unsigned int ID;
  Texture() = default;
  Texture(const char *path, bool flipVertically = true);
  void bind(unsigned int slot = 0) const;
};
