#pragma once
#include <cstdint>
#include <vector>

#include "geom/gaussianSplat.hpp"

namespace gfx {
namespace geom {}
}  // namespace gfx

class Shader;  // 你的 ShaderClass（由 ShaderClass.hpp 提供）
namespace gfx::render {

class GsRenderer {
 public:
  explicit GsRenderer() {}

  void draw(const gfx::geom::GaussianSplat& gs, ::Shader& shader) const;

 private:
};

}  // namespace gfx::render
