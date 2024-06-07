#pragma once

#include <memory>

#include "Camera.h"
#include "Mesh.h"
#include "ShaderClass.h"
#include "tests/Test.h"

namespace test {
class TestTriangle : public Test {
 public:
  TestTriangle(float screenWidth, float screenHeight);
  ~TestTriangle();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;

 private:
  class CameraEventListener;
  float backgroundColor[3] = {0.2f, 0.3f, 0.8f};
  float triangleColor[3] = {0.2f, 0.8f, 0.2f};
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Mesh> mesh;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
