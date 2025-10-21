#include "tests/TestRoom.hpp"

#include <OPPCH.h>

#include "BasicMesh.hpp"

namespace test {

TestRoom::TestRoom(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderRoom = std::make_unique<Shader>("./shaders/room_vert.glsl", "./shaders/room_frag.glsl");

  auto texMarble1 = std::make_shared<Texture>("./assets/textures/marble1.png", "albedo", 0);
  auto texMarble2 = std::make_shared<Texture>("./assets/textures/marble2.png", "albedo", 0);

  auto V1 = std::vector<std::shared_ptr<Texture>>{texMarble1};
  auto V2 = std::vector<std::shared_ptr<Texture>>{texMarble2};

  floor = createCuboidMesh(30.0f, 0.3f, 20.0f, {0, 0, 0});
  floor->setTexture(V2);

  northWall = createCuboidMesh(30.0f, 16.0f, 0.1f, {0, 8, 10});
  northWall->setTexture(V1);

  southWall = createCuboidMesh(30.0f, 16.0f, 0.1f, {0, 8, -10});
  southWall->setTexture(V1);

  eastWall = createCuboidMesh(0.1f, 16.0f, 20.0f, {15, 8, 0});
  eastWall->setTexture(V1);

  westWall = createCuboidMesh(0.1f, 16.0f, 20.0f, {-15, 8, 0});
  westWall->setTexture(V1);

  ceiling = createCuboidMesh(30.0f, 0.1f, 20.0f, {0, 16, 0});
  ceiling->setTexture(V2);

  groundMesh = createPlaneMesh(100.0f, glm::vec3(0.0f, 1.0f, 0.0f));

  glm::vec3 position = glm::vec3(7.0f, 6.0f, 3.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  shaderRoom->use();
  glUniformMatrix4fv(glGetUniformLocation(shaderRoom->PROGRAM_ID, "modelMatrix"), 1, GL_FALSE,
                     glm::value_ptr(glm::mat4(1.0f)));
  // background color
  glUniform3f(glGetUniformLocation(shaderRoom->PROGRAM_ID, "backgroundColor"), backgroundColor[0], backgroundColor[1],
              backgroundColor[2]);

  // glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "modelMatrix"), 1, GL_FALSE,
  //                    glm::value_ptr(glm::mat4(1.0f)));
  // glUniform3fv(glGetUniformLocation(shaderProgram->ID, "lightColor"), 1,
  // glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))); glUniform3fv(glGetUniformLocation(shaderProgram->ID,
  // "lightPosition"), 1,
  //              glm::value_ptr(glm::vec3(0.0f, 9.0f, 0.0f)));
  // glUniform1i(glGetUniformLocation(shaderProgram->ID, "useTexture"), false);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}

TestRoom::~TestRoom() {}

void TestRoom::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestRoom::OnRender() {
  glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  shaderRoom->use();
  camera->update(shaderRoom.get());
  glUniform1i(glGetUniformLocation(shaderRoom->PROGRAM_ID, "classId"), 0);
  groundMesh->draw(shaderRoom.get());

  glUniform1i(glGetUniformLocation(shaderRoom->PROGRAM_ID, "classId"), 1);
  northWall->draw(shaderRoom.get());
  southWall->draw(shaderRoom.get());
  eastWall->draw(shaderRoom.get());
  westWall->draw(shaderRoom.get());
  glUniform1i(glGetUniformLocation(shaderRoom->PROGRAM_ID, "classId"), 2);
  floor->draw(shaderRoom.get());
  glUniform1i(glGetUniformLocation(shaderRoom->PROGRAM_ID, "classId"), 3);
  ceiling->draw(shaderRoom.get());
}

void TestRoom::OnImGuiRender() {}

void TestRoom::OnExit() {}
}  // namespace test
