#pragma once

#include <GL/glew.h>

#include <vector>

namespace gfx::core {
class EBO {
 public:
  EBO();
  ~EBO() { reset(); }
  EBO(const EBO &) = delete;
  EBO &operator=(const EBO &) = delete;
  EBO(EBO &&o) noexcept;
  EBO &operator=(EBO &&o) noexcept;

  void bind() const;
  void bufferData(const std::vector<GLuint> &indices);

 private:
  void reset();
  GLuint ID;
};
}  // namespace gfx::core
