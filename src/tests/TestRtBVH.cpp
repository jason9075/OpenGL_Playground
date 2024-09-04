#include "tests/TestRtBVH.hpp"

#include <OPPCH.h>

namespace test {

TestRtBVH::TestRtBVH(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/rt_bvh_vert.glsl", "./shaders/rt_bvh_frag.glsl");
  model = std::make_unique<Model>("./assets/gltf_duck/Duck.gltf");

  glm::vec3 position = glm::vec3(5.0f, 1.0f, 0.0f);
  glm::vec3 orientation = glm::vec3(-1.0f, 0.0f, 0.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  startTS = std::chrono::high_resolution_clock::now();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

TestRtBVH::~TestRtBVH() {}

void TestRtBVH::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestRtBVH::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  shaderProgram->use();
  camera->update(shaderProgram.get());
  model->setModelMatrix(glm::mat4(1.0f));

  auto elapsed = std::chrono::high_resolution_clock::now() - startTS;

  glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "modelMatrix"), 1, GL_FALSE,
                     glm::value_ptr(glm::mat4(1.0)));
  auto numTriangles = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
  model->meshes[0].draw(shaderProgram.get(), numTriangles, 0);
}

void TestRtBVH::OnImGuiRender() {
  ImGui::Text("Tris: %d", model->meshes[0].numTriangles());
  if (ImGui::Button("Redraw")) {
    startTS = std::chrono::high_resolution_clock::now();
  }
}

void TestRtBVH::OnExit() {
  model->del();
  shaderProgram->del();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

}  // namespace test
