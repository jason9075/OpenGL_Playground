#include "tests/TestGaussian.h"

#include <OPPCH.h>

namespace test {

TestGaussian::TestGaussian(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/gaussian_vert.glsl", "./shaders/gaussian_frag.glsl");

  float scale[3] = {0.0109f, 0.00179f, 0.0229f};
  float rotation[4] = {0.0832f, 0.0447f, -0.0188f, -0.0145f};

  // calculate the covariance matrix
  covariance = glm::mat3(0.0f);
  glm::quat q = glm::quat(rotation[3], rotation[0], rotation[1], rotation[2]);
  glm::mat3 R(glm::normalize(q));
  glm::mat3 S = glm::mat3(scale[0], 0.0f, 0.0f, 0.0f, scale[1], 0.0f, 0.0f, 0.0f, scale[2]);
  covariance = R * S * glm::transpose(S) * glm::transpose(R);
  // covariance = glm::mat3(0.1f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.1f);
  std::cout << "covariance: " << std::endl;
  std::cout << covariance[0][0] << " " << covariance[0][1] << " " << covariance[0][2] << std::endl;
  std::cout << covariance[1][0] << " " << covariance[1][1] << " " << covariance[1][2] << std::endl;
  std::cout << covariance[2][0] << " " << covariance[2][1] << " " << covariance[2][2] << std::endl;

  std::vector<GaussianSphere> spheres;
  GaussianSphere sphere;
  sphere.position = glm::vec3(0.0f, 0.0f, 0.0f);
  sphere.color = glm::vec3(1.0f, 1.0f, 1.0f);
  sphere.opacity = 1.0f;
  sphere.covA = glm::vec3(covariance[0][0], covariance[0][1], covariance[0][2]);
  sphere.covB = glm::vec3(covariance[1][1], covariance[1][2], covariance[2][2]);
  spheres.push_back(sphere);
  splat = std::make_unique<GaussianSplat>(spheres);

  glm::vec3 camPosition = glm::vec3(0.0f, 0.0f, 2.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, camPosition, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
}
TestGaussian::~TestGaussian() {}

void TestGaussian::OnEvent(SDL_Event& event) { camera->handle(event); }

void TestGaussian::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  camera->moveCamera();

  shaderProgram->use();
  camera->update(shaderProgram.get());
  splat->draw(shaderProgram.get());
}

void TestGaussian::OnImGuiRender() {}

}  // namespace test
