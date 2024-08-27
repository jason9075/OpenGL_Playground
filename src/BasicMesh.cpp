#include "BasicMesh.hpp"

#include <OPPCH.h>

std::unique_ptr<Mesh> createCuboidMesh(float width, float height, float depth, float x, float y, float z, float r,
                                       float g, float b) {
  // position, normal
  std::vector<Vertex> vertices = {
      // Front face
      {{-width / 2 + x, -height / 2 + y, -depth / 2 + z}, {0.0f, 0.0f, -1.0f}, {r, g, b}},  // 0
      {{width / 2 + x, -height / 2 + y, -depth / 2 + z}, {0.0f, 0.0f, -1.0f}, {r, g, b}},   // 1
      {{width / 2 + x, height / 2 + y, -depth / 2 + z}, {0.0f, 0.0f, -1.0f}, {r, g, b}},    // 2
      {{-width / 2 + x, height / 2 + y, -depth / 2 + z}, {0.0f, 0.0f, -1.0f}, {r, g, b}},   // 3

      // Back face
      {{-width / 2 + x, -height / 2 + y, depth / 2 + z}, {0.0f, 0.0f, 1.0f}, {r, g, b}},  // 4
      {{width / 2 + x, -height / 2 + y, depth / 2 + z}, {0.0f, 0.0f, 1.0f}, {r, g, b}},   // 5
      {{width / 2 + x, height / 2 + y, depth / 2 + z}, {0.0f, 0.0f, 1.0f}, {r, g, b}},    // 6
      {{-width / 2 + x, height / 2 + y, depth / 2 + z}, {0.0f, 0.0f, 1.0f}, {r, g, b}},   // 7

      // Left face
      {{-width / 2 + x, -height / 2 + y, depth / 2 + z}, {-1.0f, 0.0f, 0.0f}, {r, g, b}},   // 8
      {{-width / 2 + x, height / 2 + y, depth / 2 + z}, {-1.0f, 0.0f, 0.0f}, {r, g, b}},    // 9
      {{-width / 2 + x, height / 2 + y, -depth / 2 + z}, {-1.0f, 0.0f, 0.0f}, {r, g, b}},   // 10
      {{-width / 2 + x, -height / 2 + y, -depth / 2 + z}, {-1.0f, 0.0f, 0.0f}, {r, g, b}},  // 11

      // Right face
      {{width / 2 + x, -height / 2 + y, depth / 2 + z}, {1.0f, 0.0f, 0.0f}, {r, g, b}},   // 12
      {{width / 2 + x, height / 2 + y, depth / 2 + z}, {1.0f, 0.0f, 0.0f}, {r, g, b}},    // 13
      {{width / 2 + x, height / 2 + y, -depth / 2 + z}, {1.0f, 0.0f, 0.0f}, {r, g, b}},   // 14
      {{width / 2 + x, -height / 2 + y, -depth / 2 + z}, {1.0f, 0.0f, 0.0f}, {r, g, b}},  // 15

      // Top face
      {{-width / 2 + x, height / 2 + y, -depth / 2 + z}, {0.0f, 1.0f, 0.0f}, {r, g, b}},  // 16
      {{width / 2 + x, height / 2 + y, -depth / 2 + z}, {0.0f, 1.0f, 0.0f}, {r, g, b}},   // 17
      {{width / 2 + x, height / 2 + y, depth / 2 + z}, {0.0f, 1.0f, 0.0f}, {r, g, b}},    // 18
      {{-width / 2 + x, height / 2 + y, depth / 2 + z}, {0.0f, 1.0f, 0.0f}, {r, g, b}},   // 19

      // Bottom face
      {{-width / 2 + x, -height / 2 + y, -depth / 2 + z}, {0.0f, -1.0f, 0.0f}, {r, g, b}},  // 20
      {{width / 2 + x, -height / 2 + y, -depth / 2 + z}, {0.0f, -1.0f, 0.0f}, {r, g, b}},   // 21
      {{width / 2 + x, -height / 2 + y, depth / 2 + z}, {0.0f, -1.0f, 0.0f}, {r, g, b}},    // 22
      {{-width / 2 + x, -height / 2 + y, depth / 2 + z}, {0.0f, -1.0f, 0.0f}, {r, g, b}},   // 23
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

std::unique_ptr<Mesh> createPlaneMesh(const float scale, const glm::vec3 normal) {
  std::vector<Vertex> vertices;
  // Step 1: Find two orthogonal vectors to the normal
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 tangent, bitangent;

  // Check if the normal is parallel to the up vector
  if (glm::abs(glm::dot(normal, up)) > 0.999f) {
    // If parallel, choose a different up vector
    up = glm::vec3(1.0f, 0.0f, 0.0f);
  }

  tangent = glm::normalize(glm::cross(up, normal));
  bitangent = glm::normalize(glm::cross(normal, tangent));

  // Step 2: Define the four vertices of the plane
  vertices.push_back({-scale * tangent - scale * bitangent, normal});
  vertices.push_back({scale * tangent - scale * bitangent, normal});
  vertices.push_back({scale * tangent + scale * bitangent, normal});
  vertices.push_back({-scale * tangent + scale * bitangent, normal});
  std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};

  return std::make_unique<Mesh>(vertices, indices);
}
