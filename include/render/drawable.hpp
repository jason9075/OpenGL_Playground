#pragma once
#include <cstdint>

namespace gfx::render {

struct DrawArgs {
  uint32_t instanceCount{1};
  uint32_t startIndex{0};  // for glDrawArrays*
  uint32_t count{0};       // 0 => auto 推算（使用整個 mesh）
  uint32_t baseVertex{0};  // optional
  unsigned primitive{0};   // 0 => GL_TRIANGLES
};

class Drawable {
 public:
  virtual ~Drawable() = default;
  virtual void draw(class Shader& shader, const DrawArgs& args = {}) const = 0;
};

}  // namespace gfx::render
