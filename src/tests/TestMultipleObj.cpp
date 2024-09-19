#include "tests/TestMultipleObj.hpp"

#include <OPPCH.h>

namespace test {

TestMultipleObj::TestMultipleObj(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/multiple_obj_vert.glsl", "./shaders/multiple_obj_frag.glsl");
  const char *path = "./assets/gltf_duck/Duck.gltf";
  model = std::make_unique<Model>(path);

  updateInstanceMatrices();

  for (unsigned int i = 0; i < model->meshes.size(); i++) {
    model->meshes[i].setupInstanceMatrices(instanceMatrices);
  }

  glm::vec3 position = glm::vec3(-2.0f, 7.0f, -4.0f);
  glm::vec3 orientation = glm::vec3(0.66f, -0.15f, 0.73f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}

TestMultipleObj::~TestMultipleObj() {}

void TestMultipleObj::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestMultipleObj::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  shaderProgram->use();
  camera->update(shaderProgram.get());
  model->draw(shaderProgram.get(), row * col);
}

void TestMultipleObj::OnImGuiRender() {
  // set modelMatrix
  ImGui::Text("instances: %d", row * col);
  if (ImGui::SliderInt("Row", &row, 1, 100)) {
    updateInstanceMatrices();
    for (unsigned int i = 0; i < model->meshes.size(); i++) {
      model->meshes[i].updateInstanceMatrices(instanceMatrices);
      // model->meshes[i].setupInstanceMatrices(instanceMatrices);
    }
  }
  if (ImGui::SliderInt("Col", &col, 1, 100)) {
    updateInstanceMatrices();
    for (unsigned int i = 0; i < model->meshes.size(); i++) {
      model->meshes[i].updateInstanceMatrices(instanceMatrices);
      // model->meshes[i].setupInstanceMatrices(instanceMatrices);
    }
  }
}

void TestMultipleObj::OnExit() {
  model->del();
  shaderProgram->del();
}

void TestMultipleObj::updateInstanceMatrices() {
  std::random_device rd;

  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> rotDis(0.0f, 360.0f);

  instanceMatrices.clear();
  instanceMatrices.resize(row * col);
  for (int x = 0; x < row; x++) {
    for (int z = 0; z < col; z++) {
      float angle = rotDis(gen);
      glm::mat4 modelPos = glm::mat4(1.0f);
      modelPos = glm::translate(modelPos, glm::vec3(x * OFFSET, 0.0f, z * OFFSET));
      modelPos = glm::rotate(modelPos, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
      instanceMatrices[x * col + z] = {modelPos};
    }
  }
}

}  // namespace test
