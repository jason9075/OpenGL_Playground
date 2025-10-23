#include "BasicMesh.hpp"

#include <OPPCH.h>

std::unique_ptr<gfx::geom::Mesh> createCuboidMesh(float width, float height, float depth, glm::vec3 pos,
                                                  glm::vec3 color) {
  glm::vec3 p0 = glm::vec3(-width / 2 + pos[0], -height / 2 + pos[1], -depth / 2 + pos[2]);
  glm::vec3 p1 = glm::vec3(width / 2 + pos[0], -height / 2 + pos[1], -depth / 2 + pos[2]);
  glm::vec3 p2 = glm::vec3(width / 2 + pos[0], height / 2 + pos[1], -depth / 2 + pos[2]);
  glm::vec3 p3 = glm::vec3(-width / 2 + pos[0], height / 2 + pos[1], -depth / 2 + pos[2]);
  glm::vec3 p4 = glm::vec3(-width / 2 + pos[0], -height / 2 + pos[1], depth / 2 + pos[2]);
  glm::vec3 p5 = glm::vec3(width / 2 + pos[0], -height / 2 + pos[1], depth / 2 + pos[2]);
  glm::vec3 p6 = glm::vec3(width / 2 + pos[0], height / 2 + pos[1], depth / 2 + pos[2]);
  glm::vec3 p7 = glm::vec3(-width / 2 + pos[0], height / 2 + pos[1], depth / 2 + pos[2]);

  glm::vec3 normals[] = {
      glm::vec3(0.0f, 0.0f, 1.0f),   // Front
      glm::vec3(0.0f, 0.0f, -1.0f),  // Back
      glm::vec3(-1.0f, 0.0f, 0.0f),  // Left
      glm::vec3(1.0f, 0.0f, 0.0f),   // Right
      glm::vec3(0.0f, 1.0f, 0.0f),   // Top
      glm::vec3(0.0f, -1.0f, 0.0f)   // Bottom
  };
  glm::vec2 texCoords[] = {{0.0f, 0.0f},  // UV coordinates
                           {1.0f, 0.0f},
                           {1.0f, 1.0f},
                           {0.0f, 1.0f}};

  std::vector<gfx::geom::Vertex> vertices = {};

  // Front face
  vertices.push_back({p4, normals[0], color, texCoords[0]});
  vertices.push_back({p5, normals[0], color, texCoords[1]});
  vertices.push_back({p6, normals[0], color, texCoords[2]});
  vertices.push_back({p7, normals[0], color, texCoords[3]});
  // Back face
  vertices.push_back({p0, normals[1], color, texCoords[0]});
  vertices.push_back({p1, normals[1], color, texCoords[1]});
  vertices.push_back({p2, normals[1], color, texCoords[2]});
  vertices.push_back({p3, normals[1], color, texCoords[3]});
  // Left face
  vertices.push_back({p0, normals[2], color, texCoords[0]});
  vertices.push_back({p4, normals[2], color, texCoords[1]});
  vertices.push_back({p7, normals[2], color, texCoords[2]});
  vertices.push_back({p3, normals[2], color, texCoords[3]});
  // Right face
  vertices.push_back({p1, normals[3], color, texCoords[0]});
  vertices.push_back({p5, normals[3], color, texCoords[1]});
  vertices.push_back({p6, normals[3], color, texCoords[2]});
  vertices.push_back({p2, normals[3], color, texCoords[3]});

  // Top face
  vertices.push_back({p3, normals[4], color, texCoords[0]});
  vertices.push_back({p7, normals[4], color, texCoords[1]});
  vertices.push_back({p6, normals[4], color, texCoords[2]});
  vertices.push_back({p2, normals[4], color, texCoords[3]});

  // Bottom face
  vertices.push_back({p0, normals[5], color, texCoords[0]});
  vertices.push_back({p4, normals[5], color, texCoords[1]});
  vertices.push_back({p5, normals[5], color, texCoords[2]});
  vertices.push_back({p1, normals[5], color, texCoords[3]});

  std::vector<GLuint> indices = {
      0,  1,  2,  2,  3,  0,   // Front face
      4,  5,  6,  6,  7,  4,   // Back face
      8,  9,  10, 10, 11, 8,   // Left face
      12, 13, 14, 14, 15, 12,  // Right face
      16, 17, 18, 18, 19, 16,  // Top face
      20, 21, 22, 22, 23, 20,  // Bottom face
  };

  return std::make_unique<gfx::geom::Mesh>(vertices, indices);
}

