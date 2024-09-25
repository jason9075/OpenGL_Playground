#include "DataObject.hpp"

#include <OPPCH.h>

#include "BasicMesh.hpp"
#include "stb_image.h"

VAO::VAO() { glGenVertexArrays(1, &ID); }

void VAO::linkAttr(VBO &VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, const void *offset) {
  VBO.bind();
  glEnableVertexAttribArray(layout);
  glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
  VBO.unbind();
}

void VAO::linkAttrDiv(VBO &VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride,
                      const void *offset) {
  VBO.bind();
  glEnableVertexAttribArray(layout);
  glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
  glVertexAttribDivisor(layout, 1);
  VBO.unbind();
}

void VAO::linkMat4(VBO &VBO, GLuint layout) {
  VBO.bind();
  std::size_t vec4Size = sizeof(glm::vec4);

  for (GLuint i = 0; i < 4; i++) {
    glEnableVertexAttribArray(layout + i);
    glVertexAttribPointer(layout + i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(i * vec4Size));
    glVertexAttribDivisor(layout + i, 1);
  }

  VBO.unbind();
}

void VAO::bind() { glBindVertexArray(ID); }

void VAO::unbind() { glBindVertexArray(0); }

void VAO::del() { glDeleteVertexArrays(1, &ID); }

VBO::VBO() { glGenBuffers(1, &ID); }

void VBO::bind() { glBindBuffer(GL_ARRAY_BUFFER, ID); }

void VBO::unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void VBO::del() { glDeleteBuffers(1, &ID); }

// note: EBO no need to unbind
EBO::EBO() { glGenBuffers(1, &ID); }

void EBO::bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }

void EBO::bufferData(const std::vector<GLuint> &indices) {
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

void EBO::del() { glDeleteBuffers(1, &ID); }

UBO::UBO() { glGenBuffers(1, &ID); }

void UBO::bind() { glBindBuffer(GL_UNIFORM_BUFFER, ID); }

void UBO::unbind() { glBindBuffer(GL_UNIFORM_BUFFER, 0); }

void UBO::del() { glDeleteBuffers(1, &ID); }

FBO::FBO(float width, float height) : width(width), height(height) {
  glGenFramebuffers(1, &ID);
  glGenTextures(1, &textureID);
}

void FBO::bind() { glBindFramebuffer(GL_FRAMEBUFFER, ID); }

void FBO::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void FBO::setupTexture() {
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
  unbind();
}

void FBO::bindTexture(GLenum textureUnit) {
  glActiveTexture(textureUnit);
  glBindTexture(GL_TEXTURE_2D, textureID);
}

void FBO::del() {
  glDeleteFramebuffers(1, &ID);
  glDeleteTextures(1, &textureID);
}

Texture::Texture() : ID(0), unit(0), type("") {}

Texture::Texture(const char *image, const char *texType, GLuint slot) : type(texType) {
  int width, height, nrChannels;

  unsigned char *data = stbi_load(image, &width, &height, &nrChannels, 0);

  glGenTextures(1, &ID);
  glActiveTexture(GL_TEXTURE0 + slot);
  unit = slot;
  glBindTexture(GL_TEXTURE_2D, ID);

  if (data) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    std::cerr << "Failed to load texture: " << image << std::endl;
  }

  if (nrChannels == 4) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  } else if (nrChannels == 3) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  } else if (nrChannels == 1) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
  } else {
    throw std::invalid_argument("Automatic conversion of image channels is not supported.");
  }

  // Generate mipmaps
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader &shader, const char *uniform, GLuint unit) {
  glUniform1i(glGetUniformLocation(shader.ID, uniform), unit);
}

void Texture::bind() {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::del() { glDeleteTextures(1, &ID); }

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
           const std::vector<Texture> &textures)
    : vertices(vertices), indices(indices), textures(textures), vao(), vbo(), ebo() {
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

void Mesh::setTexture(const std::vector<Texture> &textures) { this->textures = textures; }

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

void Mesh::draw(Shader *shader, const unsigned int instanceCount) {
  vao.bind();

  // Textures
  // TODO: make texture0 not hard-coded
  for (unsigned int i = 0; i < textures.size(); i++) {
    textures[i].texUnit(*shader, "texture0", i);
    textures[i].bind();
  }

  if (indices.size() > 0) {
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceCount);
  } else {
    glDrawArraysInstanced(GL_TRIANGLES, 0, vertices.size(), instanceCount);
  }

  vao.unbind();
  for (unsigned int i = 0; i < textures.size(); i++) {
    textures[i].unbind();
  }
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

/*
 * Draw triangles with the given number of vertices.
 */
void Mesh::drawTri(Shader *shader, const unsigned int numTriangles, const unsigned int startIdx) {
  vao.bind();

  // Textures
  for (unsigned int i = 0; i < textures.size(); i++) {
    textures[i].texUnit(*shader, "texture0", i);
    textures[i].bind();
  }

  auto startTriangle = std::min(startIdx * 3, static_cast<unsigned int>(indices.size()));
  auto numIndices = std::min(numTriangles * 3, static_cast<unsigned int>(indices.size()));
  if (indices.size() > 0) {
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (void *)(startTriangle * sizeof(GLuint)));
  } else {
    glDrawArrays(GL_TRIANGLES, startTriangle, numIndices);
  }

  vao.unbind();
  for (unsigned int i = 0; i < textures.size(); i++) {
    textures[i].unbind();
  }
}

