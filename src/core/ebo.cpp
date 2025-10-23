#include "core/ebo.hpp"

namespace gfx::core {
EBO::EBO() { glGenBuffers(1, &ID); }

EBO::EBO(EBO &&o) noexcept : ID(o.ID) { o.ID = 0; }
EBO &EBO::operator=(EBO &&o) noexcept {
  if (this != &o) {
    reset();
    ID = o.ID;
    o.ID = 0;
  }
  return *this;
}

void EBO::bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }

void EBO::bufferData(const std::vector<GLuint> &indices) {
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

void EBO::reset() {
  if (ID) {
    glDeleteBuffers(1, &ID);
    ID = 0;
  }
}
}  // namespace gfx::core
