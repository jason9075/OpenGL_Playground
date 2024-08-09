#include "tests/TestTriangle.h"

#include <OPPCH.h>

#include "BasicMesh.h"

namespace test {

TestTriangle::TestTriangle(const float screenWidth, const float screenHeight) {
  // Vetrex: pos , normal, color, texCoords
  std::vector<Vertex> vertices = {{{-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f}},
                                  {{0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f}},
                                  {{0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f}}};
  std::vector<GLuint> indices = {0, 1, 2};

  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/default_vert.glsl", "./shaders/default_frag.glsl");
  shaderProgram->use();

  mesh = std::make_unique<Mesh>(vertices, indices);

  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());
}

TestTriangle::~TestTriangle() {}

void TestTriangle::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestTriangle::OnRender() {
  glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  glUniform3fv(glGetUniformLocation(shaderProgram->ID, "color"), 1, triangleColor);
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "modelMatrix"), 1, GL_FALSE,
                     glm::value_ptr(glm::mat4(1.0f)));
  camera->update(shaderProgram.get());
  mesh->draw(shaderProgram.get());
}

void TestTriangle::OnImGuiRender() {
  // setting triangle color
  ImGui::ColorEdit3("Background", backgroundColor);
  ImGui::ColorEdit3("Triangle", triangleColor);
}

void TestTriangle::OnExit() {
  glUseProgram(0);
  mesh->del();
  shaderProgram->del();
}
}  // namespace test
