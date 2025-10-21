#include "DataObject.hpp"

#include <OPPCH.h>

#include "BasicMesh.hpp"
#include "stb_image.h"

VAO::VAO() { glGenVertexArrays(1, &ID); }

VAO::VAO(VAO &&other) noexcept : ID(other.ID) { other.ID = 0; }

VAO &VAO::operator=(VAO &&other) noexcept {
  if (this != &other) {
    reset();
    ID = other.ID;
    other.ID = 0;
  }
  return *this;
}
void VAO::linkAttr(VBO &vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, const void *offset) {
  vbo.bind();
  glEnableVertexAttribArray(layout);
  glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
  vbo.unbind();
}

void VAO::linkAttrDiv(VBO &vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, const void *offset) {
  vbo.bind();
  glEnableVertexAttribArray(layout);
  glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
  glVertexAttribDivisor(layout, 1);
  vbo.unbind();
}

void VAO::linkMat4(VBO &vbo, GLuint layout) {
  vbo.bind();
  std::size_t vec4Size = sizeof(glm::vec4);

  for (GLuint i = 0; i < 4; i++) {
    glEnableVertexAttribArray(layout + i);
    glVertexAttribPointer(layout + i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(i * vec4Size));
    glVertexAttribDivisor(layout + i, 1);
  }

  vbo.unbind();
}

void VAO::bind() const { glBindVertexArray(ID); }

void VAO::unbind() const { glBindVertexArray(0); }

void VAO::reset() {
  if (ID) {
    glDeleteVertexArrays(1, &ID);
    ID = 0;
  }
}

VBO::VBO() { glGenBuffers(1, &ID); }

VBO::VBO(VBO &&o) noexcept : ID(o.ID) { o.ID = 0; }

VBO &VBO ::operator=(VBO &&o) noexcept {
  if (this != &o) {
    reset();
    ID = o.ID;
    o.ID = 0;
  }
  return *this;
}
void VBO::bind() const { glBindBuffer(GL_ARRAY_BUFFER, ID); }

void VBO::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void VBO::reset() {
  if (ID) {
    glDeleteBuffers(1, &ID);
    ID = 0;
  }
}
// note: EBO no need to unbind
EBO::EBO() { glGenBuffers(1, &ID); }

EBO::EBO(EBO &&o) noexcept : ID(o.ID) { o.ID = 0; }
EBO &EBO::operator=(EBO &&o) noexcept {
  if (this != &o) {
    reset();
    ID = o.ID;
    o.ID = 0;
  }
  return *this;
}

void EBO::bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }

void EBO::bufferData(const std::vector<GLuint> &indices) {
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

void EBO::reset() {
  if (ID) {
    glDeleteBuffers(1, &ID);
    ID = 0;
  }
}

UBO::UBO() { glGenBuffers(1, &ID); }

UBO::UBO(UBO &&o) noexcept : ID(o.ID) { o.ID = 0; }
UBO &UBO::operator=(UBO &&o) noexcept {
  if (this != &o) {
    reset();
    ID = o.ID;
    o.ID = 0;
  }
  return *this;
}
void UBO::bind() const { glBindBuffer(GL_UNIFORM_BUFFER, ID); }

void UBO::unbind() const { glBindBuffer(GL_UNIFORM_BUFFER, 0); }

void UBO::reset() {
  if (ID) {
    glDeleteBuffers(1, &ID);
    ID = 0;
  }
}
FBO::FBO(float width, float height) : width(width), height(height) {
  glGenFramebuffers(1, &ID);
  glGenTextures(1, &textureID);
}

FBO::FBO(FBO &&o) noexcept : ID(o.ID), textureID(o.textureID), width(o.width), height(o.height) {
  o.ID = 0;
  o.textureID = 0;
}
FBO &FBO::operator=(FBO &&o) noexcept {
  if (this != &o) {
    reset();
    ID = o.ID;
    textureID = o.textureID;
    width = o.width;
    height = o.height;
    o.ID = 0;
    o.textureID = 0;
  }
  return *this;
}
void FBO::bind() const { glBindFramebuffer(GL_FRAMEBUFFER, ID); }

void FBO::unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

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

void FBO::reset() {
  if (textureID) {
    glDeleteTextures(1, &textureID);
    textureID = 0;
  }
  if (ID) {
    glDeleteFramebuffers(1, &ID);
    ID = 0;
  }
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

Texture::Texture(Texture &&other) noexcept { *this = std::move(other); }
Texture &Texture::operator=(Texture &&other) noexcept {
  if (this != &other) {
    // 先釋放自己舊的 GL 資源（若你有擁有權）
    if (ID) glDeleteTextures(1, &ID);

    ID = other.ID;
    unit = other.unit;
    type = std::move(other.type);

    other.ID = 0;  // 防止重複刪
  }
  return *this;
}
void Texture::texUnit(Shader &shader, const char *uniform, GLuint unit) {
  glUniform1i(glGetUniformLocation(shader.PROGRAM_ID, uniform), unit);
}

void Texture::bind() const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::reset() {
  if (ID) {
    glDeleteTextures(1, &ID);
  }
}

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
           std::vector<std::shared_ptr<Texture>> textures)
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

void Mesh::setTexture(std::vector<std::shared_ptr<Texture>> textures) { this->textures = std::move(textures); }

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

void Mesh::draw(Shader *shader, const unsigned int instanceCount) {
  vao.bind();

  // Textures
  // TODO: make texture0 not hard-coded
  for (unsigned int i = 0; i < textures.size(); i++) {
    textures[i]->texUnit(*shader, "texture0", i);
    textures[i]->bind();
  }

  if (indices.size() > 0) {
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceCount);
  } else {
    glDrawArraysInstanced(GL_TRIANGLES, 0, vertices.size(), instanceCount);
  }

  vao.unbind();
  for (unsigned int i = 0; i < textures.size(); i++) {
    textures[i]->unbind();
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
    textures[i]->texUnit(*shader, "texture0", i);
    textures[i]->bind();
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
    textures[i]->unbind();
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

void CubeMap::draw(Shader *shader) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  glUniform1i(glGetUniformLocation(shader->PROGRAM_ID, "cubemapTxt"), 0);
  cubeMesh->draw(shader);
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

void PointCloud::draw(Shader *shader) const {
  vao.bind();
  glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(points.size()));
  vao.unbind();
}

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

void GaussianSplat::draw(Shader *shader) const {
  vao.bind();
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<GLsizei>(spheres.size()));
  vao.unbind();
}
