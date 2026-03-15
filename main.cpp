

#include "vortex/rendering/texture.h"
#include <stdio.h>
#include <vortex/glad/glad.h>

#include <GLFW/glfw3.h>
#include <vortex/rendering/shader.h>
int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  auto window = glfwCreateWindow(800, 600, "Vortex", nullptr, nullptr);
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
  shader.use();
  shader.setInt("tex", 0);
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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
