#pragma once

#include <memory>

#include "Camera.h"
#include "Model.h"
#include "ShaderClass.h"
#include "tests/Test.h"

namespace test {
class TestModel : public Test {
 public:
  TestModel(const float screenWidth, const float screenHeight, const char* path);
  ~TestModel();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;

 private:
  class CameraEventListener;
  float backgroundColor[3] = {0.2f, 0.3f, 0.8f};
  float meshColor[3] = {0.6f, 0.7f, 0.2f};
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Model> model;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
