#pragma once

#include "ShaderClass.hpp"

// Let the user define the event listener
class CameraEventListener {
 public:
  virtual void onKeyDown(SDL_Keycode key) = 0;
  virtual void onKeyUp(SDL_Keycode key) = 0;
  virtual void onMouseLeftPress(SDL_Event &event) = 0;
  virtual void onMouseLeftRelease() = 0;
};

enum CameraMovement {
  MOVE_FORWARD,
  MOVE_BACKWARD,
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_UP,
  MOVE_DOWN,
  ROTATE_LEFT,
  ROTATE_RIGHT,
  ROTATE_UP,
  ROTATE_DOWN,
  MOVE_FASTER,
};

class Camera {
 public:
  Camera(int width, int height, glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
         glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f));
  glm::vec3 position;     // the position of the camera
  glm::vec3 orientation;  // the vector where the camera is looking at
  glm::vec3 up;
  glm::mat4 viewMatrix;
  glm::mat4 projMatrix;
  float fov = 45.0f;
  void moveForward();
  void moveBackward();
  void moveLeft();
  void moveRight();
  void rotateLeft();
  void rotateRight();
  void rotateUp();
  void rotateDown();
  void moveUp();
  void moveDown();
  void moveFaster();
  void moveSlower();
  void lookAroundStart(float x, float y);
  void lookAroundEnd();
  void moveCamera();
  void setEventListener(CameraEventListener *listener);
  void handle(SDL_Event &event);
  void update(Shader *shaderProgram);

  std::unordered_map<int, bool> keyState;
  bool relativeMouseMode = false;  // FPS mode

 private:
  int width;
  int height;

  float nearPlane = 0.1f;
  float farPlane = 300.0f;

  CameraEventListener *listener;

  bool firstClick = true;
  float normalSpeed = 0.1f;
  float fastSpeed = normalSpeed * 5.0f;
  float movementSpeed = normalSpeed;
  float mouseSensitivity = 0.5f;
};

class GhostCameraListener : public CameraEventListener {
 public:
  GhostCameraListener(Camera *camera);
  void onKeyDown(SDL_Keycode key);
  void onKeyUp(SDL_Keycode key);
  void onMouseLeftPress(SDL_Event &event);
  void onMouseLeftRelease();

 private:
  Camera *camera;
};
