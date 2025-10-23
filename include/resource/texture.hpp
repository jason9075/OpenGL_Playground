#pragma once

#include <GL/glew.h>

#include <string>

namespace gfx::resource {

class Texture {
 public:
  std::string type;
  Texture();
  Texture(const char *image, const char *texType, GLuint slot);
  ~Texture() { reset(); }
  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;
  Texture(Texture &&other) noexcept;
  Texture &operator=(Texture &&other) noexcept;

  void bind() const;
  void unbind() const;

 private:
  void reset();
  GLuint ID;
  GLuint unit;
};
}  // namespace gfx::resource
