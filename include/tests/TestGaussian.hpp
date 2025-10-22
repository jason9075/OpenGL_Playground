#pragma once

#include "Camera.hpp"
#include "geom/gaussianSplat.hpp"
#include "render/gs_renderer.hpp"
#include "tests/Test.hpp"

namespace test {
class TestGaussian : public Test {
 public:
  TestGaussian(const float screenWidth, const float screenHeight);
  ~TestGaussian();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;

 private:
  gfx::render::GsRenderer renderer;
  float scaleFactor = 1.0f;
  std::unique_ptr<gfx::geom::GaussianSplat> splat;
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<CameraEventListener> listener;
};
}  // namespace test
