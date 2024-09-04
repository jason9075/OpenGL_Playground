#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "ShaderClass.hpp"
#include "tests/Test.hpp"

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
  std::vector<std::string> faces = {"./assets/skybox/right.jpg", "./assets/skybox/left.jpg",
                                    "./assets/skybox/top.jpg",   "./assets/skybox/bottom.jpg",
                                    "./assets/skybox/front.jpg", "./assets/skybox/back.jpg"};
  // debug
  // std::vector<std::string> faces = {"./assets/skybox/right_debug.jpg", "./assets/skybox/left_debug.jpg",
  //                                   "./assets/skybox/top_debug.jpg",   "./assets/skybox/bottom_debug.jpg",
  //                                   "./assets/skybox/front_debug.jpg", "./assets/skybox/back_debug.jpg"};
  float modelScale = 0.2f;
  float skyboxScale = 5.0f;
  bool fixSkybox = false;
  std::unique_ptr<Shader> skyShader;
  std::unique_ptr<Shader> modelShader;
  std::unique_ptr<CubeMap> skybox;
  std::unique_ptr<Model> model;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
