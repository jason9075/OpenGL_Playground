#include "tests/TestPhysXHelloWorld.hpp"

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

TestPhysXHelloWorld::TestPhysXHelloWorld(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  // camera init
  glm::vec3 position = glm::vec3(6.0f, 3.0f, 8.0f);
  glm::vec3 orientation = glm::vec3(-0.5f, -0.1f, -0.7f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> oneRange{0.0f, 1.0f};

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  shader = std::make_unique<Shader>("./shaders/physx_hello_vert.glsl", "./shaders/physx_hello_frag.glsl");
  shader->use();
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

  groundMesh = createPlaneMesh(100.0f, glm::vec3(0, 1, 0), glm::vec3(0.6f), glm::vec3(0.0f));

  initPhysX();

  mLast = std::chrono::high_resolution_clock::now();
  mAccumulatorSec = 0.0;
}

TestPhysXHelloWorld::~TestPhysXHelloWorld() { shutdownPhysX(); }

void TestPhysXHelloWorld::OnEvent(SDL_Event& event) { camera->handle(event); }

void TestPhysXHelloWorld::OnRender() {
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
  // 先設 modelMatrix，再畫球
  for (auto& b : mBalls) {
    const PxTransform pose = b.actor->getGlobalPose();
    const glm::mat4 M = pxToGlm(pose);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(M));
    glUniform1i(glGetUniformLocation(shader->ID, "classId"), 1);
    b.mesh->draw(shader.get());
  }

  // 地板：單位矩陣
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
  glUniform1i(glGetUniformLocation(shader->ID, "classId"), 2);
  groundMesh->draw(shader.get());

  camera->moveCamera();
}

void TestPhysXHelloWorld::OnImGuiRender() {
  ImGui::Text("Ball Count: %d", int(mBalls.size()));
  if (ImGui::Button("Add ball")) {
    spawnBall(0.5, glm::vec3(oneRange(rd), 5.0f, oneRange(rd)), glm::vec3(0.9f, 0.2f, 0.2f));
  }
  if (ImGui::Button("Reset")) {
    for (auto& b : mBalls)
      if (b.actor) b.actor->release();
    mBalls.clear();
    mScene->flushSimulation();
  }
}

void TestPhysXHelloWorld::OnExit() {
  if (shader) shader->del();
  if (groundMesh) groundMesh->del();
  for (auto& b : mBalls) b.mesh->del();
  shutdownPhysX();
}

// ------------------ PhysX helpers ------------------

void TestPhysXHelloWorld::initPhysX() {
  // 1) Foundation
  mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

  // 2) Physics（這裡不啟動 PVD）
  PxTolerancesScale scale;
  mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, scale, true, nullptr);

  // 3) Dispatcher（CPU 工作緒）
  mDispatcher = PxDefaultCpuDispatcherCreate(std::max(2u, std::thread::hardware_concurrency()));

  // 4) Scene
  PxSceneDesc sceneDesc(scale);
  sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
  sceneDesc.cpuDispatcher = mDispatcher;
  sceneDesc.filterShader = PxDefaultSimulationFilterShader;
  mScene = mPhysics->createScene(sceneDesc);

  // 5) 材質
  mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);

  // 6) 地板（Extensions 內建 helper）
  mGround = PxCreatePlane(*mPhysics, PxPlane(0, 1, 0, 0), *mMaterial);
  mScene->addActor(*mGround);

  // 7) 一顆球
  spawnBall(0.5f, glm::vec3(0, 5, 0));
}

void TestPhysXHelloWorld::stepPhysics(double dtSec) {
  if (!mScene) return;
  mScene->simulate(static_cast<PxReal>(dtSec));
  mScene->fetchResults(true);
}

void TestPhysXHelloWorld::spawnBall(float radius, const glm::vec3& pos, const glm::vec3& color) {
  // PhysX actor
  PxRigidDynamic* actor =
      PxCreateDynamic(*mPhysics, PxTransform(PxVec3(pos.x, pos.y, pos.z)), PxSphereGeometry(radius), *mMaterial,
                      /*density*/ 1.0f);
  actor->setAngularDamping(0.2f);
  actor->setLinearDamping(0.05f);
  mScene->addActor(*actor);

  // Render mesh（你剛加的 createSphereMesh）
  auto mesh =
      createSphereMesh(radius, glm::vec3(0.0f), glm::vec3(oneRange(rd), oneRange(rd), oneRange(rd)), /*rings*/ 24,
                       /*sectors*/ 48);
  mBalls.push_back(Ball{actor, std::move(mesh), radius});
}

void TestPhysXHelloWorld::shutdownPhysX() {
  // 釋放順序：Actor -> Scene -> Dispatcher -> Material -> Physics -> Foundation
  for (auto& b : mBalls) {
    if (b.actor) b.actor->release();
  }
  mBalls.clear();
  if (mGround) {
    mGround->release();
    mGround = nullptr;
  }

  if (mScene) {
    mScene->release();
    mScene = nullptr;
  }
  if (mDispatcher) {
    mDispatcher->release();
    mDispatcher = nullptr;
  }

  // mMaterial 由 Physics 擁有，但獨立 release 也可
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
