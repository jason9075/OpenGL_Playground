#pragma once

#include "Camera.h"
#include "Model.h"
#include "ShaderClass.h"
#include "tests/Test.h"

namespace test {
class TestCubeMap : public Test {
 public:
  TestCubeMap(const float screenWidth, const float screenHeight);
  ~TestCubeMap();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  const std::vector<std::string> faces = {"./assets/skybox/right.jpg", "./assets/skybox/left.jpg",
                                          "./assets/skybox/top.jpg",   "./assets/skybox/bottom.jpg",
                                          "./assets/skybox/front.jpg", "./assets/skybox/back.jpg"};
  float modelScale = 1.0f;
  float skyboxScale = 1.0f;
  std::unique_ptr<Shader> skyShader;
  std::unique_ptr<Shader> modelShader;
  std::unique_ptr<CubeMap> skybox;
  std::unique_ptr<Model> model;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
