#include "Camera.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

Camera::Camera(int width, int height, glm::vec3 position, glm::vec3 orientation) {
  this->width = width;
  this->height = height;
  this->position = position;
  this->orientation = orientation;
  this->up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Camera::moveForward() { position += movementSpeed * orientation; }

void Camera::moveBackward() { position -= movementSpeed * orientation; }

void Camera::moveLeft() { position -= movementSpeed * glm::normalize(glm::cross(orientation, up)); }

void Camera::moveRight() { position += movementSpeed * glm::normalize(glm::cross(orientation, up)); }

void Camera::moveUp() { position += movementSpeed * up; }

void Camera::moveDown() { position -= movementSpeed * up; }

void Camera::moveFaster() { movementSpeed = 0.5; }

void Camera::moveSlower() { movementSpeed = 0.1; }

void Camera::lookAroundStart(float offsetX, float offsetY) {
  // hide the cursor
  SDL_ShowCursor(SDL_DISABLE);

  // move mouse to the center of the window
  if (firstClick) {
    SDL_WarpMouseInWindow(NULL, width / 2, height / 2);
    firstClick = false;
  }

  // rotX and rotY is the rotation angle in x and y axis
  float rotX = offsetY * mouseSensitivity;
  float rotY = offsetX * mouseSensitivity;

  // look up and down
  glm::vec3 right = glm::normalize(glm::cross(orientation, up));
  glm::vec3 newOri = glm::rotate(orientation, glm::radians(-rotX), right);

  float angle = glm::degrees(glm::angle(newOri, up));
  // restrict angle between 5 and 175 degrees
  if (angle < 175 && angle > 5) {
    orientation = newOri;
  }

  // look left and right
  orientation = glm::rotate(orientation, glm::radians(-rotY), up);

  // move mouse back to the center of the window again
  SDL_WarpMouseInWindow(NULL, width / 2, height / 2);
}

void Camera::lookAroundEnd() {
  SDL_ShowCursor(SDL_ENABLE);
  firstClick = true;
}

void Camera::setEventListener(EventListener *listener) { this->listener = listener; }

void Camera::moveCamera() {
  if (keyState[MOVE_FORWARD]) {
    moveForward();
  }
  if (keyState[MOVE_BACKWARD]) {
    moveBackward();
  }
  if (keyState[MOVE_LEFT]) {
    moveLeft();
  }
  if (keyState[MOVE_RIGHT]) {
    moveRight();
  }
  if (keyState[MOVE_UP]) {
    moveUp();
  }
  if (keyState[MOVE_DOWN]) {
    moveDown();
  }
  if (keyState[MOVE_FASTER])
    movementSpeed = fastSpeed;
  else
    movementSpeed = normalSpeed;
}

void Camera::handle(SDL_Event &event) {
  // Handle Keyboard input

  if (event.type == SDL_KEYDOWN && listener != nullptr) {
    // only trigger the event once
    if (event.key.repeat == 0) {
      listener->onKeyDown(event.key.keysym.sym);
    }
  } else if (event.type == SDL_KEYUP && listener != nullptr) {
    listener->onKeyUp(event.key.keysym.sym);
  }

  // Handle Mouse input , move the camera orientation when left mouse button is pressed
  if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT && event.type == SDL_MOUSEMOTION)) {
    listener->onMouseLeftPress(event);
  } else if (event.button.button == SDL_BUTTON_LEFT && event.type == SDL_MOUSEBUTTONUP) {
    listener->onMouseLeftRelease();
  }
}

void Camera::update(Shader *shaderProgram) {
  glm::mat4 view = glm::lookAt(position, position + orientation, up);
  glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / height, nearPlane, farPlane);

  glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "camMatrix"), 1, GL_FALSE,
                     glm::value_ptr(projection * view));
}
