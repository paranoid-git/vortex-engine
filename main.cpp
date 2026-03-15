

#include "vortex/model/mesh.h"
#include "vortex/rendering/cubemap.h"
#include "vortex/rendering/texture.h"
#include <stdio.h>
#include <vector>
#include <vortex/glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vortex/rendering/shader.h>

#include "vortex/matrix/camera.h"
#include "vortex/model/mesh.h"
#include "vortex/rendering/shadowMap.h"
#define windowHeight 600
#define windowWidth 800
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = windowWidth / 2.0f, lastY = windowHeight / 2.0f;
void computeTangents(std::vector<Vertex> &vertices,
                     const std::vector<unsigned int> &indices) {
  for (unsigned int i = 0; i < indices.size(); i += 3) {
    Vertex &v0 = vertices[indices[i]];
    Vertex &v1 = vertices[indices[i + 1]];
    Vertex &v2 = vertices[indices[i + 2]];

    glm::vec3 edge1 = v1.position - v0.position;
    glm::vec3 edge2 = v2.position - v0.position;
    glm::vec2 dUV1 = v1.uv - v0.uv;
    glm::vec2 dUV2 = v2.uv - v0.uv;

    float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

    glm::vec3 tangent;
    tangent.x = f * (dUV2.y * edge1.x - dUV1.y * edge2.x);
    tangent.y = f * (dUV2.y * edge1.y - dUV1.y * edge2.y);
    tangent.z = f * (dUV2.y * edge1.z - dUV1.y * edge2.z);
    tangent = glm::normalize(tangent);

    glm::vec3 bitangent;
    bitangent.x = f * (-dUV2.x * edge1.x + dUV1.x * edge2.x);
    bitangent.y = f * (-dUV2.x * edge1.y + dUV1.x * edge2.y);
    bitangent.z = f * (-dUV2.x * edge1.z + dUV1.x * edge2.z);
    bitangent = glm::normalize(bitangent);

    v0.tangent = v1.tangent = v2.tangent = tangent;
    v0.bitangent = v1.bitangent = v2.bitangent = bitangent;
  }
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  auto window =
      glfwCreateWindow(windowWidth, windowHeight, "Vortex", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  std::vector<Vertex> vertices = {
      // front face
      {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
      {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
      {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},

      // back face
      {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
      {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
      {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
      {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},

      // left face
      {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},

      // right face
      {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},

      // top face
      {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

      // bottom face
      {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
      {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
      {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
      {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
  };

  std::vector<unsigned int> indices = {
      0,  1,  2,  2,  3,  0,  // front
      4,  5,  6,  6,  7,  4,  // back
      8,  9,  10, 10, 11, 8,  // left
      12, 13, 14, 14, 15, 12, // right
      16, 17, 18, 18, 19, 16, // top
      20, 21, 22, 22, 23, 20, // bottom
  };
  computeTangents(vertices, indices);
  Mesh cube(vertices, indices);
  float skyboxVertices[] = {
      // right face (+X)
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,

      // left face (-X)
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,

      // top face (+Y)
      -1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      -1.0f,

      // bottom face (-Y)
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,

      // front face (+Z)
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,

      // back face (-Z)
      1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
  };

  Mesh skyboxMesh(PositionsOnly{}, skyboxVertices, 36);
  printf("skybox vertex count: %u \n", skyboxMesh.getVertexCount());

  std::vector<Vertex> planeVerts = {
      {{-5.0f, -0.5f, -5.0f}, {0.0f, 5.0f}, {0.0f, 1.0f, 0.0f}},
      {{5.0f, -0.5f, -5.0f}, {5.0f, 5.0f}, {0.0f, 1.0f, 0.0f}},
      {{5.0f, -0.5f, 5.0f}, {5.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
      {{-5.0f, -0.5f, 5.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
  };
  std::vector<unsigned int> planeIndices = {0, 1, 2, 2, 3, 0};
  computeTangents(planeVerts, planeIndices);
  Mesh floor(planeVerts, planeIndices);
  Shader shader("./resources/triangle.vert", "./resources/triangle.frag");
  Texture texture("./resources/brick.png");
  Texture normal("./resources/brickNormal.png");
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
  glm::vec3 lightPos(2.0f, 8.0f, 4.0f);

  Cubemap skybox({
      "./resources/skybox/px.png", // right
      "./resources/skybox/nx.png", // left
      "./resources/skybox/py.png", // top
      "./resources/skybox/ny.png", // bottom
      "./resources/skybox/pz.png", // front
      "./resources/skybox/nz.png", // back
  });
  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow *w, int width, int height) {
                                   glViewport(0, 0, width, height);
                                 });
  Shader skyboxShader("./resources/skybox.vert", "./resources/skybox.frag");

  ShadowMap shadowMap(1024, 1024);
  Shader shadowShader("./resources/shadow.vert", "./resources/shadow.frag");
  float debugQuad[] = {
      -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, 1.0f, 0.0f,
      -1.0f, 1.0f, 0.0f, 1.0f, -0.5f, 0.5f, 1.0f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
  };

  GLuint debugVAO, debugVBO;
  glGenVertexArrays(1, &debugVAO);
  glGenBuffers(1, &debugVBO);
  glBindVertexArray(debugVAO);
  glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(debugQuad), debugQuad, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);
  Shader debugShader("./resources/debug.vert", "./resources/debug.frag");

  while (!glfwWindowShouldClose(window)) {
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

    static bool spotlightOn = false;
    static bool fWasPressed = false;
    bool fPressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
    if (fPressed && !fWasPressed)
      spotlightOn = !spotlightOn;
    fWasPressed = fPressed;

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, now * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    glm::mat4 floorModel = glm::mat4(1.0f);
    glm::mat3 floorNormal = glm::transpose(glm::inverse(glm::mat3(floorModel)));

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.fov), (float)fbWidth / fbHeight, 0.1f, 100.0f);

    shadowMap.updateLightSpace(lightPos, glm::vec3(0.0f), 1.0f, 20.0f, 5.0f);

    // --- pass 1: shadow ---
    shadowMap.bindForWriting();
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    shadowShader.use();
    shadowShader.setMat4("lightSpaceMatrix", shadowMap.lightSpaceMatrix);
    shadowShader.setMat4("model", floorModel);
    floor.draw();
    shadowShader.setMat4("model", model);
    cube.draw();
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // --- reset viewport ---
    glViewport(0, 0, fbWidth, fbHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // --- skybox ---
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    skyboxShader.use();
    skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.getViewMatrix())));
    skyboxShader.setMat4("projection", projection);
    skyboxShader.setInt("skybox", 1);
    skybox.bind(1);
    skyboxMesh.draw();
    glDepthMask(GL_TRUE);

    // --- set ALL uniforms before ANY draw calls ---
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setMat4("lightSpaceMatrix", shadowMap.lightSpaceMatrix);
    shader.setVec3("viewPos", camera.position);
    shader.setInt("tex", 0);
    shader.setInt("normalMap", 1);
    shader.setInt("shadowMap", 2);

    shader.setVec3("dirLight.direction",
                   glm::normalize(glm::vec3(0.0f) - lightPos));
    shader.setVec3("dirLight.ambient", glm::vec3(0.3f));
    shader.setVec3("dirLight.diffuse", glm::vec3(0.8f));
    shader.setVec3("dirLight.specular", glm::vec3(1.0f));

    glm::vec3 pointLightPositions[] = {
        {2.0f, 2.0f, 2.0f},
        {-2.0f, 1.0f, -2.0f},
    };
    shader.setInt("numPointLights", 2);
    for (int i = 0; i < 2; i++) {
      std::string b = "pointLights[" + std::to_string(i) + "].";
      shader.setVec3(b + "position", pointLightPositions[i]);
      shader.setFloat(b + "constant", 1.0f);
      shader.setFloat(b + "linear", 0.09f);
      shader.setFloat(b + "quadratic", 0.032f);
      shader.setVec3(b + "ambient", glm::vec3(0.2f));
      shader.setVec3(b + "diffuse", glm::vec3(0.8f));
      shader.setVec3(b + "specular", glm::vec3(1.0f));
    }

    shader.setBool("useSpotLight", spotlightOn);
    shader.setVec3("spotLight.position", camera.position);
    shader.setVec3("spotLight.direction", camera.front);
    shader.setVec3("spotLight.ambient", glm::vec3(0.0f));
    shader.setVec3("spotLight.diffuse", glm::vec3(1.0f));
    shader.setVec3("spotLight.specular", glm::vec3(1.0f));
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

    texture.bind(0);
    normal.bind(1);
    shadowMap.bindForReading(2);

    // --- draw floor ---
    glDisable(GL_CULL_FACE);
    shader.setMat4("model", floorModel);
    shader.setMat3("normalMatrix", floorNormal);
    floor.draw();

    // --- draw cube ---
    glEnable(GL_CULL_FACE);
    shader.setMat4("model", model);
    shader.setMat3("normalMatrix",
                   normalMatrix); // lowercase n — matches shader
    cube.draw();

    glDisable(GL_DEPTH_TEST);
    debugShader.use();
    debugShader.setInt("depthMap", 2);
    shadowMap.bindForReading(2);
    glBindVertexArray(debugVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  printf("Hello, Vortex!");
  return 0;
}
