#include "core/ubo.hpp"

namespace gfx::core {
UBO::UBO() { glGenBuffers(1, &ID); }

UBO::UBO(UBO &&o) noexcept : ID(o.ID) { o.ID = 0; }
UBO &UBO::operator=(UBO &&o) noexcept {
  if (this != &o) {
    reset();
    ID = o.ID;
    o.ID = 0;
  }
  return *this;
}
void UBO::bind() const { glBindBuffer(GL_UNIFORM_BUFFER, ID); }

void UBO::unbind() const { glBindBuffer(GL_UNIFORM_BUFFER, 0); }

void UBO::reset() {
  if (ID) {
    glDeleteBuffers(1, &ID);
    ID = 0;
  }
}
}  // namespace gfx::core
