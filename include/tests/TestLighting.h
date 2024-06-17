#pragma once

#include <memory>

#include "Camera.h"
#include "Model.h"
#include "ShaderClass.h"
#include "tests/Test.h"

namespace test {
class TestLighting : public Test {
 public:
  TestLighting(const float screenWidth, const float screenHeight);
  ~TestLighting();

  glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);
  bool ambientToggle = true;
  bool diffuseToggle = true;
  bool specularToggle = true;

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;

 private:
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Shader> pureLightShader;
  std::unique_ptr<Mesh> lightMesh;
  std::unique_ptr<Model> model;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
