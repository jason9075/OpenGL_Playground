#include "render/gs_renderer.hpp"

#include <GL/glew.h>

#include "ShaderClass.hpp"  // 只在這裡依賴 Shader

namespace gfx::render {

void GsRenderer::draw(const gfx::geom::GaussianSplat& gs, ::Shader& shader) const {
  shader.use();

  gs.vao.bind();
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<GLsizei>(gs.spheres.size()));
  gs.vao.unbind();
}

}  // namespace gfx::render
