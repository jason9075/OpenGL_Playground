#pragma once

#include "ShaderClass.hpp"

class VBO;

class VAO {
 public:
  VAO();
  ~VAO() { reset(); };

  VAO(const VAO &) = delete;
  VAO &operator=(const VAO &) = delete;

  VAO(VAO &&other) noexcept;
  VAO &operator=(VAO &&other) noexcept;

  void linkAttr(VBO &vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, const void *offset);
  void linkAttrDiv(VBO &vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, const void *offset);
  void linkMat4(VBO &vbo, GLuint layout);
  void bind() const;
  void unbind() const;
  GLuint ID;

 private:
  void reset();
};

class VBO {
 public:
  VBO();

  template <typename T>
  VBO(const std::vector<T> &data) {
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), data.data(), GL_STATIC_DRAW);
  }
  ~VBO() { reset(); }

  VBO(const VBO &) = delete;
  VBO &operator=(const VBO &) = delete;
  VBO(VBO &&o) noexcept;
  VBO &operator=(VBO &&o) noexcept;

  template <typename T>
  void bufferData(const std::vector<T> &data) {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), data.data(), GL_STATIC_DRAW);
  }
  void bind() const;
  void unbind() const;

 private:
  void reset();
  GLuint ID;
};

class EBO {
 public:
  EBO();
  ~EBO() { reset(); }
  EBO(const EBO &) = delete;
  EBO &operator=(const EBO &) = delete;
  EBO(EBO &&o) noexcept;
  EBO &operator=(EBO &&o) noexcept;

  void bind() const;
  void bufferData(const std::vector<GLuint> &indices);

 private:
  void reset();
  GLuint ID;
};

class UBO {
 public:
  GLuint ID;
  UBO();
  ~UBO() { reset(); }
  UBO(const UBO &) = delete;
  UBO &operator=(const UBO &) = delete;
  UBO(UBO &&o) noexcept;
  UBO &operator=(UBO &&o) noexcept;

  void bind() const;
  void unbind() const;
  template <typename T>
  void bufferData(T *data, size_t count, GLenum usage) {
    glBufferData(GL_UNIFORM_BUFFER, sizeof(T) * count, data, usage);
  }
  template <typename T>
  void bufferSubData(const T *data, size_t count) {
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T) * count, data);
  }

 private:
  void reset();
};

class FBO {
 public:
  GLuint ID;
  GLuint textureID;
  FBO(float width, float height);
  ~FBO() { reset(); }
  FBO(const FBO &) = delete;
  FBO &operator=(const FBO &) = delete;
  FBO(FBO &&o) noexcept;
  FBO &operator=(FBO &&o) noexcept;
  void bind() const;
  void unbind() const;
  void setupTexture();
  void bindTexture(GLenum textureUnit);
  float width;
  float height;

 private:
  void reset();
};

class Texture {
 public:
  std::string type;
  Texture();
  Texture(const char *image, const char *texType, GLuint slot);
  ~Texture() { reset(); }
  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;
  Texture(Texture &&other) noexcept;
  Texture &operator=(Texture &&other) noexcept;

  void texUnit(Shader &shader, const char *uniform, GLuint unit);
  void bind() const;
  void unbind() const;

 private:
  void reset();
  GLuint ID;
  GLuint unit;
};

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
  std::vector<std::shared_ptr<Texture>> textures;

  VAO vao;
  VBO vbo;
  VBO instanceMatrixVBO;
  EBO ebo;
  std::vector<UBO> ubo;

  Mesh(const std::vector<Vertex> &vertices);
  Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);
  Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices,
       std::vector<std::shared_ptr<Texture>> textures);

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
  void setTexture(std::vector<std::shared_ptr<Texture>> textures);
  bool hasTexture() const;
  void setupInstanceMatrices(std::vector<glm::mat4> &instanceMatrices);
  void updateInstanceMatrices(std::vector<glm::mat4> &instanceMatrices);
  void rotate(float angle, glm::vec3 axis);

  void draw(Shader *shader, const unsigned int instanceCount = 1);
  void drawTri(Shader *shader, const unsigned int numVertices, const unsigned int startIdx = 0);

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

  void draw(Shader *shader);

 private:
  void reset();
  std::unique_ptr<Mesh> cubeMesh;
  GLuint textureID = 0;

  static void flipHorizontally(unsigned char *data, int width, int height, int channels);
  static void flipVertically(unsigned char *data, int width, int height, int channels);
};

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

  void draw(Shader *shader) const;

 private:
  std::vector<Point> points;
  VAO vao;
  VBO vbo;
};

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

  void sort(const glm::mat4 &viewMatrix, const bool isAscending = true);
  void draw(Shader *shader) const;

 private:
  std::vector<GaussianSphere> spheres;
  VAO vao;
  VBO vbo;
};
