#pragma once

#include "Camera.hpp"
#include "ShaderClass.hpp"
#include "geom/mesh.hpp"
#include "render/mesh_renderer.hpp"
#include "tests/Test.hpp"

namespace test {
class TestParallaxMapping : public Test {
 public:
  TestParallaxMapping(const float screenWidth, const float screenHeight);
  ~TestParallaxMapping();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnReload() override;
  void OnExit() override;

 private:
  gfx::render::MeshRenderer renderer;
  float farFactor = 0.5f;
  std::unique_ptr<Shader> shader;
  std::unique_ptr<gfx::geom::Mesh> wall;
  std::unique_ptr<gfx::geom::Mesh> floor;
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
