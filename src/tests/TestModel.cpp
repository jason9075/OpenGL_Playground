#include "tests/TestModel.h"

#include "GL/glew.h"

namespace test {
class TestModel::CameraEventListener : public EventListener {
 public:
  CameraEventListener(Camera *camera);
  void onKeyDown(SDL_Keycode key) override;
  void onKeyUp(SDL_Keycode key) override;
  void onMouseLeftPress(SDL_Event &event) override;
  void onMouseLeftRelease() override;

 private:
  Camera *camera;
};

TestModel::TestModel(const float screenWidth, const float screenHeight, const char *path) {
  glViewport(0, 0, screenWidth, screenHeight);

  shaderProgram = std::make_unique<Shader>("./shaders/model_vert.glsl", "./shaders/model_frag.glsl");
  model = std::make_unique<Model>(path);

  glm::vec3 position = glm::vec3(0.0f, 5.0f, 20.0f);
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  camera = std::make_unique<Camera>(screenWidth, screenHeight, position, orientation);
  listener = std::make_unique<CameraEventListener>(camera.get());
  camera->setEventListener(listener.get());
}

TestModel::~TestModel() {}

void TestModel::OnEvent(SDL_Event &event) { camera->handle(event); }

void TestModel::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->moveCamera();
  shaderProgram->use();

  /* Check out this video for more information on depth testing:
   * www.youtube.com/watch?v=3xGKu4T4SCU
   */
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  camera->update(shaderProgram.get());
  model->draw(shaderProgram.get(), camera.get());
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

void TestModel::OnImGuiRender() {}

TestModel::CameraEventListener::CameraEventListener(Camera *camera) : camera(camera) {}
void TestModel::CameraEventListener::onKeyDown(SDL_Keycode key) {
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
void TestModel::CameraEventListener::onKeyUp(SDL_Keycode key) {
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
      camera->keyState[MOVE_FASTER] = false;
      break;
  }
}

void TestModel::CameraEventListener::onMouseLeftPress(SDL_Event &e) {
  camera->lookAroundStart(e.motion.xrel, e.motion.yrel);
}

void TestModel::CameraEventListener::onMouseLeftRelease() { camera->lookAroundEnd(); }
}  // namespace test
