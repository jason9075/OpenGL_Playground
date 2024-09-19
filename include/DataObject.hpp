#pragma once

#include "ShaderClass.hpp"

class VBO;

class VAO {
 public:
  VAO();
  void linkAttr(VBO &VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, const void *offset);
  void linkAttrDiv(VBO &VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, const void *offset);
  void linkMat4(VBO &VBO, GLuint layout);
  void bind();
  void unbind();
  void del();
  GLuint ID;

 private:
};

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
  glm::vec2 texCoords;
  glm::mat4 modelMatrix;
};

struct Point {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 scale;
  glm::vec4 rotation;
};

struct GaussianSphere {
  glm::vec3 position;
  glm::vec3 color;
  float opacity;
  glm::vec3 covA;
  glm::vec3 covB;
};

class VBO {
 public:
  template <typename T>
  VBO(const std::vector<T> &data);
  VBO();
  void bind();
  void unbind();
  void del();

 private:
  GLuint ID;
};

class EBO {
 public:
  EBO();
  void bind();
  void bufferData(const std::vector<GLuint> &indices);
  void del();

 private:
  GLuint ID;
};

class UBO {
 public:
  GLuint ID;
  UBO();
  void bind();
  void unbind();
  template <typename T>
  void bufferData(T *data, size_t count, GLenum usage) {
    glBufferData(GL_UNIFORM_BUFFER, sizeof(T) * count, data, usage);
  }
  template <typename T>
  void bufferSubData(const T *data, size_t count) {
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T) * count, data);
  }
  void del();

 private:
};

class FBO {
 public:
  GLuint ID;
  GLuint textureID;
  FBO(float width, float height);
  void bind();
  void unbind();
  void setupTexture();
  void bindTexture(GLenum textureUnit);
  void del();
  float width;
  float height;
};

class Texture {
 public:
  const char *type;
  Texture();
  Texture(const char *image, const char *texType, GLuint slot);
  void texUnit(Shader &shader, const char *uniform, GLuint unit);
  void bind();
  void unbind();
  void del();

 private:
  GLuint ID;
  GLuint unit;
};

class Mesh {
 public:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;

  VAO vao;
  VBO vbo;
  VBO instanceMatrixVBO;
  EBO ebo;
  std::vector<UBO> ubo;

  Mesh(const std::vector<Vertex> &vertices);
  Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);
  Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures);

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
  void setTexture(const std::vector<Texture> &textures);
  void setupInstanceMatrices(std::vector<glm::mat4> &instanceMatrices);
  void updateInstanceMatrices(std::vector<glm::mat4> &instanceMatrices);

  void draw(Shader *shader, const unsigned int instanceCount = 1);
  void drawTri(Shader *shader, const unsigned int numVertices, const unsigned int startIdx = 0);

  void del();

 private:
  void setupMeshAttributes();
};

class CubeMap {
 public:
  VAO vao;
  CubeMap(std::vector<std::string> &faces, bool flip = true);

  void flipHorizontally(unsigned char *data, int width, int height, int channels);
  void flipVertically(unsigned char *data, int width, int height, int channels);

  void draw(Shader *shader);

  void del();

 private:
  std::unique_ptr<Mesh> cubeMesh;
  GLuint textureID;
};

class PointCloud {
 public:
  std::vector<Point> points;

  VAO vao;
  VBO vbo;

  void draw(Shader *shader);

  void del();

  PointCloud(const std::vector<Point> &points);
};

class GaussianSplat {
 public:
  VAO vao;
  VBO vbo;
  std::vector<GaussianSphere> spheres;

  GaussianSplat(const std::vector<GaussianSphere> &spheres);

  void sort(const glm::mat4 &viewMatrix, const bool isAscending = true);
  void draw(Shader *shader);

  void del();
};
