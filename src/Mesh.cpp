#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices)
    : vertices(vertices), indices(indices), vao(), vbo(vertices), ebo(indices) {
  vao.bind();
  vbo.bind();
  ebo.bind();

  vao.linkAttr(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, position));

  vao.unbind();
  vbo.unbind();
  ebo.unbind();
}

void Mesh::draw(Shader *shader) {
  shader->use();
  vao.bind();
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  vao.unbind();
}

void Mesh::del() {
  vao.del();
  vbo.del();
  ebo.del();
}
