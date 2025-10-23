#pragma once

#include <GL/glew.h>
namespace gfx::core {
class UBO {
 public:
  GLuint ID;
  UBO();
  ~UBO() { reset(); }
  UBO(const UBO &) = delete;
  UBO &operator=(const UBO &) = delete;
  UBO(UBO &&o) noexcept;
  UBO &operator=(UBO &&o) noexcept;

  void bind() const;
  void unbind() const;
  template <typename T>
  void bufferData(T *data, size_t count, GLenum usage) {
    glBufferData(GL_UNIFORM_BUFFER, sizeof(T) * count, data, usage);
  }
  template <typename T>
  void bufferSubData(const T *data, size_t count) {
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T) * count, data);
  }

 private:
  void reset();
};
}  // namespace gfx::core
