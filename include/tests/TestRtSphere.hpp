#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "ShaderClass.hpp"
#include "tests/Test.hpp"

namespace test {

struct Material {  // 48 bytes
  glm::vec4 color;
  glm::vec4 emissionColor;    // 發光顏色
  float shininess;            // 發光強度
  float smoothness;           // 表面粗糙度 PS:當鏡面反射沒開啟時，沒有作用
  float specularProbability;  // 鏡面反射機率 Specular Bounce Probability
  float padding;
};

struct Sphere {
  glm::vec3 center;   // 12 bytes
  float radius;       // 4 bytes
  Material material;  // 48 bytes
};

struct Triangle {
  glm::vec3 posA;     // 12 bytes
  float paddingA;     // 4 bytes
  glm::vec3 posB;     // 12 bytes
  float paddingB;     // 4 bytes
  glm::vec3 posC;     // 12 bytes
  float paddingC;     // 4 bytes
  glm::vec3 normal;   // 12 bytes
  float paddingN;     // 4 bytes
  Material material;  // 48 bytes
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
  static const int MAX_SPHERES = 50;
  static const int MAX_TRIANGLES = 24;  // 6 Plane (12) + 1 Light Cuboid (12)
  static constexpr float CBS = 10.0f;   // Cornell Box Size
  static constexpr float CBS2 = 2 * CBS;
  static constexpr float MAX_REFLECTIVITY = 0.995f;
  // ray casting
  int numBounces = 3;
  int numRays = 3;
  bool enableSpecularBounce = true;
  bool isSpecularWhite = false;
  float sphereLightOffset[3] = {-6.0f, 1.0f, 8.0f};
  float ambientLight = 0.0f;
  // cornell box
  bool isShowCornellPlanes = true;
  glm::vec3 backWallColor = glm::vec3(0.2f, 0.2f, 0.2f);
  float redWallReflectivity = 0.0f;
  float greenWallReflectivity = 0.0f;
  float floorReflectivity = 0.0f;
  float ceilingReflectivity = 0.0f;
  float frontWallReflectivity = 0.0f;
  float backWallReflectivity = 0.0f;
  const glm::vec3 lightCenter = glm::vec3(0.0f, CBS2 - 0.10000001f, 0.0f);
  const glm::vec3 lightSize = glm::vec3(4.0f, 0.1f, 4.0f);
  bool isShowCornellLight = true;
  float mainLightShininess = 2.0f;
  // sphere
  int numSpheres = 6;
  bool isRotate = false;
  bool showSphereLight = true;
  float sphereLightShininess = 2.0f;
  bool isRandomShine = false;
  // render
  bool isRealTime = true;
  // data
  Sphere spheres[MAX_SPHERES];
  Triangle triangles[MAX_TRIANGLES];
  unsigned int frameIdx = 0;
  std::unique_ptr<FBO> oldFrame;
  std::unique_ptr<FBO> newFrame;
  std::unique_ptr<Mesh> rtMesh;
  std::unique_ptr<Mesh> frameMesh;
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Shader> frameShader;
  std::unique_ptr<CameraEventListener> listener;

  void setReflectivity(Triangle* triangles, const int start, const int end, const float reflectivity);
  void makeCornellBox(Triangle* triangles);
  void randomizeSpheres(Sphere* spheres, const int numSpheres);
};

}  // namespace test
