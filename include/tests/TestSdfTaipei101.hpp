#pragma once

#include "Camera.hpp"
#include "DataObject.hpp"
#include "ShaderClass.hpp"
#include "tests/Test.hpp"

namespace test {
class TestSdfTaipei101 : public Test {
 public:
  TestSdfTaipei101(const float screenWidth, const float screenHeight);
  ~TestSdfTaipei101();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  std::unique_ptr<Shader> shaderSDF;
  std::unique_ptr<Mesh> sdfMesh;
  std::unique_ptr<Texture> texture;
  std::unique_ptr<Texture> texture_bump;
  float lightPos[3] = {4.0f, 10.0f, 3.0f};
  bool isLightMove = true;
  float size = 1.0f;
  float sdfColor[3] = {0.1f, 0.1f, 1.0f};  // blue
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
