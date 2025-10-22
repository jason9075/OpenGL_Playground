#pragma once

#include "Camera.hpp"
#include "ShaderClass.hpp"
#include "geom/mesh.hpp"
#include "render/mesh_renderer.hpp"
#include "tests/Test.hpp"

namespace test {
class TestRoom : public Test {
 public:
  TestRoom(const float screenWidth, const float screenHeight);
  ~TestRoom();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  gfx::render::MeshRenderer renderer;
  std::vector<gfx::resource::Texture> textures;
  std::unique_ptr<Shader> shaderRoom;
  std::unique_ptr<gfx::geom::Mesh> floor;
  std::unique_ptr<gfx::geom::Mesh> northWall;
  std::unique_ptr<gfx::geom::Mesh> southWall;
  std::unique_ptr<gfx::geom::Mesh> eastWall;
  std::unique_ptr<gfx::geom::Mesh> westWall;
  std::unique_ptr<gfx::geom::Mesh> ceiling;
  std::unique_ptr<gfx::geom::Mesh> groundMesh;
  const float backgroundColor[4] = {0.8f, 0.8f, 0.8f, 1.0f};
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
