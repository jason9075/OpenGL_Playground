#include "tests/TestGs.h"

#include <OPPCH.h>

namespace test {

TestGs::TestGs(const float screenWidth, const float screenHeight) {
  glViewport(0, 0, screenWidth, screenHeight);

  // shaderProgram = std::make_unique<Shader>("./shaders/pointcloud_vert.glsl", "./shaders/pointcloud_frag.glsl");
  shaderProgram = std::make_unique<Shader>("./shaders/gaussian_vert.glsl", "./shaders/gaussian_frag.glsl");

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

  std::vector<GaussianSphere> spheres;
  for (int i = 0; i < x.size(); i++) {
    GaussianSphere sphere;
    sphere.position = glm::vec3(x[i], y[i], z[i]);
    sphere.color = glm::vec3(0.5f + C0 * red[i], 0.5f + C0 * grn[i], 0.5f + C0 * blu[i]);  // normalize color
    glm::mat3 R(glm::normalize(glm::quat(rotate0[i], rotate1[i], rotate2[i], rotate3[i])));
    glm::mat3 S =
        glm::mat3(std::exp(scaleX[i]), 0.0f, 0.0f, 0.0f, std::exp(scaleY[i]), 0.0f, 0.0f, 0.0f, std::exp(scaleZ[i]));
    glm ::mat3 M = R * S * glm::transpose(S) * glm::transpose(R);
    sphere.covA = glm::vec3(M[0][0], M[0][1], M[0][2]);
    sphere.covB = glm::vec3(M[1][1], M[1][2], M[2][2]);
    sphere.opacity = opacity[i];
    spheres.push_back(sphere);
  }
  splat = std::make_unique<GaussianSplat>(spheres);

  // std::vector<Point> points;
  // for (int i = 0; i < x.size(); i++) {
  //   Point point;
  //   point.position = glm::vec3(x[i], y[i], z[i]);
  //   point.color = glm::vec3(0.5f + C0 * red[i], 0.5f + C0 * grn[i], 0.5f + C0 * blu[i]);  // normalize color
  //   float qlen = std::sqrt(scaleX[i] * scaleX[i] + scaleY[i] * scaleY[i] + scaleZ[i] * scaleZ[i]);
  //   point.rotation = glm::vec4((rotate0[i] / qlen), (rotate1[i] / qlen), (rotate2[i] / qlen), (rotate3[i] / qlen));
  //   point.scale = glm::vec3(std::exp(scaleX[i]), std::exp(scaleY[i]), std::exp(scaleZ[i]));
  //
  //   points.push_back(point);
  // }

  // print points[0]
  // std::cout << "position: " << points[0].position.x << " " << points[0].position.y << " " << points[0].position.z
  //           << std::endl;
  // std::cout << "color: " << points[0].color.x << " " << points[0].color.y << " " << points[0].color.z << std::endl;
  // std::cout << "scale: " << points[0].scale.x << " " << points[0].scale.y << " " << points[0].scale.z << std::endl;
  // std::cout << "rotation: " << points[0].rotation.x << " " << points[0].rotation.y << " " << points[0].rotation.z <<
  // " "
  //           << points[0].rotation.w << std::endl;
  //
  // pc = std::make_unique<PointCloud>(points);

  glm::vec3 position = glm::vec3(0.0f, 1.0f, 5.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<GhostCameraListener>(camera.get());
  camera->setEventListener(listener.get());

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
}
TestGs::~TestGs() {}

void TestGs::OnEvent(SDL_Event& event) { camera->handle(event); }

void TestGs::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  camera->moveCamera();

  shaderProgram->use();
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "scaleFactor"), scaleFactor);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "W"), 1024.0f);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "H"), 768.0f);
  float tan_fovx = tan(glm::radians(45.0f) / 2.0f);
  float tan_fovy = tan(glm::radians(45.0f) / 2.0f);
  float focal_y = 768.0f / (2.0f * tan_fovy);
  float focal_x = 1024.0f / (2.0f * tan_fovx);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "focal_x"), focal_x);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "focal_y"), focal_y);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "tan_fovx"), tan_fovx);
  glUniform1f(glGetUniformLocation(shaderProgram->ID, "tan_fovy"), tan_fovy);
  camera->update(shaderProgram.get());
  // pc->draw(shaderProgram.get());
  splat->draw(shaderProgram.get());
}

void TestGs::OnImGuiRender() {
  ImGui::SliderFloat("Rot-X", &rotateX, -3.15f, 3.15f);
  ImGui::SliderFloat("Rot-Z", &rotateZ, -3.15f, 3.15f);
  ImGui::SliderFloat("ScaleF", &scaleFactor, 0.1f, 10.0f);
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
