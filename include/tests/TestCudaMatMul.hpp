#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "ShaderClass.hpp"
#include "tests/Test.hpp"

namespace test {
class TestCudaMatMul : public Test {
 public:
  TestCudaMatMul(const float screenWidth, const float screenHeight);
  ~TestCudaMatMul();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  std::vector<glm::mat4> maInstanceMatrices;
  std::vector<glm::mat4> mbInstanceMatrices;
  std::vector<glm::mat4> tileInstanceMatrices;
  std::vector<glm::mat4> mcInstanceMatrices;
  std::vector<glm::mat4> frameInstanceMatrices;
  std::vector<glm::mat4> arrowInstanceMatrices;
  int M = 3;
  int N = 4;
  int K = 5;
  int TILE[2] = {2, 2};
  const float OFFSET = 3.0f;
  const float SEC_PER_BLOCK = 2.0f;
  const float ARROW_DEPTH = 5.0f;
  std::chrono::time_point<std::chrono::high_resolution_clock> startTS;
  std::unique_ptr<Shader> shaderProgram;
  std::unique_ptr<Mesh> matrixA;
  std::unique_ptr<Mesh> matrixB;
  std::unique_ptr<Mesh> tileMesh;
  std::unique_ptr<Mesh> matrixC;
  std::unique_ptr<Mesh> frame;
  std::unique_ptr<Mesh> arrow;
  std::unique_ptr<CameraEventListener> listener;

  void resetInstanceMatrices();
  void updateInstanceMatrices(float time);
};

}  // namespace test
