#include "render/mesh_renderer.hpp"

#include <GL/glew.h>

#include "ShaderClass.hpp"  // 只在這裡依賴 Shader
#include "geom/mesh.hpp"    // 取得 VAO/VBO/EBO/Textures/UBO

namespace gfx::render {

static inline unsigned toGLPrimitive(unsigned p) { return p ? p : GL_TRIANGLES; }

void MeshRenderer::bindUbos_(const geom::Mesh& mesh, const std::vector<uint32_t>& bindingPoints) const {
  // Mesh::ubo 是 public vector<core::UBO>（或提供 getUbos()）
  const auto& ubos = mesh.ubo;  // 若之後改 private，請加 const accessor
  const size_t n = std::min(ubos.size(), bindingPoints.size());
  for (size_t i = 0; i < n; ++i) {
    // UBO 類別中 ID 是 public；若改 private 就加 getter
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoints[i], ubos[i].ID);
  }
}

void MeshRenderer::bindTextures_(const geom::Mesh& mesh, Shader& shader) const {
  // 慣例：根據 Texture->type 設 uniform，例如 "diffuse", "specular", "normal"
  // 你的 Texture 有 texUnit(shader, uniformName, unit) 可直接用
  if (!mesh.hasTexture()) return;

  unsigned unit = 0;
  // 需要 Mesh 提供 textures accessor；目前你有 public 成員 vector
  for (const auto& texPtr : mesh.textures) {
    if (!texPtr) continue;
    const std::string uni = texPtr->type;  // e.g., "diffuse"
    glUniform1i(glGetUniformLocation(shader.PROGRAM_ID, uni.c_str()), unit);
    texPtr->bind();  // 綁到剛設定的 unit
    ++unit;
  }
}

void MeshRenderer::draw(const geom::Mesh& mesh, Shader& shader, uint32_t instanceCount,
                        const std::vector<uint32_t>& uboBindingPoints, unsigned primitive) const {
  shader.use();
  bindTextures_(mesh, shader);
  bindUbos_(mesh, uboBindingPoints);

  mesh.vao.bind();

  const bool hasIndex = !mesh.indices.empty();
  const unsigned prim = toGLPrimitive(primitive);

  if (instanceCount > 1) {
    if (hasIndex) {
      const GLsizei count = static_cast<GLsizei>(mesh.indices.size());
      glDrawElementsInstanced(prim, count, GL_UNSIGNED_INT, nullptr, instanceCount);
    } else {
      const GLsizei count = static_cast<GLsizei>(mesh.vertices.size());
      glDrawArraysInstanced(prim, 0, count, instanceCount);
    }
  } else {
    if (hasIndex) {
      const GLsizei count = static_cast<GLsizei>(mesh.indices.size());
      glDrawElements(prim, count, GL_UNSIGNED_INT, nullptr);
    } else {
      const GLsizei count = static_cast<GLsizei>(mesh.vertices.size());
      glDrawArrays(prim, 0, count);
    }
  }

  mesh.vao.unbind();
  // 負責任清潔：可視需要 unbind textures/UBOs
}

void MeshRenderer::drawRange(const geom::Mesh& mesh, Shader& shader, uint32_t numVertices, uint32_t startIdx,
                             uint32_t instanceCount, const std::vector<uint32_t>& uboBindingPoints,
                             unsigned primitive) const {
  shader.use();
  bindTextures_(mesh, shader);
  bindUbos_(mesh, uboBindingPoints);

  mesh.vao.bind();
  const unsigned prim = toGLPrimitive(primitive);

  // 僅支援非索引（對應你原本的 drawTri）
  if (instanceCount > 1) {
    glDrawArraysInstanced(prim, static_cast<GLint>(startIdx), static_cast<GLsizei>(numVertices),
                          static_cast<GLsizei>(instanceCount));
  } else {
    glDrawArrays(prim, static_cast<GLint>(startIdx), static_cast<GLsizei>(numVertices));
  }
  mesh.vao.unbind();
}

static GLint getModelMatrixLocation(::Shader& shader) {
  // 若不用 UBO，就用傳統 uniform。你可以加個快取 map<PROGRAM_ID, loc>
  return glGetUniformLocation(shader.PROGRAM_ID, "modelMatrix");
}

}  // namespace gfx::render
