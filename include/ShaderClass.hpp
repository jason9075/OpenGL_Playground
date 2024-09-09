#pragma once

class Shader {
 public:
  Shader(const char *vertShaderPath, const char *fragShaderPath);
  Shader(const char *vertShaderPath, const char *geomShaderPath, const char *fragShaderPath);
  GLuint ID;

  void reload();
  void use();
  void del();

 protected:
  std::string readFile(const char *filePath);

  GLuint compileShader(const char *shaderSource, GLenum shaderType);

  void checkCompileErrors(GLuint shader, const char *shaderPath);

  void checkLinkErrors(GLuint shader, std::string type);

 private:
  const char *vertPath;
  const char *geomPath;
  const char *fragPath;
  std::vector<GLuint> shaders;
  GLuint loadShader(const char *path, GLenum shaderType);
};
