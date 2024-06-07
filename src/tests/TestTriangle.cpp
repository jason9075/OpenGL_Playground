#include "tests/TestTriangle.h"

#include "DataObject.h"
#include "GL/glew.h"
#include "imgui.h"

namespace test {
class TestTriangle::CameraEventListener : public EventListener {
 public:
  CameraEventListener(Camera *camera);
  void onKeyDown(SDL_Keycode key) override;
  void onKeyUp(SDL_Keycode key) override;
  void onMouseLeftPress(SDL_Event &event) override;
  void onMouseLeftRelease() override;

 private:
  Camera *camera;
};

TestTriangle::TestTriangle(float screenWidth, float screenHeight) {
  std::vector<Vertex> vertices = {{{-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f}},
                                  {{0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f}},
                                  {{0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f}}};
  std::vector<GLuint> indices = {0, 1, 2};

  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/default_vert.glsl", "./shaders/default_frag.glsl");
  mesh = std::make_unique<Mesh>(vertices, indices);

  camera = std::make_unique<Camera>(screenWidth, screenHeight, 0.0f, 0.0f, 3.0f);
  listener = std::make_unique<CameraEventListener>(camera.get());
  camera->setEventListener(listener.get());
}

TestTriangle::~TestTriangle() {}

void TestTriangle::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestTriangle::OnRender() {
  camera->moveCamera();
  shaderProgram->use();
  camera->update(shaderProgram.get());
  mesh->draw(shaderProgram.get());
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

void TestTriangle::OnImGuiRender() {
  // setting triangle color
  ImGui::ColorEdit3("Background", backgroundColor);
  ImGui::ColorEdit3("Triangle", triangleColor);
}

TestTriangle::CameraEventListener::CameraEventListener(Camera *camera) : camera(camera) {}
void TestTriangle::CameraEventListener::onKeyDown(SDL_Keycode key) {
  switch (key) {
    case SDLK_w:
      camera->keyState[MOVE_FORWARD] = true;
      break;
    case SDLK_a:
      camera->keyState[MOVE_LEFT] = true;
      break;
    case SDLK_s:
      camera->keyState[MOVE_BACKWARD] = true;
      break;
    case SDLK_d:
      camera->keyState[MOVE_RIGHT] = true;
      break;
    case SDLK_SPACE:
      camera->keyState[MOVE_UP] = true;
      break;
    case SDLK_c:
      camera->keyState[MOVE_DOWN] = true;
      break;
    case SDLK_LSHIFT:  // Increase movement speed
      camera->keyState[MOVE_FASTER] = true;
      break;
  }
}
void TestTriangle::CameraEventListener::onKeyUp(SDL_Keycode key) {
  switch (key) {
    case SDLK_w:
      camera->keyState[MOVE_FORWARD] = false;
      break;
    case SDLK_a:
      camera->keyState[MOVE_LEFT] = false;
      break;
    case SDLK_s:
      camera->keyState[MOVE_BACKWARD] = false;
      break;
    case SDLK_d:
      camera->keyState[MOVE_RIGHT] = false;
      break;
    case SDLK_SPACE:
      camera->keyState[MOVE_UP] = false;
      break;
    case SDLK_c:
      camera->keyState[MOVE_DOWN] = false;
      break;
    case SDLK_LSHIFT:  // Decrease movement speed
      camera->keyState[MOVE_SLOWER] = true;
      break;
  }
}

void TestTriangle::CameraEventListener::onMouseLeftPress(SDL_Event &e) {
  camera->lookAroundStart(e.motion.xrel, e.motion.yrel);
}

void TestTriangle::CameraEventListener::onMouseLeftRelease() { camera->lookAroundEnd(); }
}  // namespace test
