#include "tests/TestLighting.h"

#include <OPPCH.h>

namespace test {

TestLighting::TestLighting(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/light_vert.glsl", "./shaders/light_frag.glsl");
  pureLightShader = std::make_unique<Shader>("./shaders/default_vert.glsl", "./shaders/default_frag.glsl");
  model = std::make_unique<Model>("./assets/gltf_duck/Duck.gltf");

  std::vector<Vertex> lightVertices = {
      {{-0.1f, -0.1f, -0.1f}, {0.0f, 0.0f, -1.0f}}, {{0.1f, -0.1f, -0.1f}, {0.0f, 0.0f, -1.0f}},
      {{0.1f, 0.1f, -0.1f}, {0.0f, 0.0f, -1.0f}},   {{-0.1f, 0.1f, -0.1f}, {0.0f, 0.0f, -1.0f}},
      {{-0.1f, -0.1f, 0.1f}, {0.0f, 0.0f, 1.0f}},   {{0.1f, -0.1f, 0.1f}, {0.0f, 0.0f, 1.0f}},
      {{0.1f, 0.1f, 0.1f}, {0.0f, 0.0f, 1.0f}},     {{-0.1f, 0.1f, 0.1f}, {0.0f, 0.0f, 1.0f}}};
  std::vector<GLuint> lightIndices = {0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 7, 6, 5, 5, 4, 7,
                                      4, 0, 3, 3, 7, 4, 4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3};
  std::vector<Texture> lightTextures = {};

  lightMesh = std::make_unique<Mesh>(lightVertices, lightIndices, lightTextures);

  glm::vec3 position = glm::vec3(0.0f, 1.0f, 5.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());
}

TestLighting::~TestLighting() {}

void TestLighting::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestLighting::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // draw model
  shaderProgram->use();
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
  glUniform4f(glGetUniformLocation(shaderProgram->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z,
              lightColor.w);
  glUniform3f(glGetUniformLocation(shaderProgram->ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
  glUniform3f(glGetUniformLocation(shaderProgram->ID, "camPos"), camera->position.x, camera->position.y,
              camera->position.z);
  glUniform1i(glGetUniformLocation(shaderProgram->ID, "ambientEnabled"), ambientToggle);
  glUniform1i(glGetUniformLocation(shaderProgram->ID, "diffuseEnabled"), diffuseToggle);
  glUniform1i(glGetUniformLocation(shaderProgram->ID, "specularEnabled"), specularToggle);
  camera->update(shaderProgram.get());
  model->draw(shaderProgram.get());

  // draw light
  pureLightShader->use();
  modelMatrix = glm::translate(glm::mat4(1.0f), lightPos);
  glUniformMatrix4fv(glGetUniformLocation(pureLightShader->ID, "modelMatrix"), 1, GL_FALSE,
                     glm::value_ptr(modelMatrix));
  glUniform4f(glGetUniformLocation(pureLightShader->ID, "color"), lightColor.x, lightColor.y, lightColor.z,
              lightColor.w);
  camera->update(pureLightShader.get());
  lightMesh->draw(pureLightShader.get());
}

void TestLighting::OnImGuiRender() {
  ImGui::ColorEdit3("Color", glm::value_ptr(lightColor));
  ImGui::SliderFloat3("Pos", glm::value_ptr(lightPos), -3.0f, 3.0f);
  ImGui::Checkbox("Ambient", &ambientToggle);
  ImGui::Checkbox("Diffuse", &diffuseToggle);
  ImGui::Checkbox("Specular", &specularToggle);
}

}  // namespace test
