#include "tests/TestModel.h"

#include <OPPCH.h>

namespace test {

TestModel::TestModel(const float screenWidth, const float screenHeight, const char *path) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/model_vert.glsl", "./shaders/model_frag.glsl");
  model = std::make_unique<Model>(path);

  glm::vec3 position = glm::vec3(0.0f, 1.0f, 5.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  /* Check out this video for more information on depth testing:
   * www.youtube.com/watch?v=3xGKu4T4SCU
   */
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}

TestModel::~TestModel() {}

void TestModel::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestModel::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  shaderProgram->use();
  camera->update(shaderProgram.get());
  // update trans, scale, and rot to model.modelMatrix
  glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), trans);
  modelMatrix = glm::scale(modelMatrix, scale);
  modelMatrix = glm::rotate(modelMatrix, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
  modelMatrix = glm::rotate(modelMatrix, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
  modelMatrix = glm::rotate(modelMatrix, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
  model->setModelMatrix(modelMatrix);
  model->draw(shaderProgram.get());
}

void TestModel::OnImGuiRender() {
  // set modelMatrix
  ImGui::Text("Model Matrix");
  ImGui::SliderFloat3("Trans", &trans[0], -10.0f, 10.0f);
  ImGui::SliderFloat3("Scale", &scale[0], 0.1f, 3.0f);
  ImGui::SliderFloat3("Rot", &rot[0], -180.0f, 180.0f);
}

}  // namespace test
