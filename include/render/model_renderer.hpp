#pragma once
#include <cstdint>
#include <vector>

#include "Model.hpp"
#include "render/mesh_renderer.hpp"

namespace gfx {
namespace geom {}
}  // namespace gfx

class Shader;  // 你的 ShaderClass（由 ShaderClass.hpp 提供）
namespace gfx::render {

class MeshRenderer;

class ModelRenderer {
 public:
  explicit ModelRenderer(const MeshRenderer& meshRenderer) : meshRenderer_(meshRenderer) {}

  // uboBindingPoints: 例如 {Camera=0, Lights=1, PerObject=2}
  void draw(const Model& model, ::Shader& shader, uint32_t instanceCount = 1,
            const std::vector<uint32_t>& uboBindingPoints = {}) const;

  // 只畫每個 mesh 的前 N 個頂點（對應你原本的 drawTri）
  void drawTri(const Model& model, ::Shader& shader, uint32_t numVertices, uint32_t instanceCount = 1,
               const std::vector<uint32_t>& uboBindingPoints = {}) const;

  // 是否用 UBO 傳 per-object（modelMatrix），預設關閉：走 glUniformMatrix4fv
  void usePerObjectUBO(bool enabled) { usePerObjectUBO_ = enabled; }
  bool usingPerObjectUBO() const { return usePerObjectUBO_; }

  // 若使用 UBO，指定 per-object UBO 的 binding point（例如 2）
  void setPerObjectBinding(uint32_t binding) { perObjectBinding_ = binding; }
  uint32_t perObjectBinding() const { return perObjectBinding_; }

 private:
  const MeshRenderer& meshRenderer_;
  bool usePerObjectUBO_{false};
  uint32_t perObjectBinding_{2};  // 預設把 PerObject UBO 綁在 2
};

}  // namespace gfx::render
