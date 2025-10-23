#pragma once

#include <PxPhysicsAPI.h>

#include <chrono>
#include <memory>

#include "Camera.hpp"
#include "ShaderClass.hpp"
#include "geom/mesh.hpp"
#include "render/mesh_renderer.hpp"
#include "tests/Test.hpp"

namespace test {
class TestPhysXHelloWorld : public Test {
 public:
  TestPhysXHelloWorld(const float screenWidth, const float screenHeight);
  ~TestPhysXHelloWorld();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  gfx::render::MeshRenderer renderer;
  std::unique_ptr<Shader> shader;
  std::unique_ptr<CameraEventListener> listener;

  std::unique_ptr<gfx::geom::Mesh> groundMesh;
  struct Ball {
    physx::PxRigidDynamic* actor = nullptr;
    std::unique_ptr<gfx::geom::Mesh> mesh;
    float radius = 0.5f;
  };
  std::vector<Ball> mBalls;

  std::random_device rd;
  std::uniform_real_distribution<float> oneRange;
  // --- PhysX core pointers ---
  physx::PxFoundation* mFoundation = nullptr;
  physx::PxPhysics* mPhysics = nullptr;
  physx::PxScene* mScene = nullptr;
  physx::PxMaterial* mMaterial = nullptr;
  physx::PxDefaultCpuDispatcher* mDispatcher = nullptr;

  // Actors
  physx::PxRigidStatic* mGround = nullptr;

  // --- Timing for fixed-step simulation (60 Hz) ---
  std::chrono::high_resolution_clock::time_point mLast;
  double mAccumulatorSec = 0.0;
  static constexpr double kFixedDt = 1.0 / 60.0;

  // Helper
  void initPhysX();
  void stepPhysics(double dtSec);
  void shutdownPhysX();
  void spawnBall(float radius, const glm::vec3& pos, const glm::vec3& color = glm::vec3(0.9f, 0.2f, 0.2f));
};

}  // namespace test
