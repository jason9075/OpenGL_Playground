#pragma once

#include <PxPhysicsAPI.h>

#include <chrono>
#include <memory>

#include "Camera.hpp"
#include "ShaderClass.hpp"
#include "render/mesh_renderer.hpp"
#include "tests/Test.hpp"

namespace test {
class TestPhysXPendulum : public Test {
 public:
  TestPhysXPendulum(const float screenWidth, const float screenHeight);
  ~TestPhysXPendulum();

  void OnEvent(SDL_Event& event) override;
  void OnRender() override;
  void OnImGuiRender() override;
  void OnExit() override;

 private:
  gfx::render::MeshRenderer renderer;
  std::unique_ptr<Shader> shader;
  std::unique_ptr<CameraEventListener> listener;

  struct BodyVis {
    physx::PxRigidActor* actor = nullptr;  // 包含 static/dynamic 或 articulation link 的 PxRigidActor 基類
    std::unique_ptr<gfx::geom::Mesh> mesh;
    glm::vec3 scale = glm::vec3(1.0f);
  };

  // ---- Scene resources ----
  physx::PxMaterial* mMaterial = nullptr;
  physx::PxRigidStatic* mGround = nullptr;

  // ---- Joint chain (classic) ----
  std::vector<physx::PxRigidDynamic*> mDynLinks;
  std::vector<physx::PxJoint*> mJoints;  // 釋放時用
  physx::PxRigidStatic* mAnchor = nullptr;

  // ---- Articulation (RC) ----
  physx::PxArticulationReducedCoordinate* mArticulation = nullptr;
  std::vector<physx::PxArticulationLink*> mArtLinks;

  std::vector<BodyVis> mBodies;

  physx::PxFoundation* mFoundation = nullptr;
  physx::PxPhysics* mPhysics = nullptr;
  physx::PxScene* mScene = nullptr;
  physx::PxJoint* mJoint = nullptr;
  physx::PxDefaultCpuDispatcher* mDispatcher = nullptr;
  std::vector<physx::PxJoint*> mOwnedJoints;

  // Actors

  // --- Timing for fixed-step simulation (60 Hz) ---
  std::chrono::high_resolution_clock::time_point mLast;
  double mAccumulatorSec = 0.0;
  static constexpr double kFixedDt = 1.0 / 60.0;

  // Joint/Articulation 共用參數
  int N = 3;
  float linkLen = 2.0f;
  float halfW = 0.15f;
  float density = 100.0f;

  // Articulation Drive UI 狀態
  float drvStiffness = 500.0f;
  float drvDamping = 10.0f;
  float drvMaxForce = 5000.0f;
  float drvTargetRad = 0.5f;  // 目標角度(弧度)

  // Helper
  void initPhysX();
  void stepPhysics(double dtSec);
  void shutdownPhysX();

  void addGround();
  void clearPendulum();
  void buildNLinkPendulum_Articulation(int N, float linkLen, float linkHalfW, float density);
  void buildNLinkPendulum_Joints(int N, float linkLen, float linkHalfW, float density);
};

}  // namespace test
