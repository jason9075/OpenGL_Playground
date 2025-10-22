#include "tests/TestCudaMatMul.hpp"

#include <OPPCH.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "BasicMesh.hpp"

namespace test {

TestCudaMatMul::TestCudaMatMul(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/cuda_matmul_vert.glsl", "./shaders/cuda_matmul_frag.glsl");
  matrixA = createCubeMesh(1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
  matrixB = createCubeMesh(1.0f, glm::vec3(0.0f, 0.0f, 0.8f));
  tileMesh = createCubeMesh(1.0f, glm::vec3(0.6f, 0.5f, 0.1f));
  matrixC = createCubeMesh(1.0f, glm::vec3(0.9f, 0.9f, 0.9f));
  frame = createFrameMesh(1 + OFFSET / 2, 1 + OFFSET / 2, 0.5f, 0.1f, glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  arrow = createArrowMesh(0.5f, 1.0f, 1.0f, 1.0f, 0.3f, glm::vec3(0.0f), glm::vec3(0.05f, 0.8f, 0.05f));
  arrow->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

  resetInstanceMatrices();

  matrixA->setupInstanceMatrices(maInstanceMatrices);
  matrixB->setupInstanceMatrices(mbInstanceMatrices);
  tileMesh->setupInstanceMatrices(tileInstanceMatrices);
  matrixC->setupInstanceMatrices(mcInstanceMatrices);
  frame->setupInstanceMatrices(frameInstanceMatrices);
  arrow->setupInstanceMatrices(arrowInstanceMatrices);

  glm::vec3 position = glm::vec3(20.0f, -30.0f, 50.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.57f, -0.82f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}

TestCudaMatMul::~TestCudaMatMul() {}

void TestCudaMatMul::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestCudaMatMul::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  float time =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTS)
          .count() /
      1000.0f;
  updateInstanceMatrices(time);

  shaderProgram->use();
  camera->update(shaderProgram.get());
  renderer.draw(*matrixA, *shaderProgram, M * N);
  renderer.draw(*matrixB, *shaderProgram, N * K);
  renderer.draw(*tileMesh, *shaderProgram, 2 * TILE[0] * TILE[1]);
  renderer.draw(*matrixC, *shaderProgram, M * K);
  // renderer.draw(*frame, *shaderProgram,2);
  renderer.draw(*arrow, *shaderProgram, TILE[0] * TILE[1]);
}

void TestCudaMatMul::OnImGuiRender() {
  ImGui::Text("Matrix A: %d x %d", M, N);
  ImGui::Text("Matrix B: %d x %d", N, K);
  ImGui::SliderInt("M", &M, 1, 10);
  ImGui::SliderInt("N", &N, 1, 10);
  ImGui::SliderInt("K", &K, 1, 10);
  ImGui::SliderInt2("W x H", TILE, 1, 10);
  if (ImGui::Button("Reset")) {
    resetInstanceMatrices();
    matrixA->setupInstanceMatrices(maInstanceMatrices);
    matrixB->setupInstanceMatrices(mbInstanceMatrices);
    tileMesh->setupInstanceMatrices(tileInstanceMatrices);
    matrixC->setupInstanceMatrices(mcInstanceMatrices);
    frame->setupInstanceMatrices(frameInstanceMatrices);
    arrow->setupInstanceMatrices(arrowInstanceMatrices);
  }
}

void TestCudaMatMul::OnExit() {}

// animation arrow
void TestCudaMatMul ::updateInstanceMatrices(float time) {
  int blockIdx = time / SEC_PER_BLOCK;
  // 4 actions per block
  int actionIdx = (int)(time / (SEC_PER_BLOCK / 4)) % 4;
  // std::cout << "BLOCK: " << blockIdx << " Action: " << actionIdx << std::endl;

  // Keyframe
  // row ceil
  int blockRows = M / TILE[1] + (M % TILE[1] == 0 ? 0 : 1);
  int blockCols = K / TILE[0] + (K % TILE[0] == 0 ? 0 : 1);
  int row = blockIdx / blockCols;
  int col = blockIdx % blockCols;
  // std::cout << "blockRows: " << blockRows << " blockCols: " << blockCols << std::endl;
  if (blockRows <= row) {
    return;
  }
  std::cout << "row: " << row << " col: " << col << std::endl;

  float arrowOffset[3] = {20.0f, 0.0f, ARROW_DEPTH};
  for (auto &model : arrowInstanceMatrices) {
    glm::vec3 offset = glm::vec3(arrowOffset[0] + OFFSET / 2.0f + col * TILE[0] * OFFSET,
                                 arrowOffset[1] + OFFSET / 2.0f + row * TILE[1] * OFFSET, arrowOffset[2]);
    model = glm::translate(glm::mat4(1.0f), offset);
  }

  arrow->updateInstanceMatrices(arrowInstanceMatrices);
}

void TestCudaMatMul::resetInstanceMatrices() {
  startTS = std::chrono::high_resolution_clock::now();
  maInstanceMatrices.clear();
  mbInstanceMatrices.clear();
  tileInstanceMatrices.clear();
  mcInstanceMatrices.clear();
  tileInstanceMatrices.clear();
  frameInstanceMatrices.clear();
  arrowInstanceMatrices.clear();

  std::random_device rd;

  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> scaleDis(0.5f, 0.9f);

  // Matrix
  float maOffset[2] = {0.0f, 0.0f};
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      glm::vec3 offset = glm::vec3(i * -OFFSET + maOffset[0], j * -OFFSET + maOffset[1], 0.0f);
      glm::mat4 model = glm::translate(glm::mat4(1.0f), offset);
      float scale = scaleDis(gen);
      model = glm::scale(model, glm::vec3(scale));
      maInstanceMatrices.push_back(model);
    }
  }

