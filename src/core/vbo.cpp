#include "core/vbo.hpp"

namespace gfx::core {
VBO::VBO() { glGenBuffers(1, &ID); }

VBO::VBO(VBO &&o) noexcept : ID(o.ID) { o.ID = 0; }

VBO &VBO ::operator=(VBO &&o) noexcept {
  if (this != &o) {
    reset();
    ID = o.ID;
    o.ID = 0;
  }
  return *this;
}
void VBO::bind() const { glBindBuffer(GL_ARRAY_BUFFER, ID); }

void VBO::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void VBO::reset() {
  if (ID) {
    glDeleteBuffers(1, &ID);
    ID = 0;
  }
}
}  // namespace gfx::core
