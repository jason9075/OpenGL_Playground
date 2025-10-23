#include "geom/gaussianSplat.hpp"

#include <OPPCH.h>

namespace gfx::geom {

GaussianSplat::GaussianSplat(const std::vector<GaussianSphere> &spheres) : spheres(spheres), vao(), vbo(spheres) {
  vao.bind();

  vao.linkAttrDiv(vbo, 0, 3, GL_FLOAT, sizeof(GaussianSphere), (void *)offsetof(GaussianSphere, position));
  vao.linkAttrDiv(vbo, 1, 3, GL_FLOAT, sizeof(GaussianSphere), (void *)offsetof(GaussianSphere, color));
  vao.linkAttrDiv(vbo, 2, 1, GL_FLOAT, sizeof(GaussianSphere), (void *)offsetof(GaussianSphere, opacity));
  vao.linkAttrDiv(vbo, 3, 3, GL_FLOAT, sizeof(GaussianSphere), (void *)offsetof(GaussianSphere, covA));
  vao.linkAttrDiv(vbo, 4, 3, GL_FLOAT, sizeof(GaussianSphere), (void *)offsetof(GaussianSphere, covB));

  vao.unbind();
}

GaussianSplat::GaussianSplat(GaussianSplat &&other) noexcept
    : spheres(std::move(other.spheres)), vao(std::move(other.vao)), vbo(std::move(other.vbo)) {}

GaussianSplat &GaussianSplat::operator=(GaussianSplat &&other) noexcept {
  if (this != &other) {
    spheres = std::move(other.spheres);
    vao = std::move(other.vao);
    vbo = std::move(other.vbo);
  }
  return *this;
}
void GaussianSplat::sort(const glm::mat4 &viewMatrix, const bool isAscending) {
  std::sort(spheres.begin(), spheres.end(), [&](const GaussianSphere &a, const GaussianSphere &b) {
    auto Az = (viewMatrix * glm::vec4(a.position, 1.0f)).z;
    auto Bz = (viewMatrix * glm::vec4(b.position, 1.0f)).z;
    return isAscending ? Az < Bz : Az > Bz;
  });

  // Update the VBO
  vbo.bind();
  glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(sizeof(GaussianSphere) * spheres.size()), spheres.data());
  vbo.unbind();
}

}  // namespace gfx::geom
