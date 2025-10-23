#pragma once
#include <cstdint>
#include <vector>

#include "render/mesh_renderer.hpp"

namespace gfx {
namespace geom {
class CubeMap;
}
}  // namespace gfx

class Shader;  // 你的 ShaderClass（由 ShaderClass.hpp 提供）
namespace gfx::render {

class CubeMapRenderer {
 public:
  explicit CubeMapRenderer(const MeshRenderer& meshRenderer) : meshRenderer_(meshRenderer) {}

  void draw(const geom::CubeMap& cm, Shader& shader) const;

 private:
  const MeshRenderer& meshRenderer_;
};

}  // namespace gfx::render
