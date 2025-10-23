#include "geom/pointCloud.hpp"

#include <OPPCH.h>

namespace gfx::geom {

PointCloud::PointCloud(const std::vector<Point> &points) : points(points), vao(), vbo(points) {
  vao.bind();

  vao.linkAttr(vbo, 0, 3, GL_FLOAT, sizeof(Point), (void *)offsetof(Point, position));
  vao.linkAttr(vbo, 1, 3, GL_FLOAT, sizeof(Point), (void *)offsetof(Point, color));
  vao.linkAttr(vbo, 2, 3, GL_FLOAT, sizeof(Point), (void *)offsetof(Point, scale));
  vao.linkAttr(vbo, 3, 4, GL_FLOAT, sizeof(Point), (void *)offsetof(Point, rotation));

  vao.unbind();
}

PointCloud::PointCloud(PointCloud &&other) noexcept
    : points(std::move(other.points)), vao(std::move(other.vao)), vbo(std::move(other.vbo)) {}

PointCloud &PointCloud::operator=(PointCloud &&other) noexcept {
  if (this != &other) {
    // 成員自己在 move assign 內會處理原資源釋放（或我們先 swap 再讓舊的自動析構）
    points = std::move(other.points);
    vao = std::move(other.vao);
    vbo = std::move(other.vbo);
  }
  return *this;
}

}  // namespace gfx::geom
