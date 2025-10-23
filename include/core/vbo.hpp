#pragma once

#include <GL/glew.h>

#include <vector>

namespace gfx::core {
class VBO {
 public:
  VBO();

  template <typename T>
  VBO(const std::vector<T> &data) {
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), data.data(), GL_STATIC_DRAW);
  }
  ~VBO() { reset(); }

  VBO(const VBO &) = delete;
  VBO &operator=(const VBO &) = delete;
  VBO(VBO &&o) noexcept;
  VBO &operator=(VBO &&o) noexcept;

  template <typename T>
  void bufferData(const std::vector<T> &data) {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), data.data(), GL_STATIC_DRAW);
  }
  void bind() const;
  void unbind() const;

 private:
  void reset();
  GLuint ID;
};
}  // namespace gfx::core
