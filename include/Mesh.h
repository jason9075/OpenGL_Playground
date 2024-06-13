#pragma once

#include "DataObject.h"
#include "ShaderClass.h"

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
