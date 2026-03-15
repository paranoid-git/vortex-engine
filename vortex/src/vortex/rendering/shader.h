#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vortex/glad/glad.h>

class Shader {
public:
  unsigned int ID;

  Shader(const char *vertPath, const char *fragPath);
  ~Shader() { glDeleteProgram(ID); }

  void use() { glUseProgram(ID); }

  void setBool(const std::string &name, bool v) const;
  void setInt(const std::string &name, int v) const;
  void setFloat(const std::string &name, float v) const;
  void setVec3(const std::string &name, const glm::vec3 &v) const;
  void setMat3(const std::string &name, const glm::mat3 &v) const;
  void setMat4(const std::string &name, const glm::mat4 &v) const;

private:
  void checkCompileErrors(unsigned int shader, const std::string &type);
};
