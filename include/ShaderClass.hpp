#pragma once

class Shader {
 public:
  Shader(const char *vertShaderPath, const char *fragShaderPath);
  Shader(const char *vertShaderPath, const char *geomShaderPath, const char *fragShaderPath);
  ~Shader();

  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;
  Shader(Shader &&other) noexcept;
  Shader &operator=(Shader &&other) noexcept;

  GLuint PROGRAM_ID = 0;

  bool reload();
  void use();
  void del();

 protected:
  static std::string readFile(const char *filePath);

  static GLuint compileShader(const char *shaderSource, GLenum shaderType);

  static void checkCompileErrors(GLuint shader, const char *shaderPath);

  static void checkLinkErrors(GLuint program, const char *tag);

 private:
  static GLuint buildProgram(const char *vert, const char *geom, const char *frag);
  void reset();

  std::string vertPath_;
  std::string geomPath_;
  std::string fragPath_;
};
