#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <vortex/glad/glad.h>

struct Vertex {
  glm::vec3 position;
  glm::vec2 uv;
  glm::vec3 normal = glm::vec3(0.0f);
  glm::vec3 tangent = glm::vec3(0.0f);   // add this
  glm::vec3 bitangent = glm::vec3(0.0f); // add this
};
struct PositionsOnly {}; // empty tag, just used to pick the right constructor

class Mesh {
public:
  // from structured vertices

  Mesh(const std::vector<Vertex> &vertices);
  Mesh(const std::vector<Vertex> &vertices,
       const std::vector<unsigned int> &indices);

  // from raw float array (your current style, easy to migrate)
  Mesh(const float *data, unsigned int vertexCount, bool hasUVs = true,
       bool hasNormals = false);
  Mesh(PositionsOnly, const float *positions, unsigned int vertexCount);
  ~Mesh();

  void draw() const;
  void updateVertices(
      const std::vector<Vertex> &vertices); // live update, useful for editor

  unsigned int getVertexCount() const { return vertexCount; }
  unsigned int getIndexCount() const { return indexCount; }

private:
  GLuint VAO = 0, VBO = 0, EBO = 0;
  unsigned int vertexCount = 0;
  unsigned int indexCount = 0;
  bool indexed = false;

  void setupFromVertices(const std::vector<Vertex> &vertices);
  void setupAttribs();
};
