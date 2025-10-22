#include "render/cubemap_renderer.hpp"

#include <GL/glew.h>

#include "ShaderClass.hpp"  // 只在這裡依賴 Shader
#include "geom/mesh.hpp"    // 取得 VAO/VBO/EBO/Textures/UBO

namespace gfx::render {

void CubeMapRenderer::draw(const geom::CubeMap& cm, Shader& shader) const {
  shader.use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cm.textureID);
  glUniform1i(glGetUniformLocation(shader.PROGRAM_ID, "cubemapTxt"), 0);

  meshRenderer_.draw(*cm.cubeMesh, shader, 1);
}

}  // namespace gfx::render
