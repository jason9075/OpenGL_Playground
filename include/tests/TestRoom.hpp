#pragma once

#include "Camera.hpp"
#include "DataObject.hpp"
#include "ShaderClass.hpp"
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
  std::vector<Texture> textures;
  std::unique_ptr<Shader> shaderRoom;
  std::unique_ptr<Mesh> floor;
  std::unique_ptr<Mesh> northWall;
  std::unique_ptr<Mesh> southWall;
  std::unique_ptr<Mesh> eastWall;
  std::unique_ptr<Mesh> westWall;
  std::unique_ptr<Mesh> ceiling;
  std::unique_ptr<Mesh> groundMesh;
  const float backgroundColor[4] = {0.8f, 0.8f, 0.8f, 1.0f};
  std::unique_ptr<CameraEventListener> listener;
};

}  // namespace test
