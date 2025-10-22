#include "tests/TestCubeMap.hpp"

#include <OPPCH.h>
#include <stb_image.h>

namespace test {

TestCubeMap::TestCubeMap(const float screenWidth, const float screenHeight)
    : renderer(mesh_renderer), cubemap_renderer(mesh_renderer) {
  glViewport(0, 0, screenWidth, screenHeight);

  skyShader = std::make_unique<Shader>("./shaders/skybox_vert.glsl", "./shaders/skybox_frag.glsl");
  modelShader = std::make_unique<Shader>("./shaders/model_vert.glsl", "./shaders/model_frag.glsl");
  skybox = std::make_unique<gfx::geom::CubeMap>(faces, false);
  model = std::make_unique<Model>("./assets/gltf_duck/Duck.gltf");

  glm::vec3 position = glm::vec3(0.0f, 1.0f, 3.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  glEnable(GL_DEPTH_TEST);
}

void TestCubeMap::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestCubeMap::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  // draw model
  modelShader->use();
  glDepthFunc(GL_LESS);
  camera->update(modelShader.get());
  glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale));
  model->setModelMatrix(modelMatrix);
  renderer.draw(*model, *modelShader);

  // draw skybox, tips: render skybox after all other objects
  skyShader->use();
  glDepthFunc(GL_LEQUAL);
  glUniform1f(glGetUniformLocation(skyShader->PROGRAM_ID, "scaleFactor"), skyboxScale);
  glUniform1i(glGetUniformLocation(skyShader->PROGRAM_ID, "fixSkybox"), fixSkybox);
  camera->update(skyShader.get());
  cubemap_renderer.draw(*skybox, *skyShader);
}

void TestCubeMap::OnImGuiRender() {
  ImGui::Checkbox("Fix Skybox", &fixSkybox);
  ImGui::Text("Scale Factor");
  ImGui::SliderFloat("Duck", &modelScale, 0.1f, 5.0f);
  ImGui::SliderFloat("Skybox", &skyboxScale, 0.1f, 50.0f);
}

void TestCubeMap::OnExit() {}

}  // namespace test
