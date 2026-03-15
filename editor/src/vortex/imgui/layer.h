#pragma once
#include <GLFW/glfw3.h>
class Layer {
public:
  Layer(GLFWwindow *window);
  ~Layer();

  void begin(); // call at start of frame
  void end();   // call at end of frame, before glfwSwapBuffers

  bool wantsMouse() const;
  bool wantsKeyboard() const;
};
