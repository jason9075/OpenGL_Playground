#pragma once

#include <GL/glew.h>

class VBO;

class VAO {
 public:
  VAO();
  void linkVBO(VBO &VBO, GLuint layout);
  void bind();
  void unbind();
  void del();

 private:
  GLuint ID;
};

class VBO {
 public:
  VBO(GLfloat *vertices, GLsizeiptr size);
  void bind();
  void unbind();
  void del();

 private:
  GLuint ID;
};

class EBO {
 public:
  EBO(GLuint *indices, GLsizeiptr size);
  void bind();
  void unbind();
  void del();

 private:
  GLuint ID;
};
