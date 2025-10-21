#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "ShaderClass.hpp"
#include "tests/Test.hpp"

namespace test {
class TestModel : public Test {
 public:
  TestModel(const float screenWidth, const float screenHeight, const char* path);
  ~TestModel() = default;

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Model> model;
  glm::vec3 trans{0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f, 1.0f, 1.0f};
  glm::vec3 rot{0.0f, 0.0f, 0.0f};
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