std::unique_ptr<gfx::geom::Mesh> createCubeMesh(float scale, glm::vec3 color) {
  glm::vec3 p0 = glm::vec3(-scale, -scale, scale);
  glm::vec3 p1 = glm::vec3(scale, -scale, scale);
  glm::vec3 p2 = glm::vec3(scale, scale, scale);
  glm::vec3 p3 = glm::vec3(-scale, scale, scale);
  glm::vec3 p4 = glm::vec3(-scale, -scale, -scale);
  glm::vec3 p5 = glm::vec3(scale, -scale, -scale);
  glm::vec3 p6 = glm::vec3(scale, scale, -scale);
  glm::vec3 p7 = glm::vec3(-scale, scale, -scale);

  glm::vec3 normals[] = {
      glm::vec3(0.0f, 0.0f, 1.0f),   // Front
      glm::vec3(0.0f, 0.0f, -1.0f),  // Back
      glm::vec3(-1.0f, 0.0f, 0.0f),  // Left
      glm::vec3(1.0f, 0.0f, 0.0f),   // Right
      glm::vec3(0.0f, 1.0f, 0.0f),   // Top
      glm::vec3(0.0f, -1.0f, 0.0f)   // Bottom
  };
  glm::vec2 texCoords[] = {{0.0f, 0.0f},  // UV coordinates
                           {1.0f, 0.0f},
                           {1.0f, 1.0f},
                           {0.0f, 1.0f}};

  std::vector<gfx::geom::Vertex> vertices = {};

  // Front face
  vertices.push_back({p0, normals[0], color, texCoords[0]});
  vertices.push_back({p1, normals[0], color, texCoords[1]});
  vertices.push_back({p2, normals[0], color, texCoords[2]});
  vertices.push_back({p3, normals[0], color, texCoords[3]});
  // Back face
  vertices.push_back({p4, normals[1], color, texCoords[0]});
  vertices.push_back({p5, normals[1], color, texCoords[1]});
  vertices.push_back({p6, normals[1], color, texCoords[2]});
  vertices.push_back({p7, normals[1], color, texCoords[3]});
  // Left face
  vertices.push_back({p0, normals[2], color, texCoords[0]});
  vertices.push_back({p4, normals[2], color, texCoords[1]});
  vertices.push_back({p7, normals[2], color, texCoords[2]});
  vertices.push_back({p3, normals[2], color, texCoords[3]});

  // Right face
  vertices.push_back({p1, normals[3], color, texCoords[0]});
  vertices.push_back({p5, normals[3], color, texCoords[1]});
  vertices.push_back({p6, normals[3], color, texCoords[2]});
  vertices.push_back({p2, normals[3], color, texCoords[3]});
  // Top face
  vertices.push_back({p3, normals[4], color, texCoords[0]});
  vertices.push_back({p7, normals[4], color, texCoords[1]});
  vertices.push_back({p6, normals[4], color, texCoords[2]});
  vertices.push_back({p2, normals[4], color, texCoords[3]});
  // Bottom face
  vertices.push_back({p0, normals[5], color, texCoords[0]});
  vertices.push_back({p4, normals[5], color, texCoords[1]});
  vertices.push_back({p5, normals[5], color, texCoords[2]});
  vertices.push_back({p1, normals[5], color, texCoords[3]});

  std::vector<GLuint> indices = {
      0,  1,  2,  2,  3,  0,   // Front face
      4,  5,  6,  6,  7,  4,   // Back face
      8,  9,  10, 10, 11, 8,   // Left face
      12, 13, 14, 14, 15, 12,  // Right face
      16, 17, 18, 18, 19, 16,  // Top face
      20, 21, 22, 22, 23, 20,  // Bottom face
  };

  return std::make_unique<gfx::geom::Mesh>(vertices, indices);
}

