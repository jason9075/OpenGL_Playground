#pragma once

#include <GL/glew.h>
namespace gfx::core {
class FBO {
 public:
  GLuint ID;
  GLuint textureID;
  FBO(float width, float height);
  ~FBO() { reset(); }
  FBO(const FBO &) = delete;
  FBO &operator=(const FBO &) = delete;
  FBO(FBO &&o) noexcept;
  FBO &operator=(FBO &&o) noexcept;
  void bind() const;
  void unbind() const;
  void setupTexture();
  void bindTexture(GLenum textureUnit);
  float width;
  float height;

 private:
  void reset();
};
}  // namespace gfx::core
