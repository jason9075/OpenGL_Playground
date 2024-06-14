#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices,
           const std::vector<Texture> &textures)
    : vertices(vertices), indices(indices), textures(textures), vao(), vbo(vertices), ebo(indices) {
  vao.bind();
  vbo.bind();
  ebo.bind();

  vao.linkAttr(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, position));
  vao.linkAttr(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, normal));
  vao.linkAttr(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, color));
  vao.linkAttr(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));

  vao.unbind();
  vbo.unbind();
  ebo.unbind();
}

void Mesh::draw(Shader *shader) {
  shader->use();
  vao.bind();

  // Textures
  for (unsigned int i = 0; i < textures.size(); i++) {
    textures[i].texUnit(*shader, "texture0", i);
    textures[i].bind();
  }

  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  vao.unbind();
}

void Mesh::del() {
  vao.del();
  vbo.del();
  ebo.del();
}