std::unique_ptr<gfx::geom::Mesh> createPlaneMesh(const float scale, const glm::vec3 normal, const glm::vec3 color,
                                                 const glm::vec3 center) {
  std::vector<gfx::geom::Vertex> vertices;
  // Step 1: Find two orthogonal vectors to the normal
  glm::vec3 tangent, bitangent;

  glm::vec3 n = glm::normalize(normal);
  glm::vec3 up = (std::abs(n.y) > 0.9f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);

  tangent = glm::normalize(glm::cross(up, normal));
  bitangent = glm::normalize(glm::cross(normal, tangent));

  glm::vec3 p0 = -scale * tangent - scale * bitangent;
  glm::vec3 p1 = scale * tangent - scale * bitangent;
  glm::vec3 p2 = scale * tangent + scale * bitangent;
  glm::vec3 p3 = -scale * tangent + scale * bitangent;

  // Step 2: Define the four vertices of the plane
  vertices.push_back({p0, normal, color, {0.0f, 0.0f}});
  vertices.push_back({p1, normal, color, {1.0f, 0.0f}});
  vertices.push_back({p2, normal, color, {1.0f, 1.0f}});
  vertices.push_back({p3, normal, color, {0.0f, 1.0f}});

  for (auto& vertex : vertices) {
    vertex.position += center;
  }
  std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};

  return std::make_unique<gfx::geom::Mesh>(vertices, indices);
}
// Helper function to add a cuboid to the mesh
void addCuboid(std::vector<gfx::geom::Vertex>& vertices, std::vector<GLuint>& indices, glm::vec3 center, float width,
               float height, float depth, glm::vec3 color) {
  float hw = width / 2.0f;
  float hh = height / 2.0f;
  float hd = depth / 2.0f;

  // Positions of the 8 vertices
  glm::vec3 p0 = center + glm::vec3(-hw, -hh, -hd);  // 0
  glm::vec3 p1 = center + glm::vec3(hw, -hh, -hd);   // 1
  glm::vec3 p2 = center + glm::vec3(hw, hh, -hd);    // 2
  glm::vec3 p3 = center + glm::vec3(-hw, hh, -hd);   // 3
  glm::vec3 p4 = center + glm::vec3(-hw, -hh, hd);   // 4
  glm::vec3 p5 = center + glm::vec3(hw, -hh, hd);    // 5
  glm::vec3 p6 = center + glm::vec3(hw, hh, hd);     // 6
  glm::vec3 p7 = center + glm::vec3(-hw, hh, hd);    // 7

  // Normals for each face
  glm::vec3 normals[] = {
      glm::vec3(0.0f, 0.0f, 1.0f),   // Front
      glm::vec3(0.0f, 0.0f, -1.0f),  // Back
      glm::vec3(-1.0f, 0.0f, 0.0f),  // Left
      glm::vec3(1.0f, 0.0f, 0.0f),   // Right
      glm::vec3(0.0f, 1.0f, 0.0f),   // Top
      glm::vec3(0.0f, -1.0f, 0.0f)   // Bottom
  };

  // Texture coordinates (can be customized as needed)
  glm::vec2 texCoords[] = {{0.0f, 0.0f},  // UV coordinates
                           {1.0f, 0.0f},
                           {1.0f, 1.0f},
                           {0.0f, 1.0f}};

  // Start index for indices
  GLuint baseIndex = static_cast<GLuint>(vertices.size());

  // Front face
  vertices.push_back({p4, normals[0], color, texCoords[0]});
  vertices.push_back({p5, normals[0], color, texCoords[1]});
  vertices.push_back({p6, normals[0], color, texCoords[2]});
  vertices.push_back({p7, normals[0], color, texCoords[3]});
  indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2, baseIndex + 2, baseIndex + 3, baseIndex});

  // Back face
  baseIndex += 4;
  vertices.push_back({p0, normals[1], color, texCoords[0]});
  vertices.push_back({p1, normals[1], color, texCoords[1]});
  vertices.push_back({p2, normals[1], color, texCoords[2]});
  vertices.push_back({p3, normals[1], color, texCoords[3]});
  indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2, baseIndex + 2, baseIndex + 3, baseIndex});

  // Left face
  baseIndex += 4;
  vertices.push_back({p0, normals[2], color, texCoords[0]});
  vertices.push_back({p4, normals[2], color, texCoords[1]});
  vertices.push_back({p7, normals[2], color, texCoords[2]});
  vertices.push_back({p3, normals[2], color, texCoords[3]});
  indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2, baseIndex + 2, baseIndex + 3, baseIndex});

  // Right face
  baseIndex += 4;
  vertices.push_back({p1, normals[3], color, texCoords[0]});
  vertices.push_back({p5, normals[3], color, texCoords[1]});
  vertices.push_back({p6, normals[3], color, texCoords[2]});
  vertices.push_back({p2, normals[3], color, texCoords[3]});
  indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2, baseIndex + 2, baseIndex + 3, baseIndex});

  // Top face
  baseIndex += 4;
  vertices.push_back({p3, normals[4], color, texCoords[0]});
  vertices.push_back({p7, normals[4], color, texCoords[1]});
  vertices.push_back({p6, normals[4], color, texCoords[2]});
  vertices.push_back({p2, normals[4], color, texCoords[3]});
  indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2, baseIndex + 2, baseIndex + 3, baseIndex});

  // Bottom face
  baseIndex += 4;
  vertices.push_back({p0, normals[5], color, texCoords[0]});
  vertices.push_back({p4, normals[5], color, texCoords[1]});
  vertices.push_back({p5, normals[5], color, texCoords[2]});
  vertices.push_back({p1, normals[5], color, texCoords[3]});
  indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2, baseIndex + 2, baseIndex + 3, baseIndex});
}