void Mesh::del() {
  for (unsigned int i = 0; i < textures.size(); i++) {
    textures[i].del();
  }
  vao.del();
  vbo.del();
  instanceMatrixVBO.del();
  ebo.del();
  for (auto &ubo : ubo) {
    ubo.del();
  }
}

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

void CubeMap::flipHorizontally(unsigned char *data, int width, int height, int nrChannels) {
  int rowSize = width * nrChannels;
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
  int rowSize = width * nrChannels;
  unsigned char *tempRow = new unsigned char[rowSize];  // 暫存一行的數據

  for (int y = 0; y < height / 2; ++y) {
    unsigned char *topRowStart = data + y * rowSize;
    unsigned char *bottomRowStart = data + (height - 1 - y) * rowSize;

    // 交換整行數據
    std::memcpy(tempRow, topRowStart, rowSize);
    std::memcpy(topRowStart, bottomRowStart, rowSize);
    std::memcpy(bottomRowStart, tempRow, rowSize);
  }

  delete[] tempRow;  // 釋放暫存空間
}

void CubeMap::draw(Shader *shader) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  glUniform1i(glGetUniformLocation(shader->ID, "cubemapTxt"), 0);
  cubeMesh->draw(shader);
}

void CubeMap::del() {
  cubeMesh->del();
  glDeleteTextures(1, &textureID);
}

PointCloud::PointCloud(const std::vector<Point> &points) : points(points), vao(), vbo(points) {
  vao.bind();

  vao.linkAttr(vbo, 0, 3, GL_FLOAT, sizeof(Point), (void *)offsetof(Point, position));
  vao.linkAttr(vbo, 1, 3, GL_FLOAT, sizeof(Point), (void *)offsetof(Point, color));
  vao.linkAttr(vbo, 2, 3, GL_FLOAT, sizeof(Point), (void *)offsetof(Point, scale));
  vao.linkAttr(vbo, 3, 4, GL_FLOAT, sizeof(Point), (void *)offsetof(Point, rotation));

  vao.unbind();
}

void PointCloud::draw(Shader *shader) {
  vao.bind();
  glDrawArrays(GL_POINTS, 0, points.size());
  vao.unbind();
}

void PointCloud::del() {
  vao.del();
  vbo.del();
}

GaussianSplat::GaussianSplat(const std::vector<GaussianSphere> &spheres) : vao(), vbo(spheres), spheres(spheres) {
  vao.bind();

  vao.linkAttrDiv(vbo, 0, 3, GL_FLOAT, sizeof(GaussianSphere), (void *)offsetof(GaussianSphere, position));
  vao.linkAttrDiv(vbo, 1, 3, GL_FLOAT, sizeof(GaussianSphere), (void *)offsetof(GaussianSphere, color));
  vao.linkAttrDiv(vbo, 2, 1, GL_FLOAT, sizeof(GaussianSphere), (void *)offsetof(GaussianSphere, opacity));
  vao.linkAttrDiv(vbo, 3, 3, GL_FLOAT, sizeof(GaussianSphere), (void *)offsetof(GaussianSphere, covA));
  vao.linkAttrDiv(vbo, 4, 3, GL_FLOAT, sizeof(GaussianSphere), (void *)offsetof(GaussianSphere, covB));

  vao.unbind();
}

void GaussianSplat::sort(const glm::mat4 &viewMatrix, const bool isAscending) {
  std::sort(spheres.begin(), spheres.end(), [&](const GaussianSphere &a, const GaussianSphere &b) {
    float zA = a.position.x * viewMatrix[0][2] + a.position.y * viewMatrix[1][2] + a.position.z * viewMatrix[2][2] +
               viewMatrix[3][2];
    float zB = b.position.x * viewMatrix[0][2] + b.position.y * viewMatrix[1][2] + b.position.z * viewMatrix[2][2] +
               viewMatrix[3][2];
    return isAscending ? zA < zB : zA > zB;
  });

  // Update the VBO
  vbo.bind();
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GaussianSphere) * spheres.size(), spheres.data());
  vbo.unbind();
}

void GaussianSplat::draw(Shader *shader) {
  vao.bind();
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, spheres.size());
  vao.unbind();
}

void GaussianSplat::del() {
  vao.del();
  vbo.del();
}
