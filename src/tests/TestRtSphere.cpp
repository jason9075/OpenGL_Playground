#include "tests/TestRtSphere.hpp"

#include <OPPCH.h>

#include <memory>

#include "BasicMesh.hpp"

namespace test {

TestRtSphere::TestRtSphere(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/rt_sphere_vert.glsl", "./shaders/rt_sphere_frag.glsl");
  frameShader = std::make_unique<Shader>("./shaders/acc_framebuffer_vert.glsl", "./shaders/acc_framebuffer_frag.glsl");

  glm::vec3 position = glm::vec3(0.0f, 10.0f, 25.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
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
  frameMesh = createPlaneMesh();

  // the first sphere is the light source
  spheres[0].center = glm::vec3(sphereLightOffset[0], CBS + sphereLightOffset[1], sphereLightOffset[2]);
  spheres[0].radius = 1.0f;
  spheres[0].material.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  spheres[0].material.emissionColor = glm::vec4(1.0f);
  spheres[0].material.shininess = sphereLightShininess;
  spheres[0].material.smoothness = 0.0f;
  // the rest of the spheres are random
  randomizeSpheres(spheres, numSpheres);

  rtMesh->setupUBO(spheres, MAX_SPHERES, GL_DYNAMIC_DRAW, 0);

  // cornell box
  makeCornellBox(triangles);

  rtMesh->setupUBO(triangles, MAX_TRIANGLES, GL_DYNAMIC_DRAW, 1);

  shaderProgram->use();
  glUniform1f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "fov"), camera->fov);
  glUniform2f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "resolution"), screenWidth, screenHeight);
  glUniformBlockBinding(shaderProgram->PROGRAM_ID, glGetUniformBlockIndex(shaderProgram->PROGRAM_ID, "sphereData"), 0);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, rtMesh->ubo[0].ID);
  glUniformBlockBinding(shaderProgram->PROGRAM_ID, glGetUniformBlockIndex(shaderProgram->PROGRAM_ID, "triangleData"),
                        1);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, rtMesh->ubo[1].ID);

  frameShader->use();
  oldFrame->bindTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(frameShader->PROGRAM_ID, "oldFrame"), 0);  // texture unit 0
  newFrame->bindTexture(GL_TEXTURE1);
  glUniform1i(glGetUniformLocation(frameShader->PROGRAM_ID, "newFrame"), 1);  // texture unit 1
}

TestRtSphere::~TestRtSphere() {}

void TestRtSphere::OnEvent(SDL_Event& event) { camera->handle(event); }

void TestRtSphere::OnRender() {
  // 1. bind anything to the framebuffer, including the backgroud
  newFrame->bind();

  // 2. render the main scene
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  // some time based animation
  if (isRotate) {
    float timeValue = SDL_GetTicks() / 1000.0f;
    spheres[0].center[0] = 9.0f * sin(timeValue);
    spheres[0].center[1] = CBS + 9.0f * cos(timeValue);
    spheres[0].center[2] = 9.0f * cos(timeValue);
  }
  rtMesh->updateUBO(spheres, MAX_SPHERES, 0);
  rtMesh->updateUBO(triangles, MAX_TRIANGLES, 1);

  shaderProgram->use();
  camera->update(shaderProgram.get());

  glUniform1ui(glGetUniformLocation(shaderProgram->PROGRAM_ID, "frameIdx"), frameIdx);
  glUniform1i(glGetUniformLocation(shaderProgram->PROGRAM_ID, "numSpheres"), numSpheres);
  glUniform1i(glGetUniformLocation(shaderProgram->PROGRAM_ID, "numBounces"), numBounces);
  glUniform1i(glGetUniformLocation(shaderProgram->PROGRAM_ID, "numRays"), numRays);
  glUniform1i(glGetUniformLocation(shaderProgram->PROGRAM_ID, "isSpecularBounce"), enableSpecularBounce);
  glUniform1i(glGetUniformLocation(shaderProgram->PROGRAM_ID, "isSpecularWhite"), isSpecularWhite);
  glUniform1f(glGetUniformLocation(shaderProgram->PROGRAM_ID, "ambientLight"), ambientLight);
  glUniform1i(glGetUniformLocation(shaderProgram->PROGRAM_ID, "showSphereLight"), showSphereLight);
  glUniform1i(glGetUniformLocation(shaderProgram->PROGRAM_ID, "showCornellLight"), isShowCornellLight);
  glUniform1i(glGetUniformLocation(shaderProgram->PROGRAM_ID, "showCornellPlanes"), isShowCornellPlanes);

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
  glUniform1i(glGetUniformLocation(frameShader->PROGRAM_ID, "numFrames"), frameIdx);
  frameMesh->draw(frameShader.get());

  frameIdx++;
}

