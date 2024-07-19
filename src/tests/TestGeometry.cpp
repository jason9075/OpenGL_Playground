#include "tests/TestGeometry.h"

#include <OPPCH.h>

namespace test {

TestGeometry::TestGeometry(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram =
      std::make_unique<Shader>("./shaders/geo_vert.glsl", "./shaders/geo_gert.glsl", "./shaders/geo_frag.glsl");

  vao = VAO();
  std::vector<Vertex> vertices;
  Vertex vertex;
  vertex.position = glm::vec3(0.0f, 0.0f, 0.0f);
  vertices.push_back(vertex);
  VBO vbo = VBO(vertices);

  vao.bind();
  vao.linkAttr(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
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
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "geoRadius"), geoRadius);
  glUniform3f(glGetUniformLocation(shaderProgram->ID, "pointColor"), 0.0f, 1.0f, 1.0f);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "smoothValue"), smoothValue);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "mask"), mask);
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
