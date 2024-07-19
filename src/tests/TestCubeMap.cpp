#include "tests/TestCubeMap.h"

#include <OPPCH.h>
#include <stb_image.h>

namespace test {

TestCubeMap::TestCubeMap(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  skyShader = std::make_unique<Shader>("./shaders/skybox_vert.glsl", "./shaders/skybox_frag.glsl");
  modelShader = std::make_unique<Shader>("./shaders/model_vert.glsl", "./shaders/model_frag.glsl");
  skybox = std::make_unique<CubeMap>(faces);
  model = std::make_unique<Model>("./assets/gltf_duck/Duck.gltf");

  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());
}

TestCubeMap::~TestCubeMap() {}

void TestCubeMap::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestCubeMap::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  glEnable(GL_DEPTH_TEST);

  // draw model
  glDepthFunc(GL_LESS);
  modelShader->use();
  camera->update(modelShader.get());
  glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale));
  model->setModelMatrix(modelMatrix);
  model->draw(modelShader.get());

  // draw skybox
  glDepthFunc(GL_LEQUAL);
  skyShader->use();

  glUniform1f(glGetUniformLocation(skyShader->ID, "scaleFactor"), skyboxScale);

  camera->update(skyShader.get());

  skybox->draw(skyShader.get());

  glDepthFunc(GL_LESS);
}

void TestCubeMap::OnImGuiRender() {
  // setting scale factor
  ImGui::Text("Scale Factor");
  ImGui::SliderFloat("Model", &modelScale, 0.1f, 5.0f);
  ImGui::SliderFloat("Sky", &skyboxScale, 0.1f, 100.0f);
}

}  // namespace test