void addTriPrism(std::vector<gfx::geom::Vertex>& vertices, std::vector<GLuint>& indices, glm::vec3 center, float width,
                 float height, float depth, glm::vec3 color) {
  float hw = width / 2.0f;
  float hh = height / 2.0f;
  float hd = depth / 2.0f;

  /*

               p3______p0
               /\      \
              /  \      \
             /    \      \
            /      \      \
           /        \      \
         p4__________p5_____p2
  */
  // Positions of the 6 vertices
  glm::vec3 p0 = center + glm::vec3(0, hh, -hd);     // 0
  glm::vec3 p1 = center + glm::vec3(-hw, -hh, -hd);  // 1
  glm::vec3 p2 = center + glm::vec3(hw, -hh, -hd);   // 2
  glm::vec3 p3 = center + glm::vec3(0, hh, hd);      // 3
  glm::vec3 p4 = center + glm::vec3(-hw, -hh, hd);   // 4
  glm::vec3 p5 = center + glm::vec3(hw, -hh, hd);    // 5

  // Normals for each face
  glm::vec3 normals[] = {
      glm::vec3(0.0f, 0.0f, -1.0f),  // Front face normal
      glm::vec3(0.0f, 0.0f, 1.0f),   // Back face normal
      glm::vec3(-hw, hh, 0.0f),      // Left face normal (computed later)
      glm::vec3(hw, hh, 0.0f),       // Right face normal (computed later)
      glm::vec3(0.0f, -1.0f, 0.0f)   // Bottom face normal
  };

  // Normalize the normals
  normals[2] = glm::normalize(glm::cross(p1 - p0, p4 - p0));  // Left face normal
  normals[3] = glm::normalize(glm::cross(p5 - p0, p2 - p0));  // Right face normal

  // Texture coordinates (optional)
  glm::vec2 texCoords[] = {
      {0.5f, 1.0f},  // Top vertex
      {0.0f, 0.0f},  // Bottom left vertex
      {1.0f, 0.0f}   // Bottom right vertex
  };

  // Start index for indices
  GLuint baseIndex = static_cast<GLuint>(vertices.size());

  // Front face (triangle)
  vertices.push_back({p0, normals[0], color, texCoords[0]});
  vertices.push_back({p1, normals[0], color, texCoords[1]});
  vertices.push_back({p2, normals[0], color, texCoords[2]});
  indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2});

  // Back face (triangle)
  baseIndex += 3;
  vertices.push_back({p3, normals[1], color, texCoords[0]});
  vertices.push_back({p5, normals[1], color, texCoords[2]});
  vertices.push_back({p4, normals[1], color, texCoords[1]});
  indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2});

  // Left face (quad)
  baseIndex += 3;
  vertices.push_back({p0, normals[2], color, texCoords[0]});
  vertices.push_back({p3, normals[2], color, texCoords[1]});
  vertices.push_back({p4, normals[2], color, texCoords[2]});
  vertices.push_back({p1, normals[2], color, texCoords[2]});
  indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2, baseIndex, baseIndex + 2, baseIndex + 3});

  // Right face (quad)
  baseIndex += 4;
  vertices.push_back({p0, normals[3], color, texCoords[0]});
  vertices.push_back({p2, normals[3], color, texCoords[1]});
  vertices.push_back({p5, normals[3], color, texCoords[2]});
  vertices.push_back({p3, normals[3], color, texCoords[2]});
  indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2, baseIndex, baseIndex + 2, baseIndex + 3});

  // Bottom face (quad)
  baseIndex += 4;
  vertices.push_back({p1, normals[4], color, texCoords[0]});
  vertices.push_back({p4, normals[4], color, texCoords[1]});
  vertices.push_back({p5, normals[4], color, texCoords[2]});
  vertices.push_back({p2, normals[4], color, texCoords[2]});
  indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2, baseIndex, baseIndex + 2, baseIndex + 3});
}
// 框形 Mesh
// use 4 cuboid to create a frame
std::unique_ptr<gfx::geom::Mesh> createFrameMesh(float width, float height, float depth, float thickness,
                                                 glm::vec3 position, glm::vec3 color) {
  std::vector<gfx::geom::Vertex> vertices;
  std::vector<GLuint> indices;

  // Left vertical side
  glm::vec3 leftPos = position + glm::vec3(-(width - thickness) / 2.0f, 0.0f, 0.0f);
  addCuboid(vertices, indices, leftPos, thickness, height, depth, color);

  // Right vertical side
  glm::vec3 rightPos = position + glm::vec3((width - thickness) / 2.0f, 0.0f, 0.0f);
  addCuboid(vertices, indices, rightPos, thickness, height, depth, color);

  // Top horizontal side
  glm::vec3 topPos = position + glm::vec3(0.0f, (height - thickness) / 2.0f, 0.0f);
  addCuboid(vertices, indices, topPos, width, thickness, depth, color);

  // Bottom horizontal side
  glm::vec3 bottomPos = position + glm::vec3(0.0f, -(height - thickness) / 2.0f, 0.0f);
  addCuboid(vertices, indices, bottomPos, width, thickness, depth, color);

  return std::make_unique<gfx::geom::Mesh>(vertices, indices);
}

