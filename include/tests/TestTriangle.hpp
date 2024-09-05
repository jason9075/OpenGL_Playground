#pragma once

#include "Camera.hpp"
#include "DataObject.hpp"
#include "ShaderClass.hpp"
#include "tests/Test.hpp"

namespace test {
class TestTriangle : public Test {
 public:
  TestTriangle(const float screenWidth, const float screenHeight);
  ~TestTriangle();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  float backgroundColor[3] = {0.0f, 0.0f, 0.0f};
  float triangleColor[3] = {0.2f, 0.8f, 0.2f};
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Mesh> mesh;
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
