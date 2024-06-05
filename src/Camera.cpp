#include "Camera.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(int width, int height, float x, float y, float z) {
    this->width = width;
    this->height = height;
    this->position = glm::vec3(x, y, z);
    this->orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    this->up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Camera::input(SDL_Event &event) {
    // Handle Keyboard input
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_w:
                position += movementSpeed * orientation;
                break;
            case SDLK_a:
                position -= movementSpeed * glm::normalize(glm::cross(orientation, up));
                break;
            case SDLK_s:
                position -= movementSpeed * orientation;
                break;
            case SDLK_d:
                position += movementSpeed * glm::normalize(glm::cross(orientation, up));
                break;
            case SDLK_SPACE:
                position += movementSpeed * up;
                break;
            case SDLK_c:
                position -= movementSpeed * up;
                break;
            case SDLK_LSHIFT:  // Increase movement speed
                movementSpeed = 0.5f;
                break;
        }
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_LSHIFT:  // Reset movement speed
                movementSpeed = 0.1f;
                break;
        }
    }

    // Handle Mouse input , move the camera orientation when left mouse button is pressed
    if (event.type == SDL_MOUSEMOTION && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        // hide the cursor
        SDL_ShowCursor(SDL_DISABLE);

        // move mouse to the center of the window
        if (firstClick) {
            SDL_WarpMouseInWindow(NULL, width / 2, height / 2);
            firstClick = false;
        }   

        // rotX and rotY is the rotation angle in x and y axis
        float rotX = event.motion.yrel * mouseSensitivity;
        float rotY = event.motion.xrel * mouseSensitivity;

        // look up and down
        glm::vec3 newOri = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation,up)));
        if (abs(glm::angle(newOri, up) - glm::radians(90.0f)) <= 85.0f) {
            orientation = newOri;
        }

        // look left and right
        orientation = glm::rotate(orientation, glm::radians(-rotY), up); 

        //move mouse back to the center of the window again
        SDL_WarpMouseInWindow(NULL, width / 2, height / 2);
    } else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT){
        SDL_ShowCursor(SDL_ENABLE);
        firstClick = true;
    }
}

void Camera::update(Shader &shaderProgram) {
    glm::mat4 view = glm::lookAt(position, position + orientation, up);
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / height, nearPlane, farPlane);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(projection * view));
}
















