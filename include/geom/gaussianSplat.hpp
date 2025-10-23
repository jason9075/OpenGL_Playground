#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "core/vao.hpp"
#include "core/vbo.hpp"

namespace gfx::geom {

struct GaussianSphere {
  glm::vec3 position;
  glm::vec3 color;
  float opacity;
  glm::vec3 covA;
  glm::vec3 covB;
};

class GaussianSplat {
 public:
  GaussianSplat(const std::vector<GaussianSphere> &spheres);
  ~GaussianSplat() = default;

  GaussianSplat(const GaussianSplat &) = delete;
  GaussianSplat &operator=(const GaussianSplat &) = delete;
  GaussianSplat(GaussianSplat &&other) noexcept;
  GaussianSplat &operator=(GaussianSplat &&other) noexcept;

  core::VAO vao;
  std::vector<GaussianSphere> spheres;
  void sort(const glm::mat4 &viewMatrix, const bool isAscending = true);

 private:
  core::VBO vbo;
};

}  // namespace gfx::geom
