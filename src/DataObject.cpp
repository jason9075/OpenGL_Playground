#include "DataObject.h"

#include <OPPCH.h>

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

void VAO::bind() { glBindVertexArray(ID); }

void VAO::unbind() { glBindVertexArray(0); }

void VAO::del() { glDeleteVertexArrays(1, &ID); }

template <typename T>
VBO::VBO(const std::vector<T> &data) {
  glGenBuffers(1, &ID);
  glBindBuffer(GL_ARRAY_BUFFER, ID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), data.data(), GL_STATIC_DRAW);
}

void VBO::bind() { glBindBuffer(GL_ARRAY_BUFFER, ID); }

void VBO::unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void VBO::del() { glDeleteBuffers(1, &ID); }

EBO::EBO(const std::vector<GLuint> &indices) {
  glGenBuffers(1, &ID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

void EBO::bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }

void EBO::unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

void EBO::del() { glDeleteBuffers(1, &ID); }

Texture::Texture(const char *image, const char *texType, GLuint slot) : type(texType) {
  int width, height, nrChannels;

  stbi_set_flip_vertically_on_load(true);
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

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices,
           const std::vector<Texture> &textures)
    : vertices(vertices), indices(indices), textures(textures), vao(), vbo(vertices), ebo(indices) {
  vao.bind();
  vbo.bind();
  ebo.bind();

  vao.linkAttr(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, position));
  vao.linkAttr(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, normal));
  vao.linkAttr(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, color));
  vao.linkAttr(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));

  vao.unbind();
  vbo.unbind();
  ebo.unbind();
}

void Mesh::draw(Shader *shader) {
  vao.bind();

  // Textures
  for (unsigned int i = 0; i < textures.size(); i++) {
    textures[i].texUnit(*shader, "texture0", i);
    textures[i].bind();
  }

  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

  vao.unbind();
  for (unsigned int i = 0; i < textures.size(); i++) {
    textures[i].unbind();
  }
}

void Mesh::del() {
  vao.del();
  vbo.del();
  ebo.del();
}

CubeMap::CubeMap(const std::vector<std::string> &faces) {
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
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

  float cubeMapPos[108] = {
      // Front face
      -1.0f, 1.0f, -1.0f,   // Top-left
      -1.0f, -1.0f, -1.0f,  // Bottom-left
      1.0f, -1.0f, -1.0f,   // Bottom-right
      1.0f, -1.0f, -1.0f,   // Bottom-right
      1.0f, 1.0f, -1.0f,    // Top-right
      -1.0f, 1.0f, -1.0f,   // Top-left

      // Back face
      -1.0f, -1.0f, 1.0f,  // Bottom-left
      -1.0f, 1.0f, 1.0f,   // Top-left
      1.0f, 1.0f, 1.0f,    // Top-right
      1.0f, 1.0f, 1.0f,    // Top-right
      1.0f, -1.0f, 1.0f,   // Bottom-right
      -1.0f, -1.0f, 1.0f,  // Bottom-left

      // Left face
      -1.0f, 1.0f, 1.0f,    // Top-left
      -1.0f, 1.0f, -1.0f,   // Top-right
      -1.0f, -1.0f, -1.0f,  // Bottom-right
      -1.0f, -1.0f, -1.0f,  // Bottom-right
      -1.0f, -1.0f, 1.0f,   // Bottom-left
      -1.0f, 1.0f, 1.0f,    // Top-left

      // Right face
      1.0f, 1.0f, 1.0f,    // Top-left
      1.0f, -1.0f, -1.0f,  // Bottom-right
      1.0f, 1.0f, -1.0f,   // Top-right
      1.0f, -1.0f, -1.0f,  // Bottom-right
      1.0f, 1.0f, 1.0f,    // Top-left
      1.0f, -1.0f, 1.0f,   // Bottom-left

      // Top face
      -1.0f, 1.0f, -1.0f,  // Top-left
      1.0f, 1.0f, -1.0f,   // Top-right
      1.0f, 1.0f, 1.0f,    // Bottom-right
      1.0f, 1.0f, 1.0f,    // Bottom-right
      -1.0f, 1.0f, 1.0f,   // Bottom-left
      -1.0f, 1.0f, -1.0f,  // Top-left

      // Bottom face
      -1.0f, -1.0f, -1.0f,  // Top-left
      -1.0f, -1.0f, 1.0f,   // Bottom-left
      1.0f, -1.0f, -1.0f,   // Top-right
      1.0f, -1.0f, -1.0f,   // Top-right
      -1.0f, -1.0f, 1.0f,   // Bottom-left
      1.0f, -1.0f, 1.0f     // Bottom-right
  };

  vao = VAO();
  std::vector<Vertex> vertices;
  for (unsigned int i = 0; i < 108; i += 3) {
    Vertex vertex;
    vertex.position = glm::vec3(cubeMapPos[i], cubeMapPos[i + 1], cubeMapPos[i + 2]);
    vertices.push_back(vertex);
  }
  VBO vbo = VBO(vertices);

  vao.bind();
  vao.linkAttr(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
  vao.unbind();
}

void CubeMap::draw(Shader *shader) {
  vao.bind();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  glUniform1i(glGetUniformLocation(shader->ID, "cubemap"), 0);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  vao.unbind();
}

void CubeMap::del() {
  vao.del();
  glDeleteTextures(1, &textureID);
}

PointCloud::PointCloud(const std::vector<Point> &points) : points(points), vao(), vbo(points) {
  vao.bind();
  vbo.bind();

  vao.linkAttr(vbo, 0, 3, GL_FLOAT, sizeof(Point), (void *)offsetof(Point, position));
  vao.linkAttr(vbo, 1, 3, GL_FLOAT, sizeof(Point), (void *)offsetof(Point, color));
  vao.linkAttr(vbo, 2, 3, GL_FLOAT, sizeof(Point), (void *)offsetof(Point, scale));
  vao.linkAttr(vbo, 3, 4, GL_FLOAT, sizeof(Point), (void *)offsetof(Point, rotation));

  vao.unbind();
  vbo.unbind();
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