std::unique_ptr<gfx::geom::Mesh> createArrowMesh(float shaftWidth, float shaftHeight, float headWidth, float headHeight,
                                                 float thickness, glm::vec3 position, glm::vec3 color) {
  std::vector<gfx::geom::Vertex> vertices;
  std::vector<GLuint> indices;

  // Calculate half dimensions
  float hsh = shaftHeight / 2.0f;
  float hhh = headHeight / 2.0f;

  // Positions for the shaft (cuboid)
  addTriPrism(vertices, indices, position + glm::vec3(0.0f, hsh, 0.0f), headWidth, headHeight, thickness, color);
  addCuboid(vertices, indices, position - glm::vec3(0.0f, hhh, 0.0f), shaftWidth, shaftHeight, thickness, color);

  return std::make_unique<gfx::geom::Mesh>(vertices, indices);
}

std::unique_ptr<gfx::geom::Mesh> createSphereMesh(float radius, glm::vec3 pos, glm::vec3 color, int rings,
                                                  int sectors) {
  rings = std::max(3, rings);
  sectors = std::max(3, sectors);

  std::vector<gfx::geom::Vertex> vertices;
  vertices.reserve((rings + 1) * (sectors + 1));

  std::vector<GLuint> indices;
  indices.reserve(rings * sectors * 6);

  const float PI = 3.14159265358979323846f;
  const float TWO_PI = 2.0f * PI;

  // 產生頂點
  for (int i = 0; i <= rings; ++i) {
    const float v = static_cast<float>(i) / rings;  // [0,1]
    const float theta = v * PI;                     // [0,π]
    const float y = std::cos(theta);
    const float r = std::sin(theta);

    for (int j = 0; j <= sectors; ++j) {
      const float u = static_cast<float>(j) / sectors;  // [0,1]
      const float phi = u * TWO_PI;                     // [0,2π]
      const float x = r * std::cos(phi);
      const float z = r * std::sin(phi);

      glm::vec3 p = pos + radius * glm::vec3(x, y, z);
      glm::vec3 n = glm::normalize(glm::vec3(x, y, z));  // 從球心指向外
      glm::vec2 uv(u, v);

      vertices.push_back({p, n, color, uv});
    }
  }

  // 產生索引（每個格子兩個三角形）
  const int stride = sectors + 1;
  for (int i = 0; i < rings; ++i) {
    for (int j = 0; j < sectors; ++j) {
      GLuint i0 = i * stride + j;
      GLuint i1 = i * stride + (j + 1);
      GLuint i2 = (i + 1) * stride + (j + 1);
      GLuint i3 = (i + 1) * stride + j;

      // CCW：i0 -> i1 -> i2,  i2 -> i3 -> i0（外向面）
      indices.push_back(i0);
      indices.push_back(i1);
      indices.push_back(i2);

      indices.push_back(i2);
      indices.push_back(i3);
      indices.push_back(i0);
    }
  }

  return std::make_unique<gfx::geom::Mesh>(vertices, indices);
}
