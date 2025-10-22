#pragma once
#include <cstdint>
#include <vector>

namespace gfx {
namespace geom {
class Mesh;
}
}  // namespace gfx

class Shader;  // 你的 ShaderClass（由 ShaderClass.hpp 提供）
namespace gfx::render {

class MeshRenderer {
 public:
  // 綁定多個 UBO 的 binding points（可選，不用就給空陣列）
  void draw(const geom::Mesh& mesh, Shader& shader, uint32_t instanceCount = 1,
            const std::vector<uint32_t>& uboBindingPoints = {}, unsigned primitive = 0 /* 0=>GL_TRIANGLES */) const;

  // 僅畫頂點範圍（非索引）
  void drawRange(const geom::Mesh& mesh, Shader& shader, uint32_t numVertices, uint32_t startIdx = 0,
                 uint32_t instanceCount = 1, const std::vector<uint32_t>& uboBindingPoints = {},
                 unsigned primitive = 0 /* 0=>GL_TRIANGLES */) const;

 private:
  void bindTextures_(const geom::Mesh& mesh, Shader& shader) const;
  void bindUbos_(const geom::Mesh& mesh, const std::vector<uint32_t>& uboBindingPoints) const;
};

}  // namespace gfx::render
