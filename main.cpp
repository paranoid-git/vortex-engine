

#include "imgui_impl_glfw.h"
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

#include "vortex/imgui/layer.h"
#include "vortex/matrix/camera.h"
#include "vortex/model/mesh.h"
#include "vortex/rendering/shadowMap.h"
#include "vortex/scene/scene.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <vortex/imgui/framebuffer.h>
#define windowHeight 600
#define windowWidth 800
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastFrame = 0.0f;
bool firstMouse = true;
bool cursorLocked = true;
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

  Layer imguiLayer(window);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  std::vector<Vertex> vertices = {
      {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
      {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
      {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
      {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
      {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
      {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
      {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
      {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
      {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
      {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
      {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
  };
  std::vector<unsigned int> indices = {
      0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
      12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20,
  };
  computeTangents(vertices, indices);
  Scene scene;

  auto cubeObj = std::make_shared<GameObject>("Cube");
  cubeObj->mesh = std::make_shared<Mesh>(vertices, indices);
  cubeObj->material.shader = std::make_shared<Shader>(
      "./resources/triangle.vert", "./resources/triangle.frag");
  cubeObj->material.diffuse =
      std::make_shared<Texture>("./resources/brick.png");
  cubeObj->material.normalMap =
      std::make_shared<Texture>("./resources/brickNormal.png");
  cubeObj->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
  scene.add(cubeObj);

  float skyboxVertices[] = {
      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,
      -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,
      1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,
      1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
      1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,
  };
  Mesh skyboxMesh(PositionsOnly{}, skyboxVertices, 36);

  std::vector<Vertex> planeVerts = {
      {{-5.0f, -0.5f, -5.0f}, {0.0f, 5.0f}, {0.0f, 1.0f, 0.0f}},
      {{5.0f, -0.5f, -5.0f}, {5.0f, 5.0f}, {0.0f, 1.0f, 0.0f}},
      {{5.0f, -0.5f, 5.0f}, {5.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
      {{-5.0f, -0.5f, 5.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
  };
  std::vector<unsigned int> planeIndices = {0, 1, 2, 2, 3, 0};
  computeTangents(planeVerts, planeIndices);

  auto floorObj = std::make_shared<GameObject>("Floor");
  floorObj->mesh = std::make_shared<Mesh>(planeVerts, planeIndices);
  floorObj->material.shader = cubeObj->material.shader;   // share shader
  floorObj->material.diffuse = cubeObj->material.diffuse; // share texture
  floorObj->material.normalMap = cubeObj->material.normalMap;
  floorObj->transform.position = glm::vec3(0.0f);
  scene.add(floorObj);

  Shader skyboxShader("./resources/skybox.vert", "./resources/skybox.frag");
  Shader shadowShader("./resources/shadow.vert", "./resources/shadow.frag");

  Cubemap skybox({
      "./resources/skybox/px.png",
      "./resources/skybox/nx.png",
      "./resources/skybox/py.png",
      "./resources/skybox/ny.png",
      "./resources/skybox/pz.png",
      "./resources/skybox/nz.png",
  });

  ShadowMap shadowMap(1024, 1024);
  Framebuffer sceneBuffer(1280, 720);

  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow *w, int width, int height) {
                                   glViewport(0, 0, width, height);
                                 });

  glfwSetCursorPosCallback(window, [](GLFWwindow *w, double x, double y) {
    ImGui_ImplGlfw_CursorPosCallback(w, x, y);
    if (!cursorLocked)
      return;
    if (firstMouse) {
      lastX = x;
      lastY = y;
      firstMouse = false;
    }
    camera.processMouse(x - lastX, lastY - y);
    lastX = x;
    lastY = y;
  });

  glfwSetScrollCallback(window, [](GLFWwindow *w, double x, double y) {
    ImGui_ImplGlfw_ScrollCallback(w, x, y);
    if (cursorLocked)
      camera.processScroll((float)y);
  });

  glfwSetMouseButtonCallback(
      window, [](GLFWwindow *w, int button, int action, int mods) {
        ImGui_ImplGlfw_MouseButtonCallback(w, button, action, mods);
      });

  glfwSetKeyCallback(
      window, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
        ImGui_ImplGlfw_KeyCallback(w, key, scancode, action, mods);
      });

  glfwSetCharCallback(window, [](GLFWwindow *w, unsigned int c) {
    ImGui_ImplGlfw_CharCallback(w, c);
  });

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glm::vec3 lightPos(3.0f, 10.0f, 6.0f);

  static glm::vec3 dirDirection(-2.0f, -8.0f, -4.0f);
  static glm::vec3 dirAmbient(0.3f);
  static glm::vec3 dirDiffuse(0.8f);
  static glm::vec3 dirSpecular(1.0f);
  static glm::vec3 pl0Pos(2.0f, 2.0f, 2.0f);
  static float pl0Linear = 0.09f, pl0Quadratic = 0.032f;
  static glm::vec3 pl1Pos(-2.0f, 1.0f, -2.0f);
  static float pl1Linear = 0.09f, pl1Quadratic = 0.032f;
  static float cutOff = 12.5f, outerCutOff = 17.5f;
  static glm::vec3 shadowLightPos(3.0f, 10.0f, 6.0f);
  static float orthoSize = 8.0f;
  static bool spotlightOn = false;

  while (!glfwWindowShouldClose(window)) {
    float now = glfwGetTime();
    float deltaTime = now - lastFrame;
    lastFrame = now;

    // must be first — before any ImGui:: calls
    imguiLayer.begin();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    static bool tabWasPressed = false;
    bool tabPressed = glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;
    if (tabPressed && !tabWasPressed) {
      cursorLocked = !cursorLocked;
      firstMouse = true;
      glfwSetInputMode(window, GLFW_CURSOR,
                       cursorLocked ? GLFW_CURSOR_DISABLED
                                    : GLFW_CURSOR_NORMAL);
    }
    tabWasPressed = tabPressed;

    if (cursorLocked) {
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CameraDirection::FORWARD, deltaTime);
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(CameraDirection::BACKWARD, deltaTime);
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CameraDirection::LEFT, deltaTime);
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CameraDirection::RIGHT, deltaTime);
    }

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
        glm::radians(camera.fov),
        sceneBuffer.width > 0 ? (float)sceneBuffer.width / sceneBuffer.height
                              : 1.0f,
        0.1f, 100.0f);

    shadowMap.updateLightSpace(shadowLightPos, glm::vec3(0.0f), 1.0f, 30.0f,
                               orthoSize);

    // --- pass 1: shadow ---
    shadowMap.bindForWriting();
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    shadowShader.use();
    shadowShader.setMat4("lightSpaceMatrix", shadowMap.lightSpaceMatrix);
    scene.drawShadow(shadowShader);
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // --- pass 2: scene into framebuffer ---
    sceneBuffer.bind();

    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    skyboxShader.use();
    skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.getViewMatrix())));
    skyboxShader.setMat4("projection", projection);
    skyboxShader.setInt("skybox", 1);
    skybox.bind(1);
    skyboxMesh.draw();
    glDepthMask(GL_TRUE);
    cubeObj->material.diffuse->bind(0);
    cubeObj->material.normalMap->bind(1);

    shadowMap.bindForReading(2);
    auto &shader = *cubeObj->material.shader;
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setMat4("lightSpaceMatrix", shadowMap.lightSpaceMatrix);
    shader.setVec3("viewPos", camera.position);
    shader.setInt("tex", 0);
    shader.setInt("normalMap", 1);
    shader.setInt("shadowMap", 2);
    shader.setVec3("dirLight.direction", dirDirection);
    shader.setVec3("dirLight.ambient", dirAmbient);
    shader.setVec3("dirLight.diffuse", dirDiffuse);
    shader.setVec3("dirLight.specular", dirSpecular);
    shader.setInt("numPointLights", 2);
    shader.setVec3("pointLights[0].position", pl0Pos);
    shader.setFloat("pointLights[0].constant", 1.0f);
    shader.setFloat("pointLights[0].linear", pl0Linear);
    shader.setFloat("pointLights[0].quadratic", pl0Quadratic);
    shader.setVec3("pointLights[0].ambient", glm::vec3(0.2f));
    shader.setVec3("pointLights[0].diffuse", glm::vec3(0.8f));
    shader.setVec3("pointLights[0].specular", glm::vec3(1.0f));
    shader.setVec3("pointLights[1].position", pl1Pos);
    shader.setFloat("pointLights[1].constant", 1.0f);
    shader.setFloat("pointLights[1].linear", pl1Linear);
    shader.setFloat("pointLights[1].quadratic", pl1Quadratic);
    shader.setVec3("pointLights[1].ambient", glm::vec3(0.2f));
    shader.setVec3("pointLights[1].diffuse", glm::vec3(0.8f));
    shader.setVec3("pointLights[1].specular", glm::vec3(1.0f));
    shader.setBool("useSpotLight", spotlightOn);
    shader.setVec3("spotLight.position", camera.position);
    shader.setVec3("spotLight.direction", camera.front);
    shader.setVec3("spotLight.ambient", glm::vec3(0.0f));
    shader.setVec3("spotLight.diffuse", glm::vec3(1.0f));
    shader.setVec3("spotLight.specular", glm::vec3(1.0f));
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(cutOff)));
    shader.setFloat("spotLight.outerCutOff",
                    glm::cos(glm::radians(outerCutOff)));
    scene.draw(shader);

    sceneBuffer.unbind();

    cubeObj->transform.rotation.y = now * 28.6f;
    // --- reset to screen ---
    glViewport(0, 0, fbWidth, fbHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // --- imgui panels ---
    ImGuiWindowFlags dockFlags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGuiViewport *vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(vp->Size);
    ImGui::SetNextWindowViewport(vp->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("DockSpace", nullptr, dockFlags);
    ImGui::PopStyleVar();

    ImGuiID dockID = ImGui::GetID("MainDockSpace");

    static bool dockLayoutBuilt = false;
    if (!dockLayoutBuilt) {
      dockLayoutBuilt = true;
      ImGui::DockBuilderRemoveNode(dockID);
      ImGui::DockBuilderAddNode(dockID, ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dockID, ImGui::GetMainViewport()->Size);

      ImGuiID dockRight, dockRightBottom;
      ImGuiID dockMain = ImGui::DockBuilderSplitNode(
          dockID, ImGuiDir_Left, 0.75f, nullptr, &dockRight);
      ImGuiID dockRightTop = ImGui::DockBuilderSplitNode(
          dockRight, ImGuiDir_Up, 0.5f, nullptr, &dockRightBottom);

      ImGui::DockBuilderDockWindow("Viewport", dockMain);
      ImGui::DockBuilderDockWindow("Hierarchy", dockRightTop);
      ImGui::DockBuilderDockWindow("Inspector", dockRightBottom);
      ImGui::DockBuilderDockWindow("Lighting", dockRight);
      ImGui::DockBuilderDockWindow("Camera", dockRight);
      ImGui::DockBuilderFinish(dockID);
    }

    ImGui::DockSpace(dockID, ImVec2(0, 0),
                     ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");
    ImVec2 panelSize = ImGui::GetContentRegionAvail();
    if (panelSize.x > 1 && panelSize.y > 1) {
      if ((int)panelSize.x != sceneBuffer.width ||
          (int)panelSize.y != sceneBuffer.height)
        sceneBuffer.resize((int)panelSize.x, (int)panelSize.y);
      ImGui::Image((ImTextureID)(uintptr_t)sceneBuffer.colorTexture, panelSize,
                   ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::Begin("Lighting");
    ImGui::SeparatorText("Directional light");
    ImGui::DragFloat3("Direction", &dirDirection.x, 0.01f);
    ImGui::ColorEdit3("Ambient", &dirAmbient.x);
    ImGui::ColorEdit3("Diffuse", &dirDiffuse.x);
    ImGui::ColorEdit3("Specular", &dirSpecular.x);
    ImGui::SeparatorText("Point light 0");
    ImGui::DragFloat3("Position##pl0", &pl0Pos.x, 0.1f);
    ImGui::SliderFloat("Linear##pl0", &pl0Linear, 0.0f, 1.0f);
    ImGui::SliderFloat("Quadratic##pl0", &pl0Quadratic, 0.0f, 1.0f);
    ImGui::SeparatorText("Point light 1");
    ImGui::DragFloat3("Position##pl1", &pl1Pos.x, 0.1f);
    ImGui::SliderFloat("Linear##pl1", &pl1Linear, 0.0f, 1.0f);
    ImGui::SliderFloat("Quadratic##pl1", &pl1Quadratic, 0.0f, 1.0f);
    ImGui::SeparatorText("Spotlight");
    ImGui::Checkbox("Enable spotlight", &spotlightOn);
    ImGui::SliderFloat("Cut off", &cutOff, 1.0f, 45.0f);
    ImGui::SliderFloat("Outer cut off", &outerCutOff, 1.0f, 45.0f);
    ImGui::SeparatorText("Shadow");
    ImGui::DragFloat3("Light pos##shadow", &shadowLightPos.x, 0.1f);
    ImGui::SliderFloat("Ortho size", &orthoSize, 1.0f, 50.0f);
    ImGui::End();

    ImGui::Begin("Camera");
    ImGui::Text("Position: %.2f %.2f %.2f", camera.position.x,
                camera.position.y, camera.position.z);
    ImGui::Text("Cursor: %s", cursorLocked ? "locked (Tab)" : "unlocked (Tab)");
    ImGui::SliderFloat("Speed", &camera.speed, 0.1f, 20.0f);
    ImGui::SliderFloat("Sensitivity", &camera.sensitivity, 0.01f, 1.0f);
    ImGui::SliderFloat("FOV", &camera.fov, 10.0f, 120.0f);
    ImGui::End();

    ImGui::Begin("Hierarchy");
    for (auto &obj : scene.objects) {
      bool isSelected = (scene.selected == obj);
      if (ImGui::Selectable(obj->name.c_str(), isSelected))
        scene.selected = obj;
    }
    ImGui::End();

    // inspector panel
    if (scene.selected) {
      ImGui::Begin("Inspector");
      ImGui::Text("%s", scene.selected->name.c_str());
      ImGui::SeparatorText("Transform");
      ImGui::DragFloat3("Position", &scene.selected->transform.position.x,
                        0.1f);
      ImGui::DragFloat3("Rotation", &scene.selected->transform.rotation.x,
                        1.0f);
      ImGui::DragFloat3("Scale", &scene.selected->transform.scale.x, 0.01f);
      ImGui::End();
    }
    imguiLayer.end();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
