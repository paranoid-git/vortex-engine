#include "vortex/rendering/texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <vortex/stb_image.h>

Texture::Texture(const char *path, bool flipVertically) {
  glGenTextures(1, &ID);
  glBindTexture(GL_TEXTURE_2D, ID);

  // wrapping & filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_set_flip_vertically_on_load(flipVertically);
  int width, height, channels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(path, &width, &height, &channels, 0);

  if (data) {
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    std::cout << "Loaded " << path << " (" << width << "x" << height << ", "
              << channels << " channels)\n";
  } else {
    std::cerr << "Failed to load texture: " << path << "\n";
  }

  stbi_image_free(data);
}

void Texture::bind(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, ID);
}
