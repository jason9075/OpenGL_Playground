#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "core/ebo.hpp"
#include "core/ubo.hpp"
#include "core/vao.hpp"
#include "core/vbo.hpp"
#include "resource/texture.hpp"

namespace gfx::geom {

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
  glm::vec2 texCoords;
  glm::mat4 modelMatrix;
};
class Mesh {
 public:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<std::shared_ptr<resource::Texture>> textures;

  core::VAO vao;
  core::VBO vbo;
  core::VBO instanceMatrixVBO;
  core::EBO ebo;
  std::vector<core::UBO> ubo;

  Mesh(const std::vector<Vertex> &vertices);
  Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);
  Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices,
       std::vector<std::shared_ptr<resource::Texture>> textures);

  ~Mesh() = default;
  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;
  Mesh(Mesh &&) noexcept = default;
  Mesh &operator=(Mesh &&) noexcept = default;

  unsigned int numTriangles();

  template <typename T>
  void setupUBO(const T *data, size_t count, GLenum usage, size_t index = 0) {
    if (ubo.size() <= index) {
      ubo.resize(index + 1);
    }
    ubo[index].bind();
    ubo[index].bufferData(data, count, usage);
    ubo[index].unbind();
  }

  template <typename T>
  void updateUBO(const T *data, size_t count, size_t index = 0) {
    ubo[index].bind();
    ubo[index].bufferSubData(data, count);
    ubo[index].unbind();
  }
  void setTexture(std::vector<std::shared_ptr<resource::Texture>> textures);
  bool hasTexture() const;
  void setupInstanceMatrices(std::vector<glm::mat4> &instanceMatrices);
  void updateInstanceMatrices(std::vector<glm::mat4> &instanceMatrices);
  void rotate(float angle, glm::vec3 axis);

 private:
  void setupMeshAttributes();
};

class CubeMap {
 public:
  CubeMap(std::vector<std::string> &faces, bool flip = true);
  ~CubeMap();

  CubeMap(const CubeMap &) = delete;
  CubeMap &operator=(const CubeMap &) = delete;
  CubeMap(CubeMap &&other) noexcept;
  CubeMap &operator=(CubeMap &&other) noexcept;

  GLuint textureID = 0;
  std::unique_ptr<Mesh> cubeMesh;

 private:
  void reset();

  static void flipHorizontally(unsigned char *data, int width, int height, int channels);
  static void flipVertically(unsigned char *data, int width, int height, int channels);
};
}  // namespace gfx::geom
