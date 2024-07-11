#include "tests/TestGs.h"

#include "glm/gtc/type_ptr.hpp"
#include "happly.h"

namespace test {

TestGs::TestGs(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/pointcloud_vert.glsl", "./shaders/pointcloud_frag.glsl");

  happly::PLYData plyIn("./models/Tree.ply");

  // Print Info
  auto comments = plyIn.comments;
  std::cout << "comments: " << std::endl;
  for (const auto& comment : comments) {
    std::cout << comment << std::endl;
  }
  auto infoComments = plyIn.objInfoComments;
  std::cout << "info comments: " << std::endl;
  for (const auto& infoComment : infoComments) {
    std::cout << infoComment << std::endl;
  }
  auto elementNames = plyIn.getElementNames();
  std::cout << "element names: " << std::endl;
  for (const auto& elementName : elementNames) {
    std::cout << "- " << elementName << std::endl;
    auto properties = plyIn.getElement(elementName).getPropertyNames();
    std::cout << "  properties: " << std::endl;
    for (const auto& property : properties) {
      std::cout << "  - " << property << std::endl;
    }
  }

  auto x = plyIn.getElement("vertex").getProperty<float>("x");
  auto y = plyIn.getElement("vertex").getProperty<float>("y");
  auto z = plyIn.getElement("vertex").getProperty<float>("z");
  auto red = plyIn.getElement("vertex").getProperty<unsigned char>("red");
  auto green = plyIn.getElement("vertex").getProperty<unsigned char>("green");
  auto blue = plyIn.getElement("vertex").getProperty<unsigned char>("blue");

  std::vector<Vertex> vertices;
  for (int i = 0; i < x.size(); i++) {
    Vertex vertex;
    vertex.position = glm::vec3(x[i], y[i], z[i]);
    vertex.color = glm::vec3(red[i] / 255.0f, green[i] / 255.0f, blue[i] / 255.0f);  // normalize color
    vertices.push_back(vertex);
  }

  pc = std::make_unique<PointCloud>(vertices);

  glEnable(GL_PROGRAM_POINT_SIZE);

  glm::vec3 position = glm::vec3(0.0f, 1.0f, 5.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());
}
TestGs::~TestGs() {}

void TestGs::OnEvent(SDL_Event& event) { camera->handle(event); }

void TestGs::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  shaderProgram->use();

  glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "modelMatrix"), 1, GL_FALSE,
                     glm::value_ptr(glm::mat4(1.0f)));
  camera->update(shaderProgram.get());
  pc->draw(shaderProgram.get());
}
void TestGs::OnImGuiRender() {}

}  // namespace test
