#include "render/model_renderer.hpp"

#include <GL/glew.h>

#include "ShaderClass.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace gfx::render {

static GLint getModelMatrixLocation(::Shader& shader) {
  // 若不用 UBO，就用傳統 uniform。你可以加個快取 map<PROGRAM_ID, loc>
  return glGetUniformLocation(shader.PROGRAM_ID, "modelMatrix");
}

void ModelRenderer::draw(const Model& model, ::Shader& shader, uint32_t instanceCount,
                         const std::vector<uint32_t>& uboBindingPoints) const {
  shader.use();

  if (usePerObjectUBO_) {
    // 用 UBO（std140）傳 modelMatrix
    // 你可以讓 Model 持有一個 UBO（PerObject），或這裡用臨時 UBO（建議長期持有）。
    GLuint ubo = 0;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), glm::value_ptr(model.modelMatrix), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, perObjectBinding_, ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // 逐 mesh 繪製
    for (const auto& m : model.meshes) {
      meshRenderer_.draw(m, shader, instanceCount, uboBindingPoints);
    }

    glDeleteBuffers(1, &ubo);
  } else {
    // 傳統 uniform
    const GLint loc = getModelMatrixLocation(shader);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model.modelMatrix));

    for (const auto& m : model.meshes) {
      meshRenderer_.draw(m, shader, instanceCount, uboBindingPoints);
    }
  }
}

void ModelRenderer::drawTri(const Model& model, ::Shader& shader, uint32_t numVertices, uint32_t instanceCount,
                            const std::vector<uint32_t>& uboBindingPoints) const {
  shader.use();

  if (usePerObjectUBO_) {
    GLuint ubo = 0;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), glm::value_ptr(model.modelMatrix), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, perObjectBinding_, ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    for (const auto& m : model.meshes) {
      meshRenderer_.drawRange(m, shader, numVertices, /*startIdx=*/0, instanceCount, uboBindingPoints);
    }

    glDeleteBuffers(1, &ubo);
  } else {
    const GLint loc = getModelMatrixLocation(shader);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model.modelMatrix));

    for (const auto& m : model.meshes) {
      meshRenderer_.drawRange(m, shader, numVertices, /*startIdx=*/0, instanceCount, uboBindingPoints);
    }
  }
}
}  // namespace gfx::render
