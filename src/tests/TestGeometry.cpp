#include "tests/TestGeometry.hpp"

#include <OPPCH.h>

#include "geom/mesh.hpp"

namespace test {

TestGeometry::TestGeometry(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram =
      std::make_unique<Shader>("./shaders/geo_vert.glsl", "./shaders/geo_gert.glsl", "./shaders/geo_frag.glsl");

  vao = gfx::core::VAO();
  gfx::geom::Vertex vertex;
  vertex.position = glm::vec3(0.0f, 0.0f, 0.0f);
  const std::vector<gfx::geom::Vertex> vertices = {vertex};
  gfx::core::VBO vbo = gfx::core::VBO(vertices);

  vao.bind();
  vao.linkAttr(vbo, 0, 3, GL_FLOAT, sizeof(gfx::geom::Vertex), (void*)0);
  vao.unbind();

  glm::vec3 camPosition = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, camPosition, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
TestGeometry::~TestGeometry() {}

void TestGeometry::OnEvent(SDL_Event& event) { camera->handle(event); }

void TestGeometry::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  shaderProgram->use();
  camera->update(shaderProgram.get());
  glUniform1f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "geoRadius"), geoRadius);
  glUniform3f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "pointColor"), 0.0f, 1.0f, 1.0f);
  glUniform1f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "smoothValue"), smoothValue);
  glUniform1f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "mask"), mask);
  vao.bind();
  glDrawArrays(GL_POINTS, 0, 1);
  vao.unbind();
}

void TestGeometry::OnImGuiRender() {
  ImGui::SliderFloat("Radius", &geoRadius, 0.0f, 10.0f);
  ImGui::SliderFloat("Smooth", &smoothValue, 0.0f, 1.0f);
  ImGui::SliderFloat("Mask", &mask, 0.0f, 1.0f);
}

}  // namespace test
