#pragma once
#include <vortex/glad/glad.h>

class Framebuffer {
public:
  unsigned int FBO;
  unsigned int colorTexture;
  unsigned int depthRBO;
  int width, height;

  Framebuffer(int width, int height);
  ~Framebuffer();

  void bind();   // render into this framebuffer
  void unbind(); // render back to screen
  void resize(int width, int height);
};
