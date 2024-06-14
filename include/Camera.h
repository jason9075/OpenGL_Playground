#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <unordered_map>

#include "ShaderClass.h"

// Let the user define the event listener
class EventListener {
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
  MOVE_FASTER,
};

class Camera {
 public:
  Camera(int width, int height, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f));
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
  void update(Shader *shaderProgram);

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
  float normalSpeed = 0.1f;
  float fastSpeed = normalSpeed * 5.0f;
  float movementSpeed = normalSpeed;
  float mouseSensitivity = 1.0f;
};