void TestRtSphere::OnImGuiRender() {
  ImGui::BulletText("Ray Casting:");
  ImGui::SliderInt("Bounces", &numBounces, 1, 10);
  ImGui::SliderInt("Ray Num", &numRays, 1, 20);
  ImGui::Checkbox("Enable Specular Bounce", &enableSpecularBounce);
  ImGui::Checkbox("White Light Reflect", &isSpecularWhite);

  ImGui::BulletText("Light:");
  ImGui::SliderFloat("Ambient", &ambientLight, 0.0f, 1.0f);
  ImGui::Checkbox("Cornell Light", &isShowCornellLight);
  if (ImGui::SliderFloat("CL Str", &mainLightShininess, 0.0f, 15.0f)) {
    for (int i = 12; i < 24; i++) triangles[i].material.shininess = mainLightShininess;
  }
  ImGui::Text(" - Sphere Light:");
  ImGui::Checkbox("Show Sphere Light", &showSphereLight);
  ImGui::SliderFloat("SL Str", &spheres[0].material.shininess, 0.0f, 15.0f);
  ImGui::SliderFloat("Radius", &spheres[0].radius, 0.5f, 4.0f);
  ImGui::Checkbox("Rotate", &isRotate);
  if (ImGui::SliderFloat3("Offset", sphereLightOffset, -10.0f, 10.0f)) {
    spheres[0].center = glm::vec3(sphereLightOffset[0], CBS + sphereLightOffset[1], sphereLightOffset[2]);
  }

  ImGui::BulletText("Cornell Box:");
  ImGui::Checkbox("Show Cornell Planes", &isShowCornellPlanes);
  if (ImGui::ColorEdit3("Back Col", glm::value_ptr(backWallColor))) {
    for (int i = 10; i < 12; i++) triangles[i].material.color = glm::vec4(backWallColor, 1.0f);
  }
  ImGui::Text(" - Reflectivity:");
  if (ImGui::SliderFloat("Red", &redWallReflectivity, 0.0f, MAX_REFLECTIVITY))
    setReflectivity(triangles, 0, 2, redWallReflectivity);
  if (ImGui::SliderFloat("Green", &greenWallReflectivity, 0.0f, MAX_REFLECTIVITY))
    setReflectivity(triangles, 2, 4, greenWallReflectivity);
  if (ImGui::SliderFloat("Floor", &floorReflectivity, 0.0f, MAX_REFLECTIVITY))
    setReflectivity(triangles, 4, 6, floorReflectivity);
  if (ImGui::SliderFloat("Ceiling", &ceilingReflectivity, 0.0f, MAX_REFLECTIVITY))
    setReflectivity(triangles, 6, 8, ceilingReflectivity);
  if (ImGui::SliderFloat("Front", &frontWallReflectivity, 0.0f, MAX_REFLECTIVITY))
    setReflectivity(triangles, 8, 10, frontWallReflectivity);
  if (ImGui::SliderFloat("Back", &backWallReflectivity, 0.0f, MAX_REFLECTIVITY))
    setReflectivity(triangles, 10, 12, backWallReflectivity);

  ImGui::BulletText("Spheres:");
  ImGui::SliderInt("Num", &numSpheres, 0, MAX_SPHERES);
  if (ImGui::Checkbox("Random Shine Spheres", &isRandomShine)) {
    std::random_device rd;
    std::mt19937 gen(rd());
    // 20% of the spheres have shininess
    std::uniform_real_distribution<float> shinessDis{0.0f, 1.0f};
    for (int i = 1; i < MAX_SPHERES; i++) {
      float shiness = isRandomShine ? shinessDis(gen) : 0.0f;
      spheres[i].material.shininess = 0.8f < shiness ? 1.0f : 0.0f;
    }
  }
  if (ImGui::Button("Reset Spheres")) {
    randomizeSpheres(spheres, numSpheres);
  }

  ImGui::BulletText("Render:");
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

void TestRtSphere::randomizeSpheres(Sphere* spheres, const int numSpheres) {
  // generate random spheres
  std::random_device rd;
  // std::mt19937 gen(42);  // fixed seed
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> posDis{-8.0f, 8.0f};
  std::uniform_real_distribution<float> radiusDis{0.5f, 2.0f};
  std::uniform_real_distribution<float> propertyDis{0.0f, 1.0f};
  for (int i = 1; i < MAX_SPHERES; i++) {
    spheres[i].center = glm::vec3(posDis(gen), CBS + posDis(gen), posDis(gen));
    spheres[i].radius = radiusDis(gen);
    spheres[i].material.color = glm::vec4(propertyDis(gen), propertyDis(gen), propertyDis(gen), 1.0f);
    spheres[i].material.emissionColor = spheres[i].material.color;
    spheres[i].material.shininess = 0.0f;
    spheres[i].material.smoothness = propertyDis(gen);
    spheres[i].material.specularProbability = propertyDis(gen);
  }
}

void TestRtSphere::setReflectivity(Triangle* triangles, const int start, const int end, const float reflectivity) {
  for (int i = start; i < end; i++) {
    triangles[i].material.smoothness = reflectivity;
    triangles[i].material.specularProbability = reflectivity;
  }
}

void TestRtSphere::makeCornellBox(Triangle* triangles) {
  // left red wall
  triangles[0].posA = glm::vec3(-CBS, 0.0f, -CBS);
  triangles[0].posB = glm::vec3(-CBS, 0.0f, CBS);
  triangles[0].posC = glm::vec3(-CBS, CBS2, CBS);
  triangles[0].normal = glm::vec3(1.0f, 0.0f, 0.0f);
  triangles[0].material.color = glm::vec4(1.0f, 0.1f, 0.1f, 1.0f);

  triangles[1].posA = glm::vec3(-CBS, 0.0f, -CBS);
  triangles[1].posB = glm::vec3(-CBS, CBS2, CBS);
  triangles[1].posC = glm::vec3(-CBS, CBS2, -CBS);
  triangles[1].normal = glm::vec3(1.0f, 0.0f, 0.0f);
  triangles[1].material.color = glm::vec4(1.0f, 0.1f, 0.1f, 1.0f);

  // right green wall
  triangles[2].posA = glm::vec3(CBS, 0.0f, -CBS);
  triangles[2].posB = glm::vec3(CBS, CBS2, -CBS);
  triangles[2].posC = glm::vec3(CBS, CBS2, CBS);
  triangles[2].normal = glm::vec3(-1.0f, 0.0f, 0.0f);
  triangles[2].material.color = glm::vec4(0.1f, 1.0f, 0.1f, 1.0f);

  triangles[3].posA = glm::vec3(CBS, 0.0f, -CBS);
  triangles[3].posB = glm::vec3(CBS, CBS2, CBS);
  triangles[3].posC = glm::vec3(CBS, 0.0f, CBS);
  triangles[3].normal = glm::vec3(-1.0f, 0.0f, 0.0f);
  triangles[3].material.color = glm::vec4(0.1f, 1.0f, 0.1f, 1.0f);

  // bottom white floor
  triangles[4].posA = glm::vec3(-CBS, 0.0f, -CBS);
  triangles[4].posB = glm::vec3(CBS, 0.0f, -CBS);
  triangles[4].posC = glm::vec3(CBS, 0.0f, CBS);
  triangles[4].normal = glm::vec3(0.0f, 1.0f, 0.0f);
  triangles[4].material.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

  triangles[5].posA = glm::vec3(-CBS, 0.0f, -CBS);
  triangles[5].posB = glm::vec3(CBS, 0.0f, CBS);
  triangles[5].posC = glm::vec3(-CBS, 0.0f, CBS);
  triangles[5].normal = glm::vec3(0.0f, 1.0f, 0.0f);
  triangles[5].material.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

  // top white ceiling
  triangles[6].posA = glm::vec3(-CBS, CBS2, -CBS);
  triangles[6].posB = glm::vec3(CBS, CBS2, CBS);
  triangles[6].posC = glm::vec3(CBS, CBS2, -CBS);
  triangles[6].normal = glm::vec3(0.0f, -1.0f, 0.0f);
  triangles[6].material.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

  triangles[7].posA = glm::vec3(-CBS, CBS2, -CBS);
  triangles[7].posB = glm::vec3(-CBS, CBS2, CBS);
  triangles[7].posC = glm::vec3(CBS, CBS2, CBS);
  triangles[7].normal = glm::vec3(0.0f, -1.0f, 0.0f);
  triangles[7].material.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

  // front white wall
  triangles[8].posA = glm::vec3(-CBS, 0.0f, CBS);
  triangles[8].posB = glm::vec3(CBS, 0.0f, CBS);
  triangles[8].posC = glm::vec3(CBS, CBS2, CBS);
  triangles[8].normal = glm::vec3(0.0f, 0.0f, -1.0f);
  triangles[8].material.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

  triangles[9].posA = glm::vec3(-CBS, 0.0f, CBS);
  triangles[9].posB = glm::vec3(CBS, CBS2, CBS);
  triangles[9].posC = glm::vec3(-CBS, CBS2, CBS);
  triangles[9].normal = glm::vec3(0.0f, 0.0f, -1.0f);
  triangles[9].material.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

  // back wall
  triangles[10].posA = glm::vec3(-CBS, 0.0f, -CBS);
  triangles[10].posB = glm::vec3(-CBS, CBS2, -CBS);
  triangles[10].posC = glm::vec3(CBS, CBS2, -CBS);
  triangles[10].normal = glm::vec3(0.0f, 0.0f, 1.0f);
  triangles[10].material.color = glm::vec4(backWallColor, 1.0f);

  triangles[11].posA = glm::vec3(-CBS, 0.0f, -CBS);
  triangles[11].posB = glm::vec3(CBS, CBS2, -CBS);
  triangles[11].posC = glm::vec3(CBS, 0.0f, -CBS);
  triangles[11].normal = glm::vec3(0.0f, 0.0f, 1.0f);
  triangles[11].material.color = glm::vec4(backWallColor, 1.0f);

  triangles[12].posA = lightCenter + glm::vec3(-lightSize.x, 0.0f, -lightSize.z);
  triangles[12].posB = lightCenter + glm::vec3(-lightSize.x, 0.0f, lightSize.z);
  triangles[12].posC = lightCenter + glm::vec3(lightSize.x, 0.0f, lightSize.z);
  triangles[12].normal = glm::vec3(0.0f, -1.0f, 0.0f);

  triangles[13].posA = lightCenter + glm::vec3(-lightSize.x, 0.0f, -lightSize.z);
  triangles[13].posB = lightCenter + glm::vec3(lightSize.x, 0.0f, lightSize.z);
  triangles[13].posC = lightCenter + glm::vec3(lightSize.x, 0.0f, -lightSize.z);
  triangles[13].normal = glm::vec3(0.0f, -1.0f, 0.0f);

  triangles[14].posA = lightCenter + glm::vec3(-lightSize.x, lightSize.y, -lightSize.z);
  triangles[14].posB = lightCenter + glm::vec3(lightSize.x, lightSize.y, -lightSize.z);
  triangles[14].posC = lightCenter + glm::vec3(lightSize.x, lightSize.y, lightSize.z);
  triangles[14].normal = glm::vec3(0.0f, 1.0f, 0.0f);

  triangles[15].posA = lightCenter + glm::vec3(-lightSize.x, lightSize.y, -lightSize.z);
  triangles[15].posB = lightCenter + glm::vec3(lightSize.x, lightSize.y, lightSize.z);
  triangles[15].posC = lightCenter + glm::vec3(-lightSize.x, lightSize.y, lightSize.z);
  triangles[15].normal = glm::vec3(0.0f, 1.0f, 0.0f);

  triangles[16].posA = lightCenter + glm::vec3(-lightSize.x, 0.0f, -lightSize.z);
  triangles[16].posB = lightCenter + glm::vec3(-lightSize.x, lightSize.y, -lightSize.z);
  triangles[16].posC = lightCenter + glm::vec3(lightSize.x, lightSize.y, -lightSize.z);
  triangles[16].normal = glm::vec3(0.0f, 0.0f, -1.0f);

  triangles[17].posA = lightCenter + glm::vec3(-lightSize.x, 0.0f, -lightSize.z);
  triangles[17].posB = lightCenter + glm::vec3(lightSize.x, lightSize.y, -lightSize.z);
  triangles[17].posC = lightCenter + glm::vec3(lightSize.x, 0.0f, -lightSize.z);
  triangles[17].normal = glm::vec3(0.0f, 0.0f, -1.0f);

  triangles[18].posA = lightCenter + glm::vec3(lightSize.x, 0.0f, -lightSize.z);
  triangles[18].posB = lightCenter + glm::vec3(lightSize.x, lightSize.y, -lightSize.z);
  triangles[18].posC = lightCenter + glm::vec3(lightSize.x, lightSize.y, lightSize.z);
  triangles[18].normal = glm::vec3(1.0f, 0.0f, 0.0f);

  triangles[19].posA = lightCenter + glm::vec3(lightSize.x, 0.0f, -lightSize.z);
  triangles[19].posB = lightCenter + glm::vec3(lightSize.x, lightSize.y, lightSize.z);
  triangles[19].posC = lightCenter + glm::vec3(lightSize.x, 0.0f, lightSize.z);
  triangles[19].normal = glm::vec3(1.0f, 0.0f, 0.0f);

  triangles[20].posA = lightCenter + glm::vec3(-lightSize.x, 0.0f, -lightSize.z);
  triangles[20].posB = lightCenter + glm::vec3(-lightSize.x, lightSize.y, -lightSize.z);
  triangles[20].posC = lightCenter + glm::vec3(-lightSize.x, lightSize.y, lightSize.z);
  triangles[20].normal = glm::vec3(-1.0f, 0.0f, 0.0f);

  triangles[21].posA = lightCenter + glm::vec3(-lightSize.x, 0.0f, -lightSize.z);
  triangles[21].posB = lightCenter + glm::vec3(-lightSize.x, lightSize.y, lightSize.z);
  triangles[21].posC = lightCenter + glm::vec3(-lightSize.x, 0.0f, lightSize.z);
  triangles[21].normal = glm::vec3(-1.0f, 0.0f, 0.0f);

  triangles[22].posA = lightCenter + glm::vec3(-lightSize.x, 0.0f, lightSize.z);
  triangles[22].posB = lightCenter + glm::vec3(-lightSize.x, lightSize.y, lightSize.z);
  triangles[22].posC = lightCenter + glm::vec3(lightSize.x, lightSize.y, lightSize.z);
  triangles[22].normal = glm::vec3(0.0f, 0.0f, 1.0f);

  triangles[23].posA = lightCenter + glm::vec3(-lightSize.x, 0.0f, lightSize.z);
  triangles[23].posB = lightCenter + glm::vec3(lightSize.x, lightSize.y, lightSize.z);
  triangles[23].posC = lightCenter + glm::vec3(lightSize.x, 0.0f, lightSize.z);
  triangles[23].normal = glm::vec3(0.0f, 0.0f, 1.0f);

  for (int i = 12; i < 24; i++) {
    triangles[i].material.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    triangles[i].material.shininess = mainLightShininess;
    triangles[i].material.smoothness = 0.0f;
    triangles[i].material.specularProbability = 0.0f;
  }

  // set emission color is same as color
  for (int i = 0; i < 24; i++) {
    triangles[i].material.emissionColor = triangles[i].material.color;
  }
}

}  // namespace test
