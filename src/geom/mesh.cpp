#include "geom/mesh.hpp"

#include <OPPCH.h>

#include "BasicMesh.hpp"
#include "stb_image.h"

namespace gfx::geom {
Mesh::Mesh(const std::vector<Vertex> &vertices) : vertices(vertices), vao(), vbo(), instanceMatrixVBO(), ebo() {
  vao.bind();
  vbo.bufferData(vertices);

  setupMeshAttributes();

  vao.unbind();
}

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices)
    : vertices(vertices), indices(indices), vao(), vbo(), ebo() {
  vao.bind();
  ebo.bind();
  ebo.bufferData(indices);

  vbo.bufferData(vertices);
  setupMeshAttributes();

  vao.unbind();
}

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices,
           std::vector<std::shared_ptr<resource::Texture>> textures)
    : vertices(vertices), indices(indices), textures(std::move(textures)), vao(), vbo(), ebo() {
  vao.bind();
  ebo.bind();
  ebo.bufferData(indices);

  vbo.bufferData(vertices);
  setupMeshAttributes();

  vao.unbind();
}

void Mesh::setupMeshAttributes() {
  vao.linkAttr(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, position));
  vao.linkAttr(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, normal));
  vao.linkAttr(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, color));
  vao.linkAttr(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
}

unsigned int Mesh::numTriangles() { return indices.size() / 3; }

void Mesh::setTexture(std::vector<std::shared_ptr<resource::Texture>> textures) {
  this->textures = std::move(textures);
}

bool Mesh::hasTexture() const { return !this->textures.empty(); }

void Mesh::setupInstanceMatrices(std::vector<glm::mat4> &instanceMatrices) {
  vao.bind();
  instanceMatrixVBO.bind();
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instanceMatrices.size(), instanceMatrices.data(), GL_STATIC_DRAW);

  vao.linkMat4(instanceMatrixVBO, 4);
  vao.unbind();
}

void Mesh::updateInstanceMatrices(std::vector<glm::mat4> &instanceMatrices) {
  vao.bind();
  instanceMatrixVBO.bind();
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * instanceMatrices.size(), instanceMatrices.data());
  vao.unbind();
}

void Mesh::rotate(float angle, glm::vec3 axis) {
  for (auto &vertex : vertices) {
    vertex.position = glm::rotate(vertex.position, glm::radians(angle), axis);
    vertex.normal = glm::rotate(vertex.normal, glm::radians(angle), axis);
  }

  vao.bind();
  vbo.bind();
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data());
  vbo.unbind();
}

// void Mesh::draw(Shader *shader, const unsigned int instanceCount) {
//   vao.bind();
//
//   // Textures
//   // TODO: make texture0 not hard-coded
//   for (unsigned int i = 0; i < textures.size(); i++) {
//     glUniform1i(glGetUniformLocation(shader->PROGRAM_ID, "texture0"), i);
//     textures[i]->bind();
//   }
//
//   if (indices.size() > 0) {
//     glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceCount);
//   } else {
//     glDrawArraysInstanced(GL_TRIANGLES, 0, vertices.size(), instanceCount);
//   }
//
//   vao.unbind();
//   for (unsigned int i = 0; i < textures.size(); i++) {
//     textures[i]->unbind();
//   }
// }
/*
 * Draw triangles with the given number of vertices.
 */
// void Mesh::drawTri(Shader *shader, const unsigned int numTriangles, const unsigned int startIdx) {
//   vao.bind();
//
//   // Textures
//   for (unsigned int i = 0; i < textures.size(); i++) {
//     glUniform1i(glGetUniformLocation(shader->PROGRAM_ID, "texture0"), i);
//     textures[i]->bind();
//   }
//
//   auto startTriangle = std::min(startIdx * 3, static_cast<unsigned int>(indices.size()));
//   auto numIndices = std::min(numTriangles * 3, static_cast<unsigned int>(indices.size()));
//   if (indices.size() > 0) {
//     glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (void *)(startTriangle * sizeof(GLuint)));
//   } else {
//     glDrawArrays(GL_TRIANGLES, startTriangle, numIndices);
//   }
//
//   vao.unbind();
//   for (unsigned int i = 0; i < textures.size(); i++) {
//     textures[i]->unbind();
//   }
// }

/*
  Flip is true if you are using the skybox.
*/
CubeMap::CubeMap(std::vector<std::string> &faces, bool flip) {
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  // origin order: right, left, top, bottom, front, back
  char order[6] = {'r', 'l', 't', 'd', 'f', 'b'};

  // if flip is true, then the order is: right, left, top, bottom, front, back
  if (flip) std::swap(faces[4], faces[5]);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      if (flip) {
        switch (order[i]) {
          case 'r':
          case 'l':
          case 'f':
          case 'b':
            flipHorizontally(data, width, height, nrChannels);
            break;
          case 't':
          case 'd':
            flipVertically(data, width, height, nrChannels);
            break;
          default:
            break;
        }
      }
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else {
      std::cerr << "Failed to load texture: " << faces[i] << std::endl;
    }
    stbi_image_free(data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  cubeMesh = createCubeMesh(1.0f);
}

CubeMap::~CubeMap() { reset(); }

CubeMap::CubeMap(CubeMap &&other) noexcept : cubeMesh(std::move(other.cubeMesh)), textureID(other.textureID) {
  other.textureID = 0;
}

CubeMap &CubeMap::operator=(CubeMap &&other) noexcept {
  if (this != &other) {
    reset();
    cubeMesh = std::move(other.cubeMesh);
    textureID = other.textureID;
    other.textureID = 0;
  }
  return *this;
}

void CubeMap::reset() {
  if (textureID) {
    glDeleteTextures(1, &textureID);
    textureID = 0;
  }
}

void CubeMap::flipHorizontally(unsigned char *data, int width, int height, int nrChannels) {
  const int rowSize = width * nrChannels;
  for (int y = 0; y < height; ++y) {
    unsigned char *rowStart = data + y * rowSize;
    for (int x = 0; x < width / 2; ++x) {
      int left = x * nrChannels;
      int right = (width - 1 - x) * nrChannels;
      // change the left and right pixels
      for (int c = 0; c < nrChannels; ++c) {
        std::swap(rowStart[left + c], rowStart[right + c]);
      }
    }
  }
}

void CubeMap::flipVertically(unsigned char *data, int width, int height, int nrChannels) {
  const int rowSize = width * nrChannels;
  std::vector<unsigned char> tmp(rowSize);  // 暫存一行的數據

  for (int y = 0; y < height / 2; ++y) {
    unsigned char *topRowStart = data + y * rowSize;
    unsigned char *bottomRowStart = data + (height - 1 - y) * rowSize;

    // 交換整行數據
    std::memcpy(tmp.data(), topRowStart, rowSize);
    std::memcpy(topRowStart, bottomRowStart, rowSize);
    std::memcpy(bottomRowStart, tmp.data(), rowSize);
  }
}
}  // namespace gfx::geom
