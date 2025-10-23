#include "core/vao.hpp"

#include <glm/glm.hpp>

#include "core/vbo.hpp"

namespace gfx::core {
VAO::VAO() { glGenVertexArrays(1, &ID); }

VAO::VAO(VAO &&other) noexcept : ID(other.ID) { other.ID = 0; }

VAO &VAO::operator=(VAO &&other) noexcept {
  if (this != &other) {
    reset();
    ID = other.ID;
    other.ID = 0;
  }
  return *this;
}
void VAO::linkAttr(VBO &vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, const void *offset) {
  vbo.bind();
  glEnableVertexAttribArray(layout);
  glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
  vbo.unbind();
}

void VAO::linkAttrDiv(VBO &vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, const void *offset) {
  vbo.bind();
  glEnableVertexAttribArray(layout);
  glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
  glVertexAttribDivisor(layout, 1);
  vbo.unbind();
}

void VAO::linkMat4(VBO &vbo, GLuint layout) {
  vbo.bind();
  std::size_t vec4Size = sizeof(glm::vec4);

  for (GLuint i = 0; i < 4; i++) {
    glEnableVertexAttribArray(layout + i);
    glVertexAttribPointer(layout + i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(i * vec4Size));
    glVertexAttribDivisor(layout + i, 1);
  }

  vbo.unbind();
}

void VAO::bind() const { glBindVertexArray(ID); }

void VAO::unbind() const { glBindVertexArray(0); }

void VAO::reset() {
  if (ID) {
    glDeleteVertexArrays(1, &ID);
    ID = 0;
  }
}
}  // namespace gfx::core
