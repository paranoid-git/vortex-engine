#include "vortex/matrix/camera.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 startPos) : position(startPos) { updateVectors(); }

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(CameraDirection dir, float dt) {
  float v = speed * dt;
  if (dir == CameraDirection::FORWARD)
    position += front * v;
  if (dir == CameraDirection::BACKWARD)
    position -= front * v;
  if (dir == CameraDirection::LEFT)
    position -= right * v;
  if (dir == CameraDirection::RIGHT)
    position += right * v;
  if (dir == CameraDirection::UP)
    position += up * v;
  if (dir == CameraDirection::DOWN)
    position -= up * v;
}

void Camera::processMouse(float xOff, float yOff) {
  yaw += xOff * sensitivity;
  pitch = std::clamp(pitch + yOff * sensitivity, -89.0f, 89.0f);
  updateVectors();
}

void Camera::processScroll(float yOff) {
  fov = std::clamp(fov - yOff, 1.0f, 90.0f);
}

void Camera::updateVectors() {
  glm::vec3 f;
  f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  f.y = sin(glm::radians(pitch));
  f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(f);
  right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
  up = glm::normalize(glm::cross(right, front));
}
