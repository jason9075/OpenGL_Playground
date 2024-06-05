#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "ShaderClass.h"

class Camera {
public:
    Camera(int width, int height, float x, float y, float z);
    void input(SDL_Event &event);
    void update(Shader &shaderProgram);

private:
    int width;
    int height;
    glm::vec3 position;     // the position of the camera
    glm::vec3 orientation;  // the vector where the camera is looking at
    glm::vec3 up;

    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    
    bool firstClick = true;
    float movementSpeed = 0.1f;
    float mouseSensitivity = 1.0f;
    
};
