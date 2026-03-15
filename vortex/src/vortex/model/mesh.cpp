#include "vortex/model/mesh.h"
#include <stdio.h>

Mesh::Mesh(const std::vector<Vertex> &vertices) { setupFromVertices(vertices); }

Mesh::Mesh(const std::vector<Vertex> &vertices,
           const std::vector<unsigned int> &indices) {
  vertexCount = vertices.size();
  indexCount = indices.size();
  indexed = true;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  setupAttribs();
}

Mesh::Mesh(const float *data, unsigned int vertexCount, bool hasUVs,
           bool hasNormals) {
  this->vertexCount = vertexCount;
  indexed = false;

  // convert raw floats into Vertex structs
  std::vector<Vertex> vertices(vertexCount);
  int stride = 3 + (hasUVs ? 2 : 0) + (hasNormals ? 3 : 0);

  for (unsigned int i = 0; i < vertexCount; i++) {
    const float *base = data + i * stride;
    vertices[i].position = {base[0], base[1], base[2]};
    if (hasUVs)
      vertices[i].uv = {base[3], base[4]};
    if (hasNormals)
      vertices[i].normal = {base[hasUVs ? 5 : 3], base[hasUVs ? 6 : 4],
                            base[hasUVs ? 7 : 5]};
  }

  setupFromVertices(vertices);
}
Mesh::Mesh(PositionsOnly, const float *positions, unsigned int vertexCount) {

  this->vertexCount = vertexCount;
  indexed = false;
  printf("PositionsOnly ctor, count=%d\n", vertexCount);
  printf("first 9 floats: %f %f %f %f %f %f %f %f %f\n", positions[0],
         positions[1], positions[2], positions[3], positions[4], positions[5],
         positions[6], positions[7], positions[8]);
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), positions,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
}

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  if (indexed)
    glDeleteBuffers(1, &EBO);
}

void Mesh::draw() const {
  glBindVertexArray(VAO);
  if (indexed)
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
  else
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
  glBindVertexArray(0);
}

void Mesh::updateVertices(const std::vector<Vertex> &vertices) {
  vertexCount = vertices.size();
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(),
               GL_DYNAMIC_DRAW); // dynamic since it's being updated
}

void Mesh::setupFromVertices(const std::vector<Vertex> &vertices) {
  vertexCount = vertices.size();
  indexed = false;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  setupAttribs();
}

void Mesh::setupAttribs() {
  // position (location 0)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);

  // uv (location 1)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, uv));
  glEnableVertexAttribArray(1);

  // normal (location 2)
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
  glEnableVertexAttribArray(2);

  // tangent (location 3)
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, tangent));
  glEnableVertexAttribArray(3);

  // bitangent (location 4)
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, bitangent));
  glEnableVertexAttribArray(4);
}
