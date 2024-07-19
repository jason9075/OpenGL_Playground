#pragma once

#include "Camera.h"
#include "DataObject.h"
#include "ShaderClass.h"
#include "tests/Test.h"

namespace test {
class TestTriangle : public Test {
 public:
  TestTriangle(const float screenWidth, const float screenHeight);
  ~TestTriangle();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;

 private:
  float backgroundColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  float triangleColor[4] = {0.2f, 0.8f, 0.2f, 1.0f};
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Mesh> mesh;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
