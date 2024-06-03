#pragma once

#include <GL/glew.h>

class Shader {
public:
  Shader(const char *vertexShaderSource, const char *fragmentShaderSource);
  void use();
  void del();

private:
  GLuint programID;
};
