#pragma once

#include <GL/glew.h>

class Shader {
 public:
  Shader(const char *vertexShaderSource, const char *fragmentShaderSource);
  GLuint ID;

  void use();
  void del();

 private:
};
