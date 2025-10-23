#pragma once

#include "geom/mesh.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class Model {
 public:
  Model(const char *path);
  Model();
  ~Model() = default;
  Model(const Model &) = delete;
  Model &operator=(const Model &) = delete;
  Model(Model &&) noexcept = default;
  Model &operator=(Model &&) noexcept = default;

  glm::mat4 modelMatrix;
  std::vector<gfx::geom::Mesh> meshes;  // one model can have multiple meshes

  // void draw(Shader *shader, const unsigned int instanceCount = 1);
  //
  // void drawTri(Shader *shader, const unsigned int numTriangles, const unsigned int instanceCount = 1);

  void setModelMatrix(glm::mat4 matrix);

 private:
  const char *path;
  json JSON;
  std::vector<unsigned char> data;  // binary data

  std::vector<std::string> loadedTexturesName;
  std::vector<std::shared_ptr<gfx::resource::Texture>> loadedTextures;

  void loadMesh(unsigned int indMesh, glm::mat4 matrix);

  void traverseNode(unsigned int nodeIndex, glm::mat4 identity = glm::mat4(1.0f));

  std::vector<unsigned char> getData();

  std::vector<float> getFloats(json accessor);
  std::vector<GLuint> getIndices(json accessor);
  std::vector<std::shared_ptr<gfx::resource::Texture>> getTextures();

  std::vector<gfx::geom::Vertex> assembleVertices(const std::vector<glm::vec3> &positions,
                                                  const std::vector<glm::vec3> &normals,
                                                  const std::vector<glm::vec2> &texCoords);

  std::vector<glm::vec2> groupFloatsVec2(const std::vector<float> &floats);
  std::vector<glm::vec3> groupFloatsVec3(const std::vector<float> &floats);
  std::vector<glm::vec4> groupFloatsVec4(const std::vector<float> &floats);
};
