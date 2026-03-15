

#include "vortex/rendering/texture.h"
#include <stdio.h>
#include <vortex/glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vortex/rendering/shader.h>

#include "vortex/matrix/camera.h"
#define windowHeight 600
#define windowWidth 800
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = windowWidth / 2.0f, lastY = windowHeight / 2.0f;

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  auto window =
      glfwCreateWindow(windowWidth, windowHeight, "Vortex", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  gladLoadGL();

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  float vertices[]{
      //   pos X   pos Y    U     V
      -0.75f, -0.75f, 0.0f, 0.0f,  0.75f, -0.75f,
      1.0f,   0.0f,   0.0f, 0.75f, 0.5f,  1.0f,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  Shader shader("./resources/triangle.vert", "./resources/triangle.frag");
  Texture texture("./resources/obama.jpg");
  // projection only needs updating on window resize — but fine to set each
  // frame
  glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), (float)windowWidth / (float)windowHeight,
      0.1f,  // near plane
      100.0f // far plane
  );

  // capture mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // mouse callback
  glfwSetCursorPosCallback(window, [](GLFWwindow *w, double x, double y) {
    // use a global or glfwSetWindowUserPointer to reach your camera
    if (firstMouse) {
      lastX = x;
      lastY = y;
      firstMouse = false;
    }
    camera.processMouse(x - lastX, lastY - y); // y inverted
    lastX = x;
    lastY = y;
  });
  // model: where the object sits in the world

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glm::mat4 model = glm::mat4(1.0f);
    shader.use();

    shader.setInt("tex", 0);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
    model =
        glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));

    float now = glfwGetTime();
    float deltaTime = now - lastFrame;
    lastFrame = now;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      camera.processKeyboard(CameraDirection::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      camera.processKeyboard(CameraDirection::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      camera.processKeyboard(CameraDirection::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      camera.processKeyboard(CameraDirection::RIGHT, deltaTime);

    // build view & projection using camera
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.fov),
                         (float)windowWidth / windowHeight, 0.1f, 100.0f);
    shader.setMat4("model", model);
    shader.setMat4("view", view); // from camera (next step)
    shader.setMat4("projection", projection);
    texture.bind(0);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  printf("Hello, Vortex!");
  return 0;
}
