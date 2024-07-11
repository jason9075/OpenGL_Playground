#pragma once

#include <iostream>
#include <vector>

#include "Camera.h"
#include "DataObject.h"
#include "ShaderClass.h"
#include "tests/Test.h"

namespace test {
class TestGs : public Test {
 public:
  TestGs(const float screenWidth, const float screenHeight);
  ~TestGs();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;

 private:
  std::unique_ptr<PointCloud> pc;
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<CameraEventListener> listener;
};
}  // namespace test
