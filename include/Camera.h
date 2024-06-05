#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <unordered_map>

#include "ShaderClass.h"

class Camera;

class EventListener {
 public:
  virtual void onKeyDown(SDL_Keycode key) = 0;
  virtual void onKeyUp(SDL_Keycode key) = 0;
  virtual void onMouseLeftPress(SDL_Event &event) = 0;
  virtual void onMouseLeftRelease() = 0;
};

class CameraEventListener : public EventListener {
 public:
  CameraEventListener(Camera &camera);
  void onKeyDown(SDL_Keycode key);
  void onKeyUp(SDL_Keycode key);
  void onMouseLeftPress(SDL_Event &event);
  void onMouseLeftRelease();

 private:
  Camera &camera;
};

enum CameraMovement {
  MOVE_FORWARD,
  MOVE_BACKWARD,
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_UP,
  MOVE_DOWN,
  MOVE_FASTER,
  MOVE_SLOWER
};

class Camera {
 public:
  Camera(int width, int height, float x, float y, float z);
  void moveForward();
  void moveBackward();
  void moveLeft();
  void moveRight();
  void moveUp();
  void moveDown();
  void moveFaster();
  void moveSlower();
  void lookAroundStart(float x, float y);
  void lookAroundEnd();
  void moveCamera();
  void setEventListener(EventListener *listener);
  void handle(SDL_Event &event);
  void update(Shader &shaderProgram);

  std::unordered_map<int, bool> keyState;

 private:
  int width;
  int height;
  glm::vec3 position;     // the position of the camera
  glm::vec3 orientation;  // the vector where the camera is looking at
  glm::vec3 up;

  float fov = 45.0f;
  float nearPlane = 0.1f;
  float farPlane = 100.0f;

  EventListener *listener;

  bool firstClick = true;
  float movementSpeed = 0.1f;
  float mouseSensitivity = 1.0f;
};
