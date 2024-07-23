#pragma once

#include "Camera.h"
#include "DataObject.h"
#include "tests/Test.h"

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
