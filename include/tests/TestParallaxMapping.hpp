#pragma once

#include "Camera.hpp"
#include "DataObject.hpp"
#include "ShaderClass.hpp"
#include "tests/Test.hpp"

namespace test {
class TestParallaxMapping : public Test {
 public:
  TestParallaxMapping(const float screenWidth, const float screenHeight);
  ~TestParallaxMapping();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnReload() override;
  void OnExit() override;

 private:
  float farFactor = 0.5f;
  std::unique_ptr<Shader> shader;
  std::unique_ptr<Mesh> wall;
  std::unique_ptr<Mesh> floor;
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
