#include "tests/TestRtSphere.hpp"

#include <OPPCH.h>

#include <memory>

#include "BasicMesh.hpp"

namespace test {

TestRtSphere::TestRtSphere(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/rt_sphere_vert.glsl", "./shaders/rt_sphere_frag.glsl");
  frameShader = std::make_unique<Shader>("./shaders/acc_framebuffer_vert.glsl", "./shaders/acc_framebuffer_frag.glsl");

  glm::vec3 position = glm::vec3(9.0f, 1.0f, 15.0f);
  glm::vec3 orientation = glm::vec3(-0.6f, 0.0f, -0.8f);
  camera = std::make_shared<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  rtMesh = createPlaneMesh();

  // setup FBO
  oldFrame = std::make_unique<FBO>(screenWidth, screenHeight);
  oldFrame->setupTexture();

  newFrame = std::make_unique<FBO>(screenWidth, screenHeight);
  newFrame->setupTexture();

  oldFrame->bind();
  auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "Old framebuffer is not complete! Error code: " << fboStatus << std::endl;
  }
  newFrame->bind();
  fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "New framebuffer is not complete! Error code: " << fboStatus << std::endl;
  }
  newFrame->unbind();
  frameMesh = createFrameMesh();

  // generate random spheres
  std::random_device rd;
  // std::mt19937 gen(42); // fixed seed
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> posDis{-5.0f, 5.0f};
  std::uniform_real_distribution<float> radiusDis{0.3f, 0.8f};
  std::uniform_real_distribution<float> propertyDis{0.0f, 1.0f};
  for (int i = 0; i < MAX_SPHERES; i++) {
    spheres[i].center = glm::vec3(posDis(gen), posDis(gen), posDis(gen));
    spheres[i].radius = radiusDis(gen);
    spheres[i].color = glm::vec4(propertyDis(gen), propertyDis(gen), propertyDis(gen), 1.0f);
    spheres[i].emissionColor = spheres[i].color;
    spheres[i].shininess = 0.0f;
    spheres[i].smoothness = propertyDis(gen);
    spheres[i].specularProbability = propertyDis(gen);
  }
  // the first sphere is the light source
  spheres[0].center = glm::vec3(-30.0f, 10.0f, 40.0f);
  spheres[0].radius = 40.0f;
  spheres[0].color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  spheres[0].emissionColor = glm::vec4(1.0f);
  spheres[0].shininess = 1.0f;
  spheres[0].smoothness = 0.0f;

  rtMesh->setupUBO(spheres, MAX_SPHERES, GL_DYNAMIC_DRAW);

  shaderProgram->use();
  glUniform1i(glGetUniformLocation(shaderProgram->ID, "numSpheres"), numSpheres);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "fov"), camera->fov);
  glUniform2f(glGetUniformLocation(shaderProgram->ID, "resolution"), screenWidth, screenHeight);
  glUniform3fv(glGetUniformLocation(shaderProgram->ID, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f)));
  glUniformBlockBinding(shaderProgram->ID, glGetUniformBlockIndex(shaderProgram->ID, "sphereData"), 0);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, rtMesh->ubo.ID);

  frameShader->use();
  oldFrame->bindTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(frameShader->ID, "oldFrame"), 0);  // texture unit 0
  newFrame->bindTexture(GL_TEXTURE1);
  glUniform1i(glGetUniformLocation(frameShader->ID, "newFrame"), 1);  // texture unit 1
}

TestRtSphere::~TestRtSphere() {}

void TestRtSphere::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestRtSphere::OnRender() {
  // 1. bind anything to the framebuffer, including the backgroud
  newFrame->bind();

  // 2. render the main scene
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  // some time based animation
  unsigned int ticks = frameIdx + 1;
  float timeValue = SDL_GetTicks() / 1000.0f;
  if (isRotate) {
    spheres[0].center[0] = 50.0f * sin(timeValue);
    spheres[0].center[2] = 50.0f * cos(timeValue);
  }
  rtMesh->updateUBO(spheres, MAX_SPHERES);

  shaderProgram->use();
  camera->update(shaderProgram.get());

  glUniform1ui(glGetUniformLocation(shaderProgram->ID, "ticks"), ticks);
  glUniform1i(glGetUniformLocation(shaderProgram->ID, "numSpheres"), numSpheres);
  glUniform1i(glGetUniformLocation(shaderProgram->ID, "numBounces"), numBounces);
  glUniform1i(glGetUniformLocation(shaderProgram->ID, "numRays"), numRays);

  rtMesh->draw(shaderProgram.get());

  // 3. after rendering the main scene, unbind the framebuffer
  newFrame->unbind();

  // 4. if not real time, store the new frame texture to the old frame texture
  if (!isRealTime) {
    oldFrame->bind();
    frameShader->use();

    // render on the new frame
    frameMesh->draw(frameShader.get());
    oldFrame->unbind();
  }

  // 5. render the frame buffer to the screen
  glUniform1i(glGetUniformLocation(frameShader->ID, "numFrames"), frameIdx);
  frameMesh->draw(frameShader.get());

  frameIdx++;
}

void TestRtSphere::OnImGuiRender() {
  ImGui::Text("Ray Casting:");
  ImGui::SliderInt("Bounces", &numBounces, 1, 10);
  ImGui::SliderInt("Num", &numRays, 1, 20);
  ImGui::Text("Main Light:");
  ImGui::SliderFloat("Height", &spheres[0].center[1], 0.0f, 100.0f);
  ImGui::SliderFloat("Radius", &spheres[0].radius, 40.0f, 100.0f);
  if (ImGui::Checkbox("Show", &isFirstSphereLight))
    glUniform1i(glGetUniformLocation(shaderProgram->ID, "isFirstSphereLight"), isFirstSphereLight);
  ImGui::Checkbox("Rotate", &isRotate);

  ImGui::Text("Spheres:");
  ImGui::SliderInt("Num", &numSpheres, 0, MAX_SPHERES);
  if (ImGui::Checkbox("Enable Specular Bounce", &enableSpecularBounce))
    glUniform1i(glGetUniformLocation(shaderProgram->ID, "isSpecularBounce"), enableSpecularBounce);
  if (ImGui::Checkbox("White Specular Light", &isSpecularWhite))
    glUniform1i(glGetUniformLocation(shaderProgram->ID, "isSpecularWhite"), isSpecularWhite);
  if (ImGui::Checkbox("Random Shine Spheres", &isRandomShine)) {
    std::random_device rd;
    std::mt19937 gen(rd());
    // 20% of the spheres have shininess
    std::uniform_real_distribution<float> shinessDis{0.0f, 10.0f};
    for (int i = 1; i < MAX_SPHERES; i++) {
      float shiness = isRandomShine ? shinessDis(gen) : 0.0f;
      spheres[i].shininess = 8.0f < shiness ? 1.2 : 0.0f;
    }
  }
  ImGui::Text("Render:");
  if (ImGui::Checkbox("Real Time Toggle", &isRealTime)) {
    frameIdx = 0;  // reset frame index
  }
}

void TestRtSphere::OnExit() {
  newFrame->del();
  oldFrame->del();
  rtMesh->del();
  frameMesh->del();
  shaderProgram->del();
  frameShader->del();
}

}  // namespace test
