#pragma once

#include <GL/glew.h>
namespace gfx::core {
class VBO;

class VAO {
 public:
  VAO();
  ~VAO() { reset(); };

  VAO(const VAO &) = delete;
  VAO &operator=(const VAO &) = delete;

  VAO(VAO &&other) noexcept;
  VAO &operator=(VAO &&other) noexcept;

  void linkAttr(VBO &vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, const void *offset);
  void linkAttrDiv(VBO &vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, const void *offset);
  void linkMat4(VBO &vbo, GLuint layout);
  void bind() const;
  void unbind() const;
  GLuint ID;

 private:
  void reset();
};
}  // namespace gfx::core
