#include "tests/TestGs.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

namespace test {

TestGs::TestGs(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/pointcloud_vert.glsl", "./shaders/pointcloud_frag.glsl");

  happly::PLYData plyIn("./assets/Medic.ply");

  // Print Info
  // printInfo(plyIn);

  auto x = plyIn.getElement("vertex").getProperty<float>("x");
  auto y = plyIn.getElement("vertex").getProperty<float>("y");
  auto z = plyIn.getElement("vertex").getProperty<float>("z");
  // https://github.com/graphdeco-inria/gaussian-splatting/issues/485
  auto red = plyIn.getElement("vertex").getProperty<float>("f_dc_0");
  auto grn = plyIn.getElement("vertex").getProperty<float>("f_dc_1");
  auto blu = plyIn.getElement("vertex").getProperty<float>("f_dc_2");

  auto scaleX = plyIn.getElement("vertex").getProperty<float>("scale_0");
  auto scaleY = plyIn.getElement("vertex").getProperty<float>("scale_1");
  auto scaleZ = plyIn.getElement("vertex").getProperty<float>("scale_2");

  auto rotate0 = plyIn.getElement("vertex").getProperty<float>("rot_0");
  auto rotate1 = plyIn.getElement("vertex").getProperty<float>("rot_1");
  auto rotate2 = plyIn.getElement("vertex").getProperty<float>("rot_2");
  auto rotate3 = plyIn.getElement("vertex").getProperty<float>("rot_3");

  auto opacity = plyIn.getElement("vertex").getProperty<float>("opacity");

  std::vector<Point> points;
  for (int i = 0; i < x.size(); i++) {
    Point point;
    point.position = glm::vec3(x[i], y[i], z[i]);
    point.color = glm::vec3(0.5f + C0 * red[i], 0.5f + C0 * grn[i], 0.5f + C0 * blu[i]);  // normalize color
    float qlen = std::sqrt(scaleX[i] * scaleX[i] + scaleY[i] * scaleY[i] + scaleZ[i] * scaleZ[i]);
    point.rotation = glm::vec4((rotate0[i] / qlen), (rotate1[i] / qlen), (rotate2[i] / qlen), (rotate3[i] / qlen));
    point.scale = glm::vec3(std::exp(scaleX[i]), std::exp(scaleY[i]), std::exp(scaleZ[i]));

    points.push_back(point);
  }

  // print points[0]
  std::cout << "position: " << points[0].position.x << " " << points[0].position.y << " " << points[0].position.z
            << std::endl;
  std::cout << "color: " << points[0].color.x << " " << points[0].color.y << " " << points[0].color.z << std::endl;
  std::cout << "scale: " << points[0].scale.x << " " << points[0].scale.y << " " << points[0].scale.z << std::endl;
  std::cout << "rotation: " << points[0].rotation.x << " " << points[0].rotation.y << " " << points[0].rotation.z << " "
            << points[0].rotation.w << std::endl;

  pc = std::make_unique<PointCloud>(points);

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
                     glm::value_ptr(glm::rotate(glm::rotate(glm::mat4(1.0f), rotateX, glm::vec3(1.0f, 0.0f, 0.0f)),
                                                rotateZ, glm::vec3(0.0f, 0.0f, 1.0f))));
  camera->update(shaderProgram.get());
  pc->draw(shaderProgram.get());
}

void TestGs::OnImGuiRender() {
  ImGui::SliderFloat("Rot-X", &rotateX, -3.15f, 3.15f);
  ImGui::SliderFloat("Rot-Z", &rotateZ, -3.15f, 3.15f);
}

void TestGs::printInfo(happly::PLYData& plyIn) {
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
}

}  // namespace test
