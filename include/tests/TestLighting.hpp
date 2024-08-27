#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "ShaderClass.hpp"
#include "tests/Test.hpp"

namespace test {
class TestLighting : public Test {
 public:
  TestLighting(const float screenWidth, const float screenHeight);
  ~TestLighting();

  float lightColor[3] = {1.0f, 1.0f, 1.0f};
  float lightPos[3] = {1.0f, 1.0f, 1.0f};
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
