#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <vector>

#include "ShaderClass.h"

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
  glm::vec3 normal;
  glm::vec3 color;
  glm::vec2 texCoords;
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

class Texture {
 public:
  const char *type;
  Texture(const char *image, const char *texType, GLuint slot);
  void texUnit(Shader &shader, const char *uniform, GLuint unit);
  void bind();
  void unbind();
  void del();

 private:
  GLuint ID;
  GLuint unit;
};

class Mesh {
 public:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;

  VAO vao;
  VBO vbo;
  EBO ebo;

  Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures);

  void draw(Shader *shader);

  void del();
};

class PointCloud {
 public:
  std::vector<Vertex> vertices;

  VAO vao;
  VBO vbo;

  PointCloud(const std::vector<Vertex> &vertices);

  void draw(Shader *shader);

  void del();
};
