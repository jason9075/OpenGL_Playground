#include "tests/TestParallaxMapping.hpp"

#include <OPPCH.h>

#include "BasicMesh.hpp"

namespace test {

TestParallaxMapping::TestParallaxMapping(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shader = std::make_unique<Shader>("./shaders/parallax_vert.glsl", "./shaders/parallax_frag.glsl");
  std::vector<Texture> textures = {Texture("./assets/textures/room_3.png", "normal", 0)};
  const float wallHeight = 3.0f;
  wall = createPlaneMesh(wallHeight, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.8f), glm::vec3(0.0, wallHeight, 0.0f));
  wall->setTexture(textures);
  floor = createPlaneMesh(10.0f, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.2f));

  glm::vec3 position = glm::vec3(0.0f, wallHeight, 6.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  shader->use();
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}

TestParallaxMapping::~TestParallaxMapping() {}

void TestParallaxMapping::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestParallaxMapping::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  glUniform1f(glGetUniformLocation(shader->ID, "farFactor"), farFactor);
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
  glUniform3f(glGetUniformLocation(shader->ID, "camPosition"), camera->position.x, camera->position.y,
              camera->position.z);
  camera->update(shader.get());
  wall->draw(shader.get());
  floor->draw(shader.get());
}

void TestParallaxMapping::OnImGuiRender() { ImGui::SliderFloat("Far Factor", &farFactor, 0.001f, 0.999f); }

void TestParallaxMapping::OnReload() {
  shader->reload();
  shader->use();
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
  glUniform3f(glGetUniformLocation(shader->ID, "camPosition"), camera->position.x, camera->position.y,
              camera->position.z);
}

void TestParallaxMapping::OnExit() { shader->del(); }
}  // namespace test
