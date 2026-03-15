#pragma once
#include <glm/glm.hpp>

enum class CameraDirection { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

class Camera {
public:
  glm::vec3 position;
  float yaw = -90.0f; // facing -Z by default
  float pitch = 0.0f;
  float fov = 45.0f;
  float speed = 5.0f;
  float sensitivity = 0.1f;

  Camera(glm::vec3 startPos = glm::vec3(0.0f, 0.0f, 3.0f));

  glm::mat4 getViewMatrix() const;
  void processKeyboard(CameraDirection dir, float deltaTime);
  void processMouse(float xOffset, float yOffset);
  void processScroll(float yOffset);
  glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);

private:
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 right;
  void updateVectors();
};
