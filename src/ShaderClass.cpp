#include "ShaderClass.h"

#include <GL/glew.h>

#include <iostream>

#include "Utils.h"

Shader::Shader(const char *vertShaderPath, const char *fragShaderPath) {
  std::string vertexCode = readFile(vertShaderPath);
  std::string fragmentCode = readFile(fragShaderPath);

  const char *vertexShaderSource = vertexCode.c_str();
  const char *fragmentShaderSource = fragmentCode.c_str();

  // Build and compile our shader program
  // Vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // Check for compile time errors
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  // Fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // Check for compile time errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  // Link shaders
  ID = glCreateProgram();
  glAttachShader(ID, vertexShader);
  glAttachShader(ID, fragmentShader);
  glLinkProgram(ID);
  // Check for linking errors
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  // Delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Shader::use() { glUseProgram(ID); }

void Shader::del() { glDeleteProgram(ID); }
