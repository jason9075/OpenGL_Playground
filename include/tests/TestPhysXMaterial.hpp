#pragma once

#include <PxPhysicsAPI.h>

#include <chrono>
#include <memory>

#include "Camera.hpp"
#include "DataObject.hpp"
#include "ShaderClass.hpp"
#include "tests/Test.hpp"

namespace test {
class TestPhysXMaterial : public Test {
 public:
  TestPhysXMaterial(const float screenWidth, const float screenHeight);
  ~TestPhysXMaterial();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  std::unique_ptr<Shader> shader;
  std::unique_ptr<CameraEventListener> listener;

  struct Cube {
    short cid;
    physx::PxRigidDynamic* actor = nullptr;
    std::unique_ptr<Mesh> mesh;
    float scale;
  };
  std::vector<Cube> mCubes;
  std::unique_ptr<Mesh> groundMesh;
  std::unique_ptr<Mesh> rampMesh;

  // --- PhysX core pointers ---
  physx::PxFoundation* mFoundation = nullptr;
  physx::PxPhysics* mPhysics = nullptr;
  physx::PxScene* mScene = nullptr;
  physx::PxMaterial* mMaterial = nullptr;
  physx::PxDefaultCpuDispatcher* mDispatcher = nullptr;
  std::vector<physx::PxMaterial*> mOwnedMaterials;
  bool mLockRolling = false;  // 預設禁止滾動（只滑）

  // Actors
  physx::PxRigidStatic* mGround = nullptr;
  physx::PxRigidStatic* mRamp = nullptr;

  // --- Timing for fixed-step simulation (60 Hz) ---
  std::chrono::high_resolution_clock::time_point mLast;
  double mAccumulatorSec = 0.0;
  static constexpr double kFixedDt = 1.0 / 60.0;

  // Helper
  void initPhysX(glm::vec3 slopeN, glm::vec3 slopePos);
  void stepPhysics(double dtSec);
  void shutdownPhysX();
  void applyRollingLock(physx::PxRigidDynamic* actor, bool enable);
  physx::PxMaterial& mkMat(float sf, float df, float rest);
  void spawnCube(short cid, float scale, const glm::vec3& pos, const glm::vec3& color, physx::PxMaterial& material,
                 const char* texturePath);
  void spawnCubes();

  // Disable copy
  TestPhysXMaterial(const TestPhysXMaterial&) = delete;
  TestPhysXMaterial& operator=(const TestPhysXMaterial&) = delete;
};

}  // namespace test
