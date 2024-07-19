#pragma once

#include "Camera.h"
#include "DataObject.h"
#include "ShaderClass.h"
#include "happly.h"
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
  float rotateX = 0.6f;
  float rotateZ = 3.15f;
  const float C0 = 0.28209479177387814f;
  std::unique_ptr<PointCloud> pc;
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<CameraEventListener> listener;
  void printInfo(happly::PLYData& plyIn);
};
}  // namespace test
