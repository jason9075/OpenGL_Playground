#pragma once

#include "Camera.hpp"
#include "DataObject.hpp"
#include "ShaderClass.hpp"
#include "happly.h"
#include "tests/Test.hpp"

namespace test {
class TestGs : public Test {
 public:
  TestGs(const float screenWidth, const float screenHeight);
  ~TestGs();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;

 private:
  float rotateX = 35.;
  float rotateZ = 180.;
  const float C0 = 0.28209479177387814f;
  float scaleFactor = 1.0f;
  std::unique_ptr<PointCloud> pc;
  std::unique_ptr<GaussianSplat> splat;
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<CameraEventListener> listener;
  void printInfo(happly::PLYData& plyIn);
};
}  // namespace test
