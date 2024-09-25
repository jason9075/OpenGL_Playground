#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "ShaderClass.hpp"
#include "tests/Test.hpp"

namespace test {
class TestRtBVH : public Test {
 public:
  TestRtBVH(const float screenWidth, const float screenHeight);
  ~TestRtBVH();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> startTS;
  std::unique_ptr<Mesh> beam;

  glm::mat4 beamModelMatrix = glm::mat4(1.0f);
  glm::quat beamRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  // beam rotation in 3D space

  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Model> model;
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
