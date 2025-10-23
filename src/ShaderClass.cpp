#include "ShaderClass.hpp"

#include <OPPCH.h>

Shader::Shader(const char *vertShaderPath, const char *fragShaderPath)
    : vertPath_(vertShaderPath ? vertShaderPath : ""), fragPath_(fragShaderPath ? fragShaderPath : "") {
  PROGRAM_ID = buildProgram(vertPath_.c_str(), nullptr, fragPath_.c_str());
}

Shader::Shader(const char *vertShaderPath, const char *geomShaderPath, const char *fragShaderPath)
    : vertPath_(vertShaderPath ? vertShaderPath : ""),
      geomPath_(geomShaderPath ? geomShaderPath : ""),
      fragPath_(fragShaderPath ? fragShaderPath : "") {
  PROGRAM_ID = buildProgram(vertPath_.c_str(), geomPath_.empty() ? nullptr : geomPath_.c_str(), fragPath_.c_str());
}

Shader::~Shader() { reset(); }

Shader::Shader(Shader &&other) noexcept
    : vertPath_(std::move(other.vertPath_)),
      geomPath_(std::move(other.geomPath_)),
      fragPath_(std::move(other.fragPath_)),
      PROGRAM_ID(other.PROGRAM_ID) {
  other.PROGRAM_ID = 0;
}

Shader &Shader::operator=(Shader &&other) noexcept {
  if (this != &other) {
    reset();
    vertPath_ = std::move(other.vertPath_);
    geomPath_ = std::move(other.geomPath_);
    fragPath_ = std::move(other.fragPath_);
    PROGRAM_ID = other.PROGRAM_ID;
    other.PROGRAM_ID = 0;
  }
  return *this;
}

bool Shader::reload() {
  GLuint newProg =
      buildProgram(vertPath_.empty() ? nullptr : vertPath_.c_str(), geomPath_.empty() ? nullptr : geomPath_.c_str(),
                   fragPath_.empty() ? nullptr : fragPath_.c_str());

  if (!newProg) return false;
  reset();
  PROGRAM_ID = newProg;
  return true;
}

void Shader::use() const { glUseProgram(PROGRAM_ID); }

std::string Shader::readFile(const char *filePath) {
  std::ifstream file(filePath, std::ios::in);
  if (!file.is_open()) {
    std::cerr << "Could not read file " << filePath << ". File does not exist.\n";
    return "";
  }
  std::stringstream ss;
  std::string line;
  while (std::getline(file, line)) {
    if (line.rfind("#include", 0) == 0) {
      // 形如: #include "path"
      auto left = line.find('"');
      auto right = line.find_last_of('"');
      if (left != std::string::npos && right != std::string::npos && right > left) {
        std::string includeFile = line.substr(left + 1, right - left - 1);
        ss << readFile(includeFile.c_str());
        ss << "\n";
      }
    } else {
      ss << line << "\n";
    }
  }
  return ss.str();
}

GLuint Shader::compileShader(const char *shaderSource, GLenum shaderType) {
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);
  return shader;
}

void Shader::checkCompileErrors(GLuint shader, const char *shaderPath) {
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::" << shaderPath << "::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
}

void Shader::checkLinkErrors(GLuint program, const char *tag) {
  GLint success = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar infoLog[2048];
    glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
    std::cerr << "ERROR::SHADER::" << tag << "::LINKING_FAILED\n" << infoLog << "\n";
  }
}

GLuint Shader::buildProgram(const char *vert, const char *geom, const char *frag) {
  std::vector<GLuint> shaders;
  shaders.reserve(3);

  auto attach = [&](const char *path, GLenum type) {
    if (!path) return;
    std::string code = readFile(path);
    GLuint sh = compileShader(code.c_str(), type);
    checkCompileErrors(sh, path);
    shaders.push_back(sh);
  };

  attach(vert, GL_VERTEX_SHADER);
  attach(geom, GL_GEOMETRY_SHADER);
  attach(frag, GL_FRAGMENT_SHADER);

  GLuint prog = glCreateProgram();
  for (auto sh : shaders) glAttachShader(prog, sh);
  glLinkProgram(prog);
  checkLinkErrors(prog, "PROGRAM");

  // 無論成功與否都刪 shader 物件
  for (auto sh : shaders) {
    glDetachShader(prog, sh);
    glDeleteShader(sh);
  }

  // 連結失敗則刪 program 並回 0
  GLint linked = GL_FALSE;
  glGetProgramiv(prog, GL_LINK_STATUS, &linked);
  if (!linked) {
    glDeleteProgram(prog);
    return 0;
  }
  return prog;
}

void Shader::reset() {
  if (PROGRAM_ID) {
    glDeleteProgram(PROGRAM_ID);
    PROGRAM_ID = 0;
  }
}