  float mbOffset[2] = {20.0f, 20.0f};
  for (int i = 0; i < K; i++) {
    for (int j = 0; j < N; j++) {
      glm::vec3 offset = glm::vec3(i * OFFSET + mbOffset[0], j * -OFFSET + mbOffset[1], 0.0f);
      glm::mat4 model = glm::translate(glm::mat4(1.0f), offset);
      float scale = scaleDis(gen);
      model = glm::scale(model, glm::vec3(scale));
      mbInstanceMatrices.push_back(model);
    }
  }

  float mcOffset[2] = {20.0f, 0.0f};
  for (int i = 0; i < K; i++) {
    for (int j = 0; j < M; j++) {
      glm::vec3 offset = glm::vec3(i * OFFSET + mcOffset[0], j * -OFFSET + mcOffset[1], 0.0f);
      mcInstanceMatrices.push_back(glm::translate(glm::mat4(1.0f), offset));
    }
  }

  // Tile Matrix

  float tileOffset[2] = {15.0f, 0.0f};
  for (int i = 0; i < TILE[0]; i++) {
    for (int j = 0; j < TILE[1]; j++) {
      glm::vec3 offset = glm::vec3(i * -OFFSET + tileOffset[0], j * -OFFSET + tileOffset[1], 0.0f);
      tileInstanceMatrices.push_back(glm::translate(glm::mat4(1.0f), offset));
    }
  }
  tileOffset[0] = 20.0f;
  tileOffset[1] = 5.0f;
  for (int i = 0; i < TILE[0]; i++) {
    for (int j = 0; j < TILE[1]; j++) {
      glm::vec3 offset = glm::vec3(i * OFFSET + tileOffset[0], j * OFFSET + tileOffset[1], 0.0f);
      tileInstanceMatrices.push_back(glm::translate(glm::mat4(1.0f), offset));
    }
  }

  // Frame

  // TODO: scale frame will make thickness of frame change
  float frameOffset[2] = {15.0f - 1.25f * TILE[0], 0.0f - 0.75f * TILE[1]};
  float scaleX = TILE[0] * 1.5 + 0.5;
  float scaleY = TILE[1] * 1.5 + 0.5;
  std::cout << "scaleX: " << scaleX << " scaleY: " << scaleY << std::endl;
  glm::mat4 frameModel = glm::translate(glm::mat4(1.0f), glm::vec3(frameOffset[0], frameOffset[1], 0.0f));
  frameModel = glm::scale(frameModel, glm::vec3(scaleX, scaleY, 1.0f));

  frameInstanceMatrices.push_back(frameModel);
  frameOffset[0] = 20.0f + 0.75 * TILE[0];
  frameOffset[1] = 5.0f + 0.75 * TILE[1];
  frameModel = glm::translate(glm::mat4(1.0f), glm::vec3(frameOffset[0], frameOffset[1], 0.0f));
  frameModel = glm::scale(frameModel, glm::vec3(scaleX, scaleY, 1.0f));
  frameInstanceMatrices.push_back(frameModel);

  // arrow
  float arrowOffset[3] = {20.0f, 0.0f, ARROW_DEPTH};
  for (int i = 0; i < TILE[0]; i++) {
    for (int j = 0; j < TILE[1]; j++) {
      glm::vec3 offset = glm::vec3(i * OFFSET + arrowOffset[0], j * -OFFSET + arrowOffset[1], arrowOffset[2]);
      arrowInstanceMatrices.push_back(glm::translate(glm::mat4(1.0f), offset));
    }
  }
}

}  // namespace test
