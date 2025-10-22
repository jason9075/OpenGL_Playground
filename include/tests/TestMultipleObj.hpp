#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "ShaderClass.hpp"
#include "render/mesh_renderer.hpp"
#include "render/model_renderer.hpp"
#include "tests/Test.hpp"

namespace test {
class TestMultipleObj : public Test {
 public:
  TestMultipleObj(const float screenWidth, const float screenHeight);
  ~TestMultipleObj();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  std::vector<glm::mat4> instanceMatrices;
  // Note: if you want to use lower row and col, you have to use setupInstanceMatrices instead of updateInstanceMatrices
  int row = 100;
  int col = 100;
  const float OFFSET = 3.0f;
  gfx::render::MeshRenderer mesh_renderer;
  gfx::render::ModelRenderer renderer;
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Model> model;
  std::unique_ptr<CameraEventListener> listener;

  void updateInstanceMatrices();
};

}  // namespace test
