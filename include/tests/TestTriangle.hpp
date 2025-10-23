#pragma once

#include "Camera.hpp"
#include "ShaderClass.hpp"
#include "geom/mesh.hpp"  // 你的 Mesh
#include "render/mesh_renderer.hpp"
#include "tests/Test.hpp"

namespace test {
class TestTriangle : public Test {
 public:
  TestTriangle(const float screenWidth, const float screenHeight);
  ~TestTriangle();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<gfx::geom::Mesh> mesh;
  std::unique_ptr<CameraEventListener> listener;
  gfx::render::MeshRenderer renderer;
  float backgroundColor[3] = {0.0f, 0.0f, 0.0f};
  float triangleColor[3] = {0.2f, 0.8f, 0.2f};
};

}  // namespace test
