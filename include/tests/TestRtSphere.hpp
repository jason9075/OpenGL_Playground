#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "ShaderClass.hpp"
#include "tests/Test.hpp"

namespace test {

struct Sphere {
  glm::vec3 center;
  float radius;
  glm::vec4 color;
  glm::vec4 emissionColor;
  float padding[3];
  float shininess;
};

class TestRtSphere : public Test {
 public:
  TestRtSphere(const float screenWidth, const float screenHeight);
  ~TestRtSphere();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  static const int MAX_SPHERES = 100;
  int numSpheres = 5;
  int numBounces = 3;
  int numRays = 2;
  float lightStrengthFactor = 1.0f;
  bool isRotate = false;
  bool isFirstSphereLight = true;
  bool isRandomShine = false;
  bool isRealTime = true;
  Sphere spheres[MAX_SPHERES];
  unsigned int frameIdx = 0;
  std::unique_ptr<FBO> oldFrame;
  std::unique_ptr<FBO> newFrame;
  std::unique_ptr<Mesh> rtMesh;
  std::unique_ptr<Mesh> frameMesh;
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Shader> frameShader;
  // std::unique_ptr<Camera> camera;
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
