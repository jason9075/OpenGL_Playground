#pragma once

#include "Camera.hpp"
#include "DataObject.hpp"
#include "tests/Test.hpp"

namespace test {
class TestGaussian : public Test {
 public:
  TestGaussian(const float screenWidth, const float screenHeight);
  ~TestGaussian();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;

 private:
  float scaleFactor = 1.0f;
  std::unique_ptr<GaussianSplat> splat;
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<CameraEventListener> listener;
};
}  // namespace test
