#pragma once
#include "vortex/rendering/shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vortex/glad/glad.h>

class ShadowMap {
public:
  unsigned int depthMapFBO;
  unsigned int depthMap;
  unsigned int width, height;
  glm::mat4 lightSpaceMatrix;

  ShadowMap(unsigned int width = 1024, unsigned int height = 1024);
  ~ShadowMap();

  void bindForWriting();                        // call before shadow pass
  void bindForReading(unsigned int slot) const; // call before main pass
  void updateLightSpace(glm::vec3 lightPos, glm::vec3 target = glm::vec3(0.0f),
                        float nearPlane = 1.0f, float farPlane = 25.0f,
                        float orthoSize = 10.0f);
};
