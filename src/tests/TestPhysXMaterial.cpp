#include "tests/TestPhysXMaterial.hpp"

#include <OPPCH.h>
#include <PxPhysicsAPI.h>  // PhysX 一次到位的 header

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

TestPhysXMaterial::TestPhysXMaterial(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  // camera init
  glm::vec3 position = glm::vec3(30.0f, 13.0f, 8.0f);
  glm::vec3 orientation = glm::vec3(-0.7f, -0.25f, -0.5f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  shader = std::make_unique<Shader>("./shaders/physx_material_vert.glsl", "./shaders/physx_material_frag.glsl");
  shader->use();
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

  groundMesh = createPlaneMesh(100.0f, glm::vec3(0, 1, 0), glm::vec3(0.6f), glm::vec3(0.0f));

  // 斜坡角度（改這個就能快速測試）：繞 X 軸旋轉
  const float slopeDeg = 10.0f;
  const float slopeRad = glm::radians(slopeDeg);
  // 把世界 +Y 法向量繞 X 旋轉得到斜坡法線： (0, cosθ, sinθ)
  glm::vec3 slopeN = glm::normalize(glm::vec3(0.0f, std::cos(slopeRad), std::sin(slopeRad)));
  // 斜坡位置往 +Y 稍微抬高，避免與地板共面
  glm::vec3 slopePos = glm::vec3(-5.0f, 1.0f, -5.0f);

  rampMesh = createPlaneMesh(20.0f, slopeN, glm::vec3(0.82f), slopePos);

  initPhysX(slopeN, slopePos);

  mLast = std::chrono::high_resolution_clock::now();
  mAccumulatorSec = 0.0;
}

TestPhysXMaterial::~TestPhysXMaterial() { shutdownPhysX(); }

void TestPhysXMaterial::OnEvent(SDL_Event& event) { camera->handle(event); }

void TestPhysXMaterial::OnRender() {
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

  // 同步 PhysX → uniform
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
  glUniform1i(glGetUniformLocation(shader->ID, "useTexture"), false);
  rampMesh->draw(shader.get());

  // 地板：單位矩陣
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
  glUniform1i(glGetUniformLocation(shader->ID, "useTexture"), false);
  groundMesh->draw(shader.get());

  // Cube
  for (auto& c : mCubes) {
    const PxTransform pose = c.actor->getGlobalPose();
    const glm::mat4 M = pxToGlm(pose);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(M));
    glUniform1i(glGetUniformLocation(shader->ID, "useTexture"), c.mesh->hasTexture());
    c.mesh->draw(shader.get());
  }

  camera->moveCamera();
}

void TestPhysXMaterial::OnImGuiRender() {
  ImGui::Text(" Red: Rubber\n Brown: Wood\n Blue: Ice\n White: Steel");

  bool prev = mLockRolling;
  ImGui::Checkbox("Disable Rolling (slide only)", &mLockRolling);
  if (mLockRolling != prev) {
    for (auto& c : mCubes) {
      if (c.actor) applyRollingLock(c.actor, mLockRolling);
    }
  }
  if (ImGui::Button("Reset")) {
    for (auto& c : mCubes)
      if (c.actor) c.actor->release();
    mCubes.clear();
    spawnCubes();
    mScene->flushSimulation();
  }
}

void TestPhysXMaterial::OnExit() {
  if (shader) shader->del();
  if (groundMesh) groundMesh->del();
  shutdownPhysX();
}

// ------------------ PhysX helpers ------------------

void TestPhysXMaterial::initPhysX(glm::vec3 slopeN, glm::vec3 slopePos) {
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

  // Material of floor and ramp
  mMaterial = mPhysics->createMaterial(0.6f, 0.55f, 0.05f);
  mMaterial->setFrictionCombineMode(PxCombineMode::eMIN);
  mMaterial->setRestitutionCombineMode(PxCombineMode::eMAX);

  // Floor
  mGround = PxCreatePlane(*mPhysics, PxPlane(0, 1, 0, 0), *mMaterial);
  mScene->addActor(*mGround);

  // Ramp
  const PxVec3 n(slopeN.x, slopeN.y, slopeN.z);
  const float d = -glm::dot(slopeN, slopePos);
  PxPlane plane(n, d);
  PxTransform pose = PxTransformFromPlaneEquation(plane);
  mRamp = PxCreatePlane(*mPhysics, plane, *mMaterial);
  mRamp->setGlobalPose(pose);
  mScene->addActor(*mRamp);

  spawnCubes();
}

