#include "tests/TestGaussian.h"

#include <OPPCH.h>

namespace test {

TestGaussian::TestGaussian(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/gaussian_vert.glsl", "./shaders/gaussian_frag.glsl");

  float scale[3] = {0.0109f, 0.00179f, 0.0229f};
  glm::quat rot = glm::quat(0.0832f, 0.0447f, -0.0188f, -0.0145f);

  // calculate the covariance matrix
  glm::mat3 R(glm::normalize(rot));
  glm::mat3 S = glm::mat3(scale[0], 0.0f, 0.0f, 0.0f, scale[1], 0.0f, 0.0f, 0.0f, scale[2]);
  glm::mat3 M = R * S * glm::transpose(S) * glm::transpose(R);
  // M = glm::mat3(0.1f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.1f);
  // scale up the covariance matrix
  std::cout << "covariance: " << std::endl;
  std::cout << M[0][0] << " " << M[0][1] << " " << M[0][2] << std::endl;
  std::cout << M[1][0] << " " << M[1][1] << " " << M[1][2] << std::endl;
  std::cout << M[2][0] << " " << M[2][1] << " " << M[2][2] << std::endl;

  std::vector<GaussianSphere> spheres;
  GaussianSphere sphere;
  sphere.position = glm::vec3(0.0f, 0.0f, 0.0f);
  sphere.color = glm::vec3(1.0f, 1.0f, 1.0f);
  sphere.opacity = 1.0f;
  sphere.covA = glm::vec3(M[0][0], M[0][1], M[0][2]);
  sphere.covB = glm::vec3(M[1][1], M[1][2], M[2][2]);
  spheres.push_back(sphere);
  GaussianSphere sphere2;
  sphere2.position = glm::vec3(0.0f, 0.0f, 5.0f);
  sphere2.color = glm::vec3(0.0f, 1.0f, 1.0f);
  sphere2.opacity = 1.0f;
  sphere2.covA = glm::vec3(M[0][0], M[0][1], M[0][2]);
  sphere2.covB = glm::vec3(M[1][1], M[1][2], M[2][2]);
  spheres.push_back(sphere2);
  splat = std::make_unique<GaussianSplat>(spheres);

  glm::vec3 camPosition = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, camPosition, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  // glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
}
TestGaussian::~TestGaussian() {}

void TestGaussian::OnEvent(SDL_Event& event) { camera->handle(event); }

void TestGaussian::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  camera->moveCamera();

  shaderProgram->use();
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "scaleFactor"), scaleFactor);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "W"), 1024.0f);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "H"), 768.0f);
  float tan_fovx = tan(glm::radians(45.0f) / 2.0f);
  float tan_fovy = tan(glm::radians(45.0f) / 2.0f);
  float focal_y = 768.0f / (2.0f * tan_fovy);
  float focal_x = 1024.0f / (2.0f * tan_fovx);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "focal_x"), focal_x);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "focal_y"), focal_y);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "tan_fovx"), tan_fovx);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "tan_fovy"), tan_fovy);

  camera->update(shaderProgram.get());
  splat->draw(shaderProgram.get());
}

void TestGaussian::OnImGuiRender() {
  ImGui::Text("Camera Position:");
  ImGui::Text("X:%.2f Y:%.2f Z:%.2f", camera->position.x, camera->position.y, camera->position.z);
  ImGui::Text("Camera Orientation:");
  ImGui::Text("X:%.2f Y:%.2f Z:%.2f", camera->orientation.x, camera->orientation.y, camera->orientation.z);
  ImGui::SliderFloat("ScaleF", &scaleFactor, 0.1f, 10.0f);
}

}  // namespace test
