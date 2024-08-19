#include <OPPCH.h>

#include "BasicMesh.h"
#include "tests/TestSdfTaipei101.h"

namespace test {

TestSdfTaipei101::TestSdfTaipei101(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderSDF = std::make_unique<Shader>("./shaders/sdf_taipei101_vert.glsl", "./shaders/sdf_taipei101_frag.glsl");

  sdfMesh = createPlaneMesh();

  // texture
  texture = std::make_unique<Texture>("./assets/textures/windows2.png", "normal", 0);
  texture_bump = std::make_unique<Texture>("./assets/textures/windows_bump.png", "normal", 1);

  // camera
  glm::vec3 position = glm::vec3(1.0f, 6.0f, 6.0f);
  glm::vec3 orientation = glm::vec3(-0.06f, -0.38f, -0.92f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // sdf shader
  shaderSDF->use();
  glUniform2f(glGetUniformLocation(shaderSDF->ID, "resolution"), screenWidth, screenHeight);
  glUniform3f(glGetUniformLocation(shaderSDF->ID, "sdfCenter"), 0.0f, 0.0f, 0.0f);
  glUniform1f(glGetUniformLocation(shaderSDF->ID, "fov"), camera->fov);
  glUniform3fv(glGetUniformLocation(shaderSDF->ID, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f)));
}

TestSdfTaipei101::~TestSdfTaipei101() {}

void TestSdfTaipei101::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestSdfTaipei101::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  if (isLightMove) {
    float timeValue = SDL_GetTicks() / 1000.0f;
    lightPos[0] = 4.0f * sin(timeValue);
    lightPos[2] = 4.0f * cos(timeValue);
  }

  shaderSDF->use();
  glUniform1f(glGetUniformLocation(shaderSDF->ID, "sdfSize"), size);
  glUniform3fv(glGetUniformLocation(shaderSDF->ID, "sdfColor"), 1, sdfColor);
  glUniform3fv(glGetUniformLocation(shaderSDF->ID, "lightPosition"), 1, lightPos);
  glUniform3f(glGetUniformLocation(shaderSDF->ID, "camPosition"), camera->position.x, camera->position.y,
              camera->position.z);
  camera->update(shaderSDF.get());
  texture->texUnit(*shaderSDF, "taipei101", 0);
  texture->bind();
  texture_bump->texUnit(*shaderSDF, "taipei101_bump", 1);
  texture_bump->bind();
  sdfMesh->draw(shaderSDF.get());
}

void TestSdfTaipei101::OnImGuiRender() {
  ImGui::Text("Camera Position:");
  ImGui::Text("X:%.2f Y:%.2f Z:%.2f", camera->position.x, camera->position.y, camera->position.z);
  ImGui::Text("Camera Orientation:");
  ImGui::Text("X:%.2f Y:%.2f Z:%.2f", camera->orientation.x, camera->orientation.y, camera->orientation.z);
  ImGui::Text("Look At:");
  ImGui::Text("X:%.2f Y:%.2f Z:%.2f", camera->position.x + camera->orientation.x,
              camera->position.y + camera->orientation.y, camera->position.z + camera->orientation.z);
  ImGui::Checkbox("Move Light", &isLightMove);
  ImGui::SliderFloat("Size", &size, 0.01f, 3.0f);
  ImGui::ColorEdit3("SDF", sdfColor);
}

void TestSdfTaipei101::OnExit() {
  glUseProgram(0);
  sdfMesh->del();
  shaderSDF->del();
}

}  // namespace test
