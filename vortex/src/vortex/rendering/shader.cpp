#include "vortex/rendering/shader.h"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

Shader::Shader(const char *vertPath, const char *fragPath) {
  // 1. Read source files from disk
  std::string vertCode, fragCode;
  std::ifstream vFile(vertPath), fFile(fragPath);

  if (!vFile.is_open() || !fFile.is_open()) {
    std::cerr << "ERROR: Could not open shader files\n";
    return;
  }

  std::stringstream vStream, fStream;
  vStream << vFile.rdbuf();
  fStream << fFile.rdbuf();
  vertCode = vStream.str();
  fragCode = fStream.str();

  const char *vSrc = vertCode.c_str();
  const char *fSrc = fragCode.c_str();

  // 2. Compile vertex shader
  unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert, 1, &vSrc, nullptr);
  glCompileShader(vert);
  checkCompileErrors(vert, "VERTEX");

  // 3. Compile fragment shader
  unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag, 1, &fSrc, nullptr);
  glCompileShader(frag);
  checkCompileErrors(frag, "FRAGMENT");

  // 4. Link into a program
  ID = glCreateProgram();
  glAttachShader(ID, vert);
  glAttachShader(ID, frag);
  glLinkProgram(ID);
  checkCompileErrors(ID, "PROGRAM");

  // 5. Individual shader objects no longer needed
  glDeleteShader(vert);
  glDeleteShader(frag);
}

void Shader::setBool(const std::string &name, bool v) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)v);
}
void Shader::setInt(const std::string &name, int v) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), v);
}
void Shader::setFloat(const std::string &name, float v) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), v);
}
void Shader::setVec3(const std::string &name, const glm::vec3 &v) const {
  glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(v));
}
void Shader::setMat3(const std::string &name, const glm::mat3 &v) const {
  glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(v));
}
void Shader::setMat4(const std::string &name, const glm::mat4 &v) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(v));
}

void Shader::checkCompileErrors(unsigned int shader, const std::string &type) {
  int success;
  char log[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, nullptr, log);
      std::cerr << "SHADER COMPILE ERROR [" << type << "]:\n" << log << "\n";
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, nullptr, log);
      std::cerr << "PROGRAM LINK ERROR:\n" << log << "\n";
    }
  }
}
