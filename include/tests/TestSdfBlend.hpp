#pragma once

#include "Camera.hpp"
#include "ShaderClass.hpp"
#include "geom/mesh.hpp"
#include "render/mesh_renderer.hpp"
#include "tests/Test.hpp"

namespace test {
class TestSdfBlend : public Test {
 public:
  TestSdfBlend(const float screenWidth, const float screenHeight);
  ~TestSdfBlend();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  gfx::render::MeshRenderer renderer;
  std::unique_ptr<Shader> shaderSDF;
  std::unique_ptr<Shader> shaderModel;
  std::unique_ptr<gfx::geom::Mesh> sdfMesh;
  std::unique_ptr<gfx::geom::Mesh> modelMesh;
  float lightPos[3] = {4.0f, 5.0f, 3.0f};
  bool isLightMove = false;
  bool isShowSdf = true;
  bool isSphere = true;
  bool isShowModel = true;
  float size = 1.0f;
  float sdfColor[3] = {0.0f, 0.5f, 1.0f};    // blue
  float modelColor[3] = {1.0f, 0.5f, 0.0f};  // orange
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
