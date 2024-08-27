#pragma once

#include "Camera.hpp"
#include "DataObject.hpp"
#include "tests/Test.hpp"

namespace test {
class TestGeometry : public Test {
 public:
  TestGeometry(const float screenWidth, const float screenHeight);
  ~TestGeometry();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;

 private:
  VAO vao;
  float geoRadius = 1.0f;
  float smoothValue = 0.25f;
  float mask = 0.25f;
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<CameraEventListener> listener;
};
}  // namespace test
