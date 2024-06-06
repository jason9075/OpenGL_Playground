#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>

class VBO;

class VAO {
 public:
  VAO();
  void linkAttr(VBO &VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, const void *offset);
  void bind();
  void unbind();
  void del();

 private:
  GLuint ID;
};

struct Vertex {
  glm::vec3 position;
};

class VBO {
 public:
  VBO(const std::vector<Vertex> &vertices);
  void bind();
  void unbind();
  void del();

 private:
  GLuint ID;
};

class EBO {
 public:
  EBO(const std::vector<GLuint> &indices);
  void bind();
  void unbind();
  void del();

 private:
  GLuint ID;
};
