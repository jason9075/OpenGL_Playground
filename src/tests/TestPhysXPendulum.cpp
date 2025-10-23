#include "tests/TestPhysXPendulum.hpp"

#include <OPPCH.h>
#include <PxPhysicsAPI.h>
#include <extensions/PxRigidActorExt.h>
#include <extensions/PxRigidBodyExt.h>

#include "BasicMesh.hpp"
using namespace physx;

namespace {

// 用 PxMat44 直接轉（PhysX/GLM 都是 column-major）
inline glm::mat4 pxToGlm(const physx::PxTransform& t) {
  physx::PxMat44 m(t);  // column0..3
  glm::mat4 out(1.0f);
  out[0] = glm::vec4(m.column0.x, m.column0.y, m.column0.z, m.column0.w);
  out[1] = glm::vec4(m.column1.x, m.column1.y, m.column1.z, m.column1.w);
  out[2] = glm::vec4(m.column2.x, m.column2.y, m.column2.z, m.column2.w);
  out[3] = glm::vec4(m.column3.x, m.column3.y, m.column3.z, m.column3.w);
  return out;
}

// 用位移 + 四元數（等價；看你喜歡哪個）
inline glm::mat4 pxToGlmTR(const physx::PxTransform& t, const glm::vec3& scale = glm::vec3(1.0f)) {
  const physx::PxVec3& p = t.p;
  const physx::PxQuat& q = t.q;
  glm::vec3 T(p.x, p.y, p.z);
  glm::quat R(q.w, q.x, q.y, q.z);  // glm::quat(w,x,y,z)
  glm::mat4 M = glm::translate(glm::mat4(1.0f), T) * glm::mat4_cast(R) * glm::scale(glm::mat4(1.0f), scale);
  return M;
}

// PhysX 需要 allocator / error callback 的實例（可做成單例/靜態）
static PxDefaultAllocator gAllocator;
static PxDefaultErrorCallback gErrorCallback;

}  // namespace

