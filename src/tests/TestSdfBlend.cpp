#include "tests/TestSdfBlend.h"

#include <OPPCH.h>

#include "BasicMesh.h"

namespace test {

TestSdfBlend::TestSdfBlend(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderModel = std::make_unique<Shader>("./shaders/light_vert.glsl", "./shaders/light_frag.glsl");
  shaderSDF = std::make_unique<Shader>("./shaders/sdf_blend_vert.glsl", "./shaders/sdf_blend_frag.glsl");

  // mesh for rendering the sdf
  sdfMesh = createPlaneMesh();

  // mesh for rendering the model
  modelMesh = createCubeMesh(1.0f);

  // camera
  glm::vec3 position = glm::vec3(2.0f, 2.0f, 8.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position);
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

  // model shader
  shaderModel->use();
  glUniform3fv(glGetUniformLocation(shaderModel->ID, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f)));
  glUniform1i(glGetUniformLocation(shaderModel->ID, "useTexture"), 0);
  glUseProgram(0);
}

TestSdfBlend::~TestSdfBlend() {}

void TestSdfBlend::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestSdfBlend::OnRender() {
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  if (isLightMove) {
    float timeValue = SDL_GetTicks() / 1000.0f;
    lightPos[0] = 4.0f * sin(timeValue);
    lightPos[2] = 4.0f * cos(timeValue);
  }

  if (isShowModel) {
    shaderModel->use();
    glUniform3fv(glGetUniformLocation(shaderModel->ID, "lightPosition"), 1, lightPos);
    glUniformMatrix4fv(glGetUniformLocation(shaderModel->ID, "modelMatrix"), 1, GL_FALSE,
                       glm::value_ptr(glm::scale(glm::mat4(1.0f), glm::vec3(size))));
    glUniform3f(glGetUniformLocation(shaderModel->ID, "camPosition"), camera->position.x, camera->position.y,
                camera->position.z);
    glUniform3fv(glGetUniformLocation(shaderModel->ID, "color"), 1, modelColor);
    camera->update(shaderModel.get());
    modelMesh->draw(shaderModel.get());
  }

  if (isShowSdf) {
    shaderSDF->use();
    glUniform1f(glGetUniformLocation(shaderSDF->ID, "sdfSize"), size);
    glUniform3fv(glGetUniformLocation(shaderSDF->ID, "sdfColor"), 1, sdfColor);
    glUniform3fv(glGetUniformLocation(shaderSDF->ID, "lightPosition"), 1, lightPos);
    glUniform3f(glGetUniformLocation(shaderSDF->ID, "camPosition"), camera->position.x, camera->position.y,
                camera->position.z);
    glUniform1i(glGetUniformLocation(shaderSDF->ID, "isSphere"), isSphere);
    camera->update(shaderSDF.get());
    sdfMesh->draw(shaderSDF.get());
  }
}

void TestSdfBlend::OnImGuiRender() {
  ImGui::Text("Camera Position:");
  ImGui::Text("X:%.2f Y:%.2f Z:%.2f", camera->position.x, camera->position.y, camera->position.z);
  ImGui::Text("Camera Orientation:");
  ImGui::Text("X:%.2f Y:%.2f Z:%.2f", camera->orientation.x, camera->orientation.y, camera->orientation.z);
  ImGui::Text("Look At:");
  ImGui::Text("X:%.2f Y:%.2f Z:%.2f", camera->position.x + camera->orientation.x,
              camera->position.y + camera->orientation.y, camera->position.z + camera->orientation.z);
  ImGui::Checkbox("Move Light", &isLightMove);
  ImGui::SliderFloat("Size", &size, 0.5f, 3.0f);
  ImGui::Checkbox("Show SDF", &isShowSdf);
  ImGui::Checkbox("SDF Sphere", &isSphere);
  ImGui::ColorEdit3("SDF", sdfColor);
  ImGui::Checkbox("Show Model", &isShowModel);
  ImGui::ColorEdit3("Model", modelColor);
}

void TestSdfBlend::OnExit() {
  glUseProgram(0);
  modelMesh->del();
  sdfMesh->del();
  shaderSDF->del();
  shaderModel->del();
}

}  // namespace test
