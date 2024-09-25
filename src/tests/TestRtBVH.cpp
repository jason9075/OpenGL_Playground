#include "tests/TestRtBVH.hpp"

#include <OPPCH.h>

#include "BasicMesh.hpp"

namespace test {

TestRtBVH::TestRtBVH(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/rt_bvh_vert.glsl", "./shaders/rt_bvh_frag.glsl");
  model = std::make_unique<Model>("./assets/gltf_duck/Duck.gltf");
  // iterate over all triangles vertices
  for (auto &mesh : model->meshes) {
    // indices
    for (int i = 0; i < mesh.indices.size(); i += 3) {
      glm::vec3 v0 = mesh.vertices[mesh.indices[i]].position;
      glm::vec3 v1 = mesh.vertices[mesh.indices[i + 1]].position;
      glm::vec3 v2 = mesh.vertices[mesh.indices[i + 2]].position;
      std::cout << "v0: " << v0.x << " " << v0.y << " " << v0.z << std::endl;
      std::cout << "v1: " << v1.x << " " << v1.y << " " << v1.z << std::endl;
      std::cout << "v2: " << v2.x << " " << v2.y << " " << v2.z << std::endl;
    }
  }

  beam = createCuboidMesh(0.05f, 10.0f, 0.05f, glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  // move the beam position to z = -6.0f
  beamModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -6.0f));

  glm::vec3 position = glm::vec3(5.0f, 5.0f, -9.0f);
  glm::vec3 orientation = glm::vec3(-0.54f, -0.41f, 0.66f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  startTS = std::chrono::high_resolution_clock::now();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}

TestRtBVH::~TestRtBVH() {}

void TestRtBVH::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestRtBVH::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  shaderProgram->use();
  camera->update(shaderProgram.get());
  model->setModelMatrix(glm::mat4(1.0f));

  glUniform1i(glGetUniformLocation(shaderProgram->ID, "enableLighting"), 1);
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "modelMatrix"), 1, GL_FALSE,
                     glm::value_ptr(glm::mat4(1.0)));
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  auto elapsed = std::chrono::high_resolution_clock::now() - startTS;
  auto numTriangles = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
  model->meshes[0].drawTri(shaderProgram.get(), numTriangles, 0);

  glUniform1i(glGetUniformLocation(shaderProgram->ID, "enableLighting"), 0);
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "modelMatrix"), 1, GL_FALSE,
                     glm::value_ptr(beamModelMatrix * glm::mat4_cast(beamRotation)));
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  beam->draw(shaderProgram.get());
}

void TestRtBVH::OnImGuiRender() {
  ImGui::Text("Tris: %d", model->meshes[0].numTriangles());
  if (ImGui::Button("Redraw")) {
    startTS = std::chrono::high_resolution_clock::now();
  }
  ImGui::Text("Beam Rotation");
  // Angle axis rotation by degrees
  ImGui::Text("X: %.1f, Y: %.1f, Z: %.1f", glm::degrees(glm::eulerAngles(beamRotation)).x,
              glm::degrees(glm::eulerAngles(beamRotation)).y, glm::degrees(glm::eulerAngles(beamRotation)).z);
  if (ImGui::Button("X+")) {
    glm::quat q = glm::angleAxis(glm::radians(5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    beamRotation = q * beamRotation;
  }
  if (ImGui::Button("X-")) {
    glm::quat q = glm::angleAxis(glm::radians(-5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    beamRotation = q * beamRotation;
  }
  if (ImGui::Button("Y+")) {
    glm::quat q = glm::angleAxis(glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    beamRotation = q * beamRotation;
  }
  if (ImGui::Button("Y-")) {
    glm::quat q = glm::angleAxis(glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    beamRotation = q * beamRotation;
  }
  if (ImGui::Button("Z+")) {
    glm::quat q = glm::angleAxis(glm::radians(5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    beamRotation = q * beamRotation;
  }
  if (ImGui::Button("Z-")) {
    glm::quat q = glm::angleAxis(glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    beamRotation = q * beamRotation;
  }
}

void TestRtBVH::OnExit() {
  model->del();
  shaderProgram->del();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

}  // namespace test