namespace test {

TestPhysXPendulum::TestPhysXPendulum(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  // camera init
  glm::vec3 position = glm::vec3(20.0f, 5.0f, 3.0f);
  glm::vec3 orientation = glm::vec3(-1.0f, 0.0f, -0.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  shader = std::make_unique<Shader>("./shaders/physx_pendulum_vert.glsl", "./shaders/physx_pendulum_frag.glsl");
  shader->use();
  glUniformMatrix4fv(glGetUniformLocation(shader->PROGRAM_ID, "modelMatrix"), 1, GL_FALSE,
                     glm::value_ptr(glm::mat4(1.0f)));

  initPhysX();

  mLast = std::chrono::high_resolution_clock::now();
  mAccumulatorSec = 0.0;
}

TestPhysXPendulum::~TestPhysXPendulum() { shutdownPhysX(); }

void TestPhysXPendulum::OnEvent(SDL_Event& event) { camera->handle(event); }

void TestPhysXPendulum::OnRender() {
  // --- fixed-step 物理 ---
  auto now = std::chrono::high_resolution_clock::now();
  double dtSec = std::chrono::duration<double>(now - mLast).count();
  mLast = now;

  // 限制極端卡頓造成的巨量步數
  if (dtSec > 0.25) dtSec = 0.25;

  mAccumulatorSec += dtSec;
  while (mAccumulatorSec >= kFixedDt) {
    stepPhysics(kFixedDt);
    mAccumulatorSec -= kFixedDt;
  }

  // Clear Graphic
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 繪製
  shader->use();
  camera->update(shader.get());

  for (auto& b : mBodies) {
    glm::mat4 M;
    if (b.actor == mGround) {
      M = glm::mat4(1.0f);  // 地板用單位矩陣
    } else {
      PxTransform pose = b.actor->getGlobalPose();
      M = pxToGlmTR(pose, b.scale);
    }
    glUniformMatrix4fv(glGetUniformLocation(shader->PROGRAM_ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(M));
    renderer.draw(*b.mesh, *shader);
  }
  camera->moveCamera();
}

void TestPhysXPendulum::OnImGuiRender() {
  ImGui::Text("N-link Pendulum\nBlue: Articulation\nGreen: Joint");
  ImGui::SliderInt("N", &N, 1, 10);
  ImGui::SliderFloat("Link Length", &linkLen, 0.5f, 5.0f);
  ImGui::SliderFloat("Half Width", &halfW, 0.05f, 0.5f);
  ImGui::SliderFloat("Density", &density, 10.0f, 1000.0f);

  if (ImGui::Button("Reset Scene")) {
    clearPendulum();
    mScene->flushSimulation();
    buildNLinkPendulum_Articulation(N, linkLen, halfW, density);
    buildNLinkPendulum_Joints(N, linkLen, halfW, density);
  }

  ImGui::Separator();

  // -------- Articulation (第一個方棒的 drive) --------
  if (ImGui::CollapsingHeader("Articulation Drive", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::SliderFloat("Stiffness (kp)", &drvStiffness, 0.0f, 5000.0f);
    ImGui::SliderFloat("Damping (kd)", &drvDamping, 0.0f, 500.0f);
    ImGui::SliderFloat("MaxForce", &drvMaxForce, 0.0f, 20000.0f);
    ImGui::SliderFloat("Target (rad)", &drvTargetRad, -3.14f, 3.14f);

    if (ImGui::Button("Apply Drive to link[1]/TWIST")) {
      if (mArticulation && mArtLinks.size() > 1) {
        auto* j = static_cast<PxArticulationJointReducedCoordinate*>(mArtLinks[1]->getInboundJoint());
        if (j) {
          PxArticulationDrive drv(drvStiffness, drvDamping, drvMaxForce, PxArticulationDriveType::eFORCE);
          j->setDriveParams(PxArticulationAxis::eTWIST, drv);
          j->setDriveTarget(PxArticulationAxis::eTWIST, drvTargetRad);
        }
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Release Drive")) {
      if (mArticulation && mArtLinks.size() > 1) {
        auto* j = static_cast<PxArticulationJointReducedCoordinate*>(mArtLinks[1]->getInboundJoint());
        if (j) {
          PxArticulationDrive none(0.0f, 0.0f, 0.0f, PxArticulationDriveType::eFORCE);
          j->setDriveParams(PxArticulationAxis::eTWIST, none);
          j->setDriveVelocity(PxArticulationAxis::eTWIST, 0.0f);
        }
      }
    }

    // （可選）顯示目前角度
    if (mArticulation && mArtLinks.size() > 1) {
      auto* j = static_cast<PxArticulationJointReducedCoordinate*>(mArtLinks[1]->getInboundJoint());
      if (j) {
        float q = j->getJointPosition(PxArticulationAxis::eTWIST);
        ImGui::Text("Current TWIST: %.3f rad", q);
      }
    }
  }

  ImGui::Separator();

  // -------- Joint Chain 控制 --------
  if (ImGui::CollapsingHeader("Joint Chain Control", ImGuiTreeNodeFlags_DefaultOpen)) {
    static float kickW = 15.0f;  // rad/s
    ImGui::SetNextItemWidth(150);
    ImGui::SliderFloat("Kick ang vel (rad/s)", &kickW, -50.0f, 50.0f);
    if (ImGui::Button("Kick Joint Chain (link[0])")) {
      if (!mDynLinks.empty()) {
        // Revolute 轉軸在 joint local X，這裡給 X 角速度
        mDynLinks[0]->setAngularVelocity(PxVec3(kickW, 0.0f, 0.0f));
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Damp All (sleep-ish)")) {
      // 簡單阻尼：把所有 dynamic 的角速度/線速度衰減
      for (auto* a : mDynLinks)
        if (a) {
          a->setAngularVelocity(a->getAngularVelocity() * 0.1f);
          a->setLinearVelocity(a->getLinearVelocity() * 0.1f);
        }
    }
  }
}

void TestPhysXPendulum::OnExit() { shutdownPhysX(); }

// ------------------ PhysX helpers ------------------

void TestPhysXPendulum::initPhysX() {
  // Foundation
  mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

  // Physics（這裡不啟動 PVD）
  PxTolerancesScale scale;
  mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, scale, true, nullptr);

  // Dispatcher（CPU 工作緒）
  mDispatcher = PxDefaultCpuDispatcherCreate(std::max(2u, std::thread::hardware_concurrency()));

  // Scene
  PxSceneDesc sceneDesc(scale);
  sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
  sceneDesc.cpuDispatcher = mDispatcher;
  sceneDesc.filterShader = PxDefaultSimulationFilterShader;
  mScene = mPhysics->createScene(sceneDesc);

  // Material
  mMaterial = mPhysics->createMaterial(0.6f, 0.6f, 0.1f);  // staticFric, dynamicFric, restitution

  addGround();

  buildNLinkPendulum_Articulation(N, linkLen, halfW, density);
  buildNLinkPendulum_Joints(N, linkLen, halfW, density);
}

void TestPhysXPendulum::addGround() {
  // y=0 的無限平面
  mGround = PxCreatePlane(*mPhysics, PxPlane(0, 1, 0, 0), *mMaterial);
  mScene->addActor(*mGround);

  // 視覺：大平面
  BodyVis vis;
  vis.actor = mGround;
  vis.mesh = createPlaneMesh(/*scale=*/80.0f, /*normal=*/glm::vec3(0, 1, 0), /*color=*/glm::vec3(0.75f));

  vis.scale = glm::vec3(1.0f);
  mBodies.push_back(std::move(vis));
}

void TestPhysXPendulum::clearPendulum() {
  // joints
  for (auto* j : mJoints)
    if (j) j->release();
  mJoints.clear();

  // articulation
  for (auto* l : mArtLinks) l = nullptr;
  mArtLinks.clear();
  if (mArticulation) {
    mArticulation->release();
    mArticulation = nullptr;
  }

  // classic chain actors
  for (auto* a : mDynLinks)
    if (a) {
      a->release();
    }
  mDynLinks.clear();
  if (mAnchor) {
    mAnchor->release();
    mAnchor = nullptr;
  }

  // 視覺刪除除了地板之外的 body
  std::vector<BodyVis> kept;
  kept.reserve(mBodies.size());
  for (auto& b : mBodies) {
    if (b.actor == mGround) {
      kept.push_back(std::move(b));
    }
  }
  mBodies.swap(kept);
}

void TestPhysXPendulum::buildNLinkPendulum_Articulation(int N, float linkLen, float linkHalfW, float density) {
  mArticulation = mPhysics->createArticulationReducedCoordinate();
  mArticulation->setArticulationFlag(PxArticulationFlag::eFIX_BASE, true);
  // global damping 等通用參數可視需要設定
  mArticulation->setSolverIterationCounts(16, 4);  // more stable for long chains

  const float startY = 10.0f;
  const float offsetZ = 5.0f;
  PxTransform basePose(PxVec3(0, startY, offsetZ));

  // root link (固定在世界位置、不受重力) → root link 一樣是 link，但 DOF 設 0
  PxArticulationLink* root = mArticulation->createLink(nullptr, basePose);
  {
    PxRigidActorExt::createExclusiveShape(*root, PxBoxGeometry(linkHalfW, linkHalfW, linkHalfW), *mMaterial);
    PxRigidBodyExt::updateMassAndInertia(*root, density);
    root->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
  }
  mArtLinks.push_back(root);

  // 視覺（root 小方塊）
  {
    BodyVis vis;
    vis.actor = root;
    vis.mesh = createCuboidMesh(linkHalfW * 2, linkHalfW * 2, linkHalfW * 2, /*pos=*/glm::vec3(0),
                                /*color=*/glm::vec3(0.8f, 0.4f, 0.2f));
    vis.scale = glm::vec3(1.0f);
    mBodies.push_back(std::move(vis));
  }

  // 逐節建立：每節 link 的本地錨點在其上端，parent 端錨點在其下端
  PxArticulationLink* parent = root;
  for (int i = 0; i < N; ++i) {
    // 本節 link 的 COM 置於桿中心
    PxTransform childPose = PxTransform(basePose.p + PxVec3(0, -(i + 0.5f) * linkLen, 0));
    PxArticulationLink* child = mArticulation->createLink(parent, childPose);

    // 幾何：細長方棒（沿 Y）
    PxRigidActorExt::createExclusiveShape(*child, PxBoxGeometry(linkHalfW, linkLen * 0.5f, linkHalfW), *mMaterial);

    // 設定 mass & inertia
    PxRigidBodyExt::updateMassAndInertia(*child, density);

    // 關節：用 RC Joint 設定 1 DOF（Z 軸旋轉，平面擺動）
    PxArticulationJointReducedCoordinate* j =
        static_cast<PxArticulationJointReducedCoordinate*>(child->getInboundJoint());
    // parent frame：位於 parent 的下端；child frame：位於 child 的上端
    const PxTransform parentF(PxVec3(0, -(i == 0 ? linkHalfW : linkLen * 0.5f), 0));
    const PxTransform childF(PxVec3(0, +linkLen * 0.5f, 0));
    j->setParentPose(parentF);
    j->setChildPose(childF);

    j->setJointType(PxArticulationJointType::eREVOLUTE);
    j->setMotion(PxArticulationAxis::eTWIST, PxArticulationMotion::eFREE);     // X
    j->setMotion(PxArticulationAxis::eSWING1, PxArticulationMotion::eLOCKED);  // Y
    j->setMotion(PxArticulationAxis::eSWING2, PxArticulationMotion::eLOCKED);  // Z

    // 可選：阻尼/驅動，讓它比較不亂抖
    PxArticulationDrive drive(/*stiffness*/ 0.0f,
                              /*damping*/ 0.0f,
                              /*maxForce*/ 0.0f,
                              /*driveType*/ PxArticulationDriveType::eFORCE);
    j->setDriveParams(PxArticulationAxis::eTWIST, drive);

    // 視覺
    {
      BodyVis vis;
      vis.actor = child;
      vis.mesh = createCuboidMesh(linkHalfW * 2, linkLen, linkHalfW * 2,
                                  /*pos=*/glm::vec3(0), /*color=*/glm::vec3(0.7f, 0.8f, 1.0f));
      vis.scale = glm::vec3(1.0f);
      mBodies.push_back(std::move(vis));
    }

    mArtLinks.push_back(child);
    parent = child;
  }

  // 把整套 articulation 丟進 scene
  mScene->addArticulation(*mArticulation);
}

void TestPhysXPendulum::buildNLinkPendulum_Joints(int N, float linkLen, float linkHalfW, float density) {
  // Anchor：世界固定點
  const float startY = 10.0f;
  const float offsetZ = -5.0f;
  mAnchor = mPhysics->createRigidStatic(PxTransform(PxVec3(0, startY, offsetZ)));
  {
    // 小方塊形狀只是方便看
    PxRigidActorExt::createExclusiveShape(*mAnchor, PxBoxGeometry(linkHalfW, linkHalfW, linkHalfW), *mMaterial);
    mScene->addActor(*mAnchor);

    BodyVis vis;
    vis.actor = mAnchor;
    vis.mesh = createCuboidMesh(linkHalfW * 2, linkHalfW * 2, linkHalfW * 2, glm::vec3(0), glm::vec3(0.8f, 0.4f, 0.2f));
    mBodies.push_back(std::move(vis));
  }

  PxRigidActor* parent = mAnchor;
  PxVec3 parentPivotLocal(0, -linkHalfW, 0);  // anchor 下端

  for (int i = 0; i < N; ++i) {
    // 動態方棒
    PxTransform pose(PxVec3(0, 10.0f - (i + 0.5f) * linkLen, offsetZ));
    PxRigidDynamic* link = mPhysics->createRigidDynamic(pose);
    PxRigidActorExt::createExclusiveShape(*link, PxBoxGeometry(linkHalfW, linkLen * 0.5f, linkHalfW), *mMaterial);
    PxRigidBodyExt::updateMassAndInertia(*link, density);
    mScene->addActor(*link);
    mDynLinks.push_back(link);

    // Revolute joint around Z
    PxTransform parentLocal(parentPivotLocal);
    PxTransform childLocal(PxVec3(0, +linkLen * 0.5f, 0));
    PxRevoluteJoint* j = PxRevoluteJointCreate(*mPhysics, parent, parentLocal, link, childLocal);
    j->setDriveVelocity(0.0f);
    j->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, false);
    // 限制 ±150°
    PxJointAngularLimitPair limit(-PxPi * 5.0f / 6.0f, PxPi * 5.0f / 6.0f);
    j->setLimit(limit);
    j->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);

    mJoints.push_back(j);

    // 視覺
    {
      BodyVis vis;
      vis.actor = link;
      vis.mesh = createCuboidMesh(linkHalfW * 2, linkLen, linkHalfW * 2, glm::vec3(0), glm::vec3(0.7f, 1.0f, 0.7f));
      vis.scale = glm::vec3(1.0f);
      mBodies.push_back(std::move(vis));
    }

    parent = link;
    parentPivotLocal = PxVec3(0, -linkLen * 0.5f, 0);  // 之後每節的 pivot 都在下端
  }
}

void TestPhysXPendulum::stepPhysics(double dtSec) {
  if (!mScene) return;
  mScene->simulate(static_cast<PxReal>(dtSec));
  mScene->fetchResults(true);
}

void TestPhysXPendulum::shutdownPhysX() {
  // 刪 pendulum 與視覺（保留地板已在 clearPendulum 處理）
  clearPendulum();

  // 地板
  if (mGround) {
    mGround->release();
    mGround = nullptr;
  }

  // Material
  if (mMaterial) {
    mMaterial->release();
    mMaterial = nullptr;
  }

  // Scene/Dispatcher/Physics/Foundation
  if (mScene) {
    mScene->release();
    mScene = nullptr;
  }
  if (mDispatcher) {
    mDispatcher->release();
    mDispatcher = nullptr;
  }
  if (mPhysics) {
    mPhysics->release();
    mPhysics = nullptr;
  }
  if (mFoundation) {
    mFoundation->release();
    mFoundation = nullptr;
  }

  mBodies.clear();
  mOwnedJoints.clear();  // 若你有別處 push 的 joints
  mJoint = nullptr;
}
}  // namespace test
