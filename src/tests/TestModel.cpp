#include "tests/TestModel.h"

#include "GL/glew.h"

namespace test {

TestModel::TestModel(const float screenWidth, const float screenHeight, const char *path) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/model_vert.glsl", "./shaders/model_frag.glsl");
  model = std::make_unique<Model>(path);

  glm::vec3 position = glm::vec3(0.0f, 5.0f, 20.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());
}

TestModel::~TestModel() {}

void TestModel::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestModel::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();
  shaderProgram->use();

  /* Check out this video for more information on depth testing:
   * www.youtube.com/watch?v=3xGKu4T4SCU
   */
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  camera->update(shaderProgram.get());
  model->draw(shaderProgram.get(), camera.get());
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

void TestModel::OnImGuiRender() {}

}  // namespace test