void TestPhysXMaterial::stepPhysics(double dtSec) {
  if (!mScene) return;
  mScene->simulate(static_cast<PxReal>(dtSec));
  mScene->fetchResults(true);
}

PxMaterial& TestPhysXMaterial::mkMat(float sf, float df, float rest) {
  PxMaterial* m = mPhysics->createMaterial(sf, df, rest);
  m->setFrictionCombineMode(PxCombineMode::eMIN);
  m->setRestitutionCombineMode(PxCombineMode::eMAX);
  mOwnedMaterials.push_back(m);
  return *m;
}

void TestPhysXMaterial::spawnCube(short cid, float scale, const glm::vec3& pos, const glm::vec3& color,
                                  PxMaterial& material, const char* texturePath) {
  // PhysX actor
  PxRigidDynamic* actor =
      PxCreateDynamic(*mPhysics, PxTransform(PxVec3(pos.x, pos.y, pos.z)), PxBoxGeometry(scale, scale, scale), material,
                      /*density*/ 1.0f);
  applyRollingLock(actor, mLockRolling);
  actor->setSolverIterationCounts(8, 2);
  actor->setLinearDamping(0.03f);
  actor->setAngularDamping(0.2f);
  mScene->addActor(*actor);

  auto mesh = createCubeMesh(scale, color);

  if (texturePath != nullptr) {
    std::vector<Texture> textures;
    textures.emplace_back(texturePath, "normal", 0);
    mesh->setTexture(textures);
  }
  mCubes.push_back(Cube{cid, actor, std::move(mesh), scale});
}

void TestPhysXMaterial::spawnCubes() {
  float height = 10.0f;
  // rubber, wood, ice, steel
  // 高摩擦低彈 （黏、幾乎不彈）
  spawnCube(0, 1.0f, glm::vec3(-5.0f, height, -15.0f), glm::vec3(0.9f, 0.2f, 0.2f), mkMat(2.0f, 1.8f, 0.05f), nullptr);
  // 中摩擦中彈 （中性)
  spawnCube(1, 1.0f, glm::vec3(0.0f, height, -15.0f), glm::vec3(0.6f, 0.4f, 0.2f), mkMat(0.6f, 0.45f, 0.2f),
            "./assets/textures/wooden.jpg");
  // 低摩擦高彈 （幾乎無摩擦；彈可高可低）
  spawnCube(2, 1.0f, glm::vec3(5.0f, height, -15.0f), glm::vec3(0.6f, 0.8f, 1.0f), mkMat(0.02f, 0.01f, 0.8f),
            "./assets/textures/ice.png");
  // 低摩擦低彈 （滑、不彈）
  spawnCube(3, 1.0f, glm::vec3(10.0f, height, -15.0f), glm::vec3(0.8f, 0.8f, 0.8f), mkMat(0.2f, 0.15f, 0.05f),
            "./assets/textures/steel.png");
}
void TestPhysXMaterial::applyRollingLock(PxRigidDynamic* actor, bool enable) {
  const PxRigidDynamicLockFlags flags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
                                        PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
                                        PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
  actor->setRigidDynamicLockFlags(enable ? flags : PxRigidDynamicLockFlags());
}
void TestPhysXMaterial::shutdownPhysX() {
  // 釋放順序：Actor -> Scene -> Dispatcher -> Material -> Physics -> Foundation
  for (auto& c : mCubes)
    if (c.actor) c.actor->release();
  mCubes.clear();

  if (mGround) {
    mGround->release();
    mGround = nullptr;
  }
  if (mRamp) {
    mRamp->release();
    mRamp = nullptr;
  }

  if (mScene) {
    mScene->release();
    mScene = nullptr;
  }
  if (mDispatcher) {
    mDispatcher->release();
    mDispatcher = nullptr;
  }

  // 釋放測試材質
  for (auto* mat : mOwnedMaterials)
    if (mat) mat->release();
  mOwnedMaterials.clear();

  if (mMaterial) {
    mMaterial->release();
    mMaterial = nullptr;
  }
  if (mPhysics) {
    mPhysics->release();
    mPhysics = nullptr;
  }
  if (mFoundation) {
    mFoundation->release();
    mFoundation = nullptr;
  }
}
}  // namespace test
