#include "tests/TestLighting.hpp"

#include <OPPCH.h>

#include "BasicMesh.hpp"

namespace test {

TestLighting::TestLighting(const float screenWidth, const float screenHeight) : renderer(mesh_renderer) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/light_vert.glsl", "./shaders/light_frag.glsl");
  pureLightShader = std::make_unique<Shader>("./shaders/default_vert.glsl", "./shaders/default_frag.glsl");
  model = std::make_unique<Model>("./assets/gltf_duck/Duck.gltf");

  lightMesh = createCubeMesh(0.1);

  glm::vec3 position = glm::vec3(0.0f, 1.0f, 5.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}

TestLighting::~TestLighting() {}

void TestLighting::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestLighting::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  // draw model
  shaderProgram->use();
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram->PROGRAM_ID, "modelMatrix"), 1, GL_FALSE,
                     glm::value_ptr(modelMatrix));
  glUniform3fv(glGetUniformLocation(shaderProgram->PROGRAM_ID, "lightColor"), 1, lightColor);
  glUniform3fv(glGetUniformLocation(shaderProgram->PROGRAM_ID, "lightPosition"), 1, lightPos);
  glUniform1i(glGetUniformLocation(shaderProgram->PROGRAM_ID, "ambientEnabled"), ambientToggle);
  glUniform1i(glGetUniformLocation(shaderProgram->PROGRAM_ID, "diffuseEnabled"), diffuseToggle);
  glUniform1i(glGetUniformLocation(shaderProgram->PROGRAM_ID, "specularEnabled"), specularToggle);
  camera->update(shaderProgram.get());
  renderer.draw(*model, *shaderProgram);

  // draw light
  pureLightShader->use();
  modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
  glUniformMatrix4fv(glGetUniformLocation(pureLightShader->PROGRAM_ID, "modelMatrix"), 1, GL_FALSE,
                     glm::value_ptr(modelMatrix));
  glUniform3fv(glGetUniformLocation(pureLightShader->PROGRAM_ID, "color"), 1, lightColor);
  camera->update(pureLightShader.get());
  mesh_renderer.draw(*lightMesh, *pureLightShader, 1);
}

void TestLighting::OnImGuiRender() {
  ImGui::ColorEdit3("Color", lightColor);
  ImGui::SliderFloat3("Pos", lightPos, -3.0f, 3.0f);
  ImGui::Checkbox("Ambient", &ambientToggle);
  ImGui::Checkbox("Diffuse", &diffuseToggle);
  ImGui::Checkbox("Specular", &specularToggle);
}

}  // namespace test
