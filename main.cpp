

#include <stdio.h>
#include <vortex/glad/glad.h>

#include <GLFW/glfw3.h>
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
      -0.75f, -0.75f, 0.75f, -0.75f, 0.f, 0.75f,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        (const void *)0);
  glEnableVertexAttribArray(0);

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  const char *vertexSource = R"(
        #version 460
        in vec2 vp;
        void main() {
            gl_Position = vec4(vp, 0, 1);
        }
    )";
  glShaderSource(vs, 1, &vertexSource, nullptr);
  glCompileShader(vs);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  const char *fragSource = R"(
        #version 460
        out vec4 color;
        void main() {
            color = vec4(1, 0, 0, 1);
        }
    )";
  glShaderSource(fs, 1, &fragSource, nullptr);
  glCompileShader(fs);

  GLuint sp = glCreateProgram();
  glAttachShader(sp, vs);
  glAttachShader(sp, fs);
  glLinkProgram(sp);

  while (!glfwWindowShouldClose(window)) {
    glUseProgram(sp);
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
