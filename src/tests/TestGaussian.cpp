#include "tests/TestGaussian.hpp"

#include <OPPCH.h>

namespace test {

TestGaussian::TestGaussian(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/gaussian_vert.glsl", "./shaders/gaussian_frag.glsl");

  float scale1[3] = {-2.51659, -5.3231, -2.7751};
  glm::quat rot1 = glm::quat(0.0832f, 0.0447f, -0.0188f, -0.0145f);

  float scale2[3] = {-4.51659, -6.3231, -3.7751};
  glm::quat rot2 = glm::quat(0.0332f, 0.0467f, -0.0238f, -0.0845f);

  // calculate the covariance matrix
  glm::mat3 R(glm::normalize(rot1));
  glm::mat3 S =
      glm::mat3(std::exp(scale1[0]), 0.0f, 0.0f, 0.0f, std::exp(scale1[1]), 0.0f, 0.0f, 0.0f, std::exp(scale1[2]));
  glm::mat3 M1 = R * S * glm::transpose(S) * glm::transpose(R);

  R = glm::mat3(glm::normalize(rot2));
  S = glm::mat3(std::exp(scale2[0]), 0.0f, 0.0f, 0.0f, std::exp(scale2[1]), 0.0f, 0.0f, 0.0f, std::exp(scale2[2]));
  glm::mat3 M2 = R * S * glm::transpose(S) * glm::transpose(R);

  std::vector<gfx::geom::GaussianSphere> spheres;
  gfx::geom::GaussianSphere sphere1;  // Yellow (Front)
  sphere1.position = glm::vec3(0.0f, 0.0f, 0.0f);
  sphere1.color = glm::vec3(1.0f, 1.0f, 0.0f);
  sphere1.opacity = 1.0f;
  sphere1.covA = glm::vec3(M1[0][0], M1[0][1], M1[0][2]);
  sphere1.covB = glm::vec3(M1[1][1], M1[1][2], M1[2][2]);
  gfx::geom::GaussianSphere sphere2;  // BlueGreen (Back)
  sphere2.position = glm::vec3(0.05f, 0.05f, -1.0f);
  sphere2.color = glm::vec3(0.0f, 1.0f, 1.0f);
  sphere2.opacity = 1.0f;
  sphere2.covA = glm::vec3(M2[0][0], M2[0][1], M2[0][2]);
  sphere2.covB = glm::vec3(M2[1][1], M2[1][2], M2[2][2]);
  spheres.push_back(sphere1);
  spheres.push_back(sphere2);  // order matters, first sphere is drawn first

  splat = std::make_unique<gfx::geom::GaussianSplat>(spheres);

  glm::vec3 camPosition = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, camPosition, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  glDisable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}
TestGaussian::~TestGaussian() {}

void TestGaussian::OnEvent(SDL_Event& event) { camera->handle(event); }

void TestGaussian::OnRender() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  camera->moveCamera();

  shaderProgram->use();
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram->PROGRAM_ID, "modelMatrix"), 1, GL_FALSE,
                     glm::value_ptr(glm::mat4(1.0f)));
  glUniform1f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "scaleFactor"), scaleFactor);
  glUniform1f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "W"), 1024.0f);
  glUniform1f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "H"), 768.0f);
  float tan_fovx = tan(glm::radians(45.0f) / 2.0f);
  float tan_fovy = tan(glm::radians(45.0f) / 2.0f);
  float focal_y = 768.0f / (2.0f * tan_fovy);
  float focal_x = 1024.0f / (2.0f * tan_fovx);
  glUniform1f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "focal_x"), focal_x);
  glUniform1f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "focal_y"), focal_y);
  glUniform1f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "tan_fovx"), tan_fovx);
  glUniform1f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "tan_fovy"), tan_fovy);

  camera->update(shaderProgram.get());
  splat->sort(camera->viewMatrix, true);
  renderer.draw(*splat, *shaderProgram);
}

void TestGaussian::OnImGuiRender() { ImGui::SliderFloat("Render S", &scaleFactor, 0.1f, 10.0f); }

}  // namespace test
