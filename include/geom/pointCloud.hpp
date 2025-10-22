#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "core/vao.hpp"
#include "core/vbo.hpp"

namespace gfx::geom {

struct Point {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 scale;
  glm::vec4 rotation;
};

class PointCloud {
 public:
  PointCloud(const std::vector<Point> &points);
  ~PointCloud() = default;
  PointCloud(const PointCloud &) = delete;
  PointCloud &operator=(const PointCloud &) = delete;
  PointCloud(PointCloud &&other) noexcept;
  PointCloud &operator=(PointCloud &&other) noexcept;

  // void draw(Shader *shader) const;

 private:
  std::vector<Point> points;
  core::VAO vao;
  core::VBO vbo;
};

}  // namespace gfx::geom
