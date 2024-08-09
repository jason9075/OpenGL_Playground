#include "BasicMesh.h"

#include <OPPCH.h>

std::unique_ptr<Mesh> createCubeMesh(float scale) {
  // position, normal
  std::vector<Vertex> vertices = {
      // Front face
      {{-scale, -scale, -scale}, {0.0f, 0.0f, -1.0f}},  // 0
      {{scale, -scale, -scale}, {0.0f, 0.0f, -1.0f}},   // 1
      {{scale, scale, -scale}, {0.0f, 0.0f, -1.0f}},    // 2
      {{-scale, scale, -scale}, {0.0f, 0.0f, -1.0f}},   // 3

      // Back face
      {{-scale, -scale, scale}, {0.0f, 0.0f, 1.0f}},  // 4
      {{scale, -scale, scale}, {0.0f, 0.0f, 1.0f}},   // 5
      {{scale, scale, scale}, {0.0f, 0.0f, 1.0f}},    // 6
      {{-scale, scale, scale}, {0.0f, 0.0f, 1.0f}},   // 7

      // Left face
      {{-scale, -scale, scale}, {-1.0f, 0.0f, 0.0f}},   // 8
      {{-scale, scale, scale}, {-1.0f, 0.0f, 0.0f}},    // 9
      {{-scale, scale, -scale}, {-1.0f, 0.0f, 0.0f}},   // 10
      {{-scale, -scale, -scale}, {-1.0f, 0.0f, 0.0f}},  // 11

      // Right face
      {{scale, -scale, scale}, {1.0f, 0.0f, 0.0f}},   // 12
      {{scale, scale, scale}, {1.0f, 0.0f, 0.0f}},    // 13
      {{scale, scale, -scale}, {1.0f, 0.0f, 0.0f}},   // 14
      {{scale, -scale, -scale}, {1.0f, 0.0f, 0.0f}},  // 15

      // Top face
      {{-scale, scale, -scale}, {0.0f, 1.0f, 0.0f}},  // 16
      {{scale, scale, -scale}, {0.0f, 1.0f, 0.0f}},   // 17
      {{scale, scale, scale}, {0.0f, 1.0f, 0.0f}},    // 18
      {{-scale, scale, scale}, {0.0f, 1.0f, 0.0f}},   // 19

      // Bottom face
      {{-scale, -scale, -scale}, {0.0f, -1.0f, 0.0f}},  // 20
      {{scale, -scale, -scale}, {0.0f, -1.0f, 0.0f}},   // 21
      {{scale, -scale, scale}, {0.0f, -1.0f, 0.0f}},    // 22
      {{-scale, -scale, scale}, {0.0f, -1.0f, 0.0f}},   // 23
  };
  std::vector<GLuint> indices = {
      0,  1,  2,  2,  3,  0,   // Front face
      4,  5,  6,  6,  7,  4,   // Back face
      8,  9,  10, 10, 11, 8,   // Left face
      12, 13, 14, 14, 15, 12,  // Right face
      16, 17, 18, 18, 19, 16,  // Top face
      20, 21, 22, 22, 23, 20,  // Bottom face
  };

  return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> createPlaneMesh() {
  std::vector<Vertex> vertices = {
      {{-1.0f, -1.0f, 0.0f}},  // bottom left
      {{1.0f, -1.0f, 0.0f}},   // bottom right
      {{1.0f, 1.0f, 0.0f}},    // top right
      {{-1.0f, 1.0f, 0.0f}},   // top left
  };
  std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};

  return std::make_unique<Mesh>(vertices, indices);
}
