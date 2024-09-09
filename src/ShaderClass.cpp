#include "ShaderClass.hpp"

#include <OPPCH.h>

Shader::Shader(const char *vertShaderPath, const char *fragShaderPath) {
  vertPath = vertShaderPath;
  geomPath = nullptr;
  fragPath = fragShaderPath;
  std::vector<GLenum> shaderType = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
  std::vector<const char *> shaderPath({vertPath, fragPath});

  for (int i = 0; i < shaderType.size(); i++) {
    GLuint shader = loadShader(shaderPath[i], shaderType[i]);
    shaders.push_back(shader);
  }

  // Link shaders
  ID = glCreateProgram();
  for (int i = 0; i < shaders.size(); i++) {
    glAttachShader(ID, shaders[i]);
  }
  glLinkProgram(ID);
  checkLinkErrors(ID, "PROGRAM");
  // Delete the shaders as they're linked into our program now and no longer necessary
  for (int i = 0; i < shaders.size(); i++) {
    glDeleteShader(shaders[i]);
  }
}

Shader::Shader(const char *vertShaderPath, const char *geomShaderPath, const char *fragShaderPath) {
  vertPath = vertShaderPath;
  geomPath = geomShaderPath;
  fragPath = fragShaderPath;
  std::vector<GLenum> shaderType = {GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER};
  std::vector<const char *> shaderPath({vertPath, geomPath, fragPath});

  for (int i = 0; i < shaderType.size(); i++) {
    GLuint shader = loadShader(shaderPath[i], shaderType[i]);
    shaders.push_back(shader);
  }

  // Link shaders
  ID = glCreateProgram();
  for (int i = 0; i < shaders.size(); i++) {
    glAttachShader(ID, shaders[i]);
  }
  glLinkProgram(ID);
  checkLinkErrors(ID, "PROGRAM");
  for (int i = 0; i < shaders.size(); i++) {
    glDeleteShader(shaders[i]);
  }
}

GLuint Shader::loadShader(const char *path, GLenum shaderType) {
  std::string shaderCode = readFile(path);

  const char *shaderSource = shaderCode.c_str();
  GLuint shader = compileShader(shaderSource, shaderType);
  checkCompileErrors(shader, path);

  return shader;
}

void Shader::reload() {
  // clear the existing shaders
  for (int i = 0; i < shaders.size(); i++) {
    glDetachShader(ID, shaders[i]);
    glDeleteShader(shaders[i]);
  }
  shaders.clear();

  std::vector<GLenum> shaderType;
  std::vector<const char *> shaderPath;

  if (vertPath != nullptr) {
    shaderType.push_back(GL_VERTEX_SHADER);
    shaderPath.push_back(vertPath);
  }
  if (geomPath != nullptr) {
    std::cout << "Reloading geometry shader" << std::endl;
    shaderType.push_back(GL_GEOMETRY_SHADER);
    shaderPath.push_back(geomPath);
  }
  if (fragPath != nullptr) {
    shaderType.push_back(GL_FRAGMENT_SHADER);
    shaderPath.push_back(fragPath);
  }
  for (int i = 0; i < shaderType.size(); i++) {
    GLuint shader = loadShader(shaderPath[i], shaderType[i]);
    shaders.push_back(shader);
  }

  // Link shaders
  ID = glCreateProgram();
  for (int i = 0; i < shaders.size(); i++) {
    glAttachShader(ID, shaders[i]);
  }
  glLinkProgram(ID);
  checkLinkErrors(ID, "PROGRAM");
  for (int i = 0; i < shaders.size(); i++) {
    glDeleteShader(shaders[i]);
  }
}

void Shader::use() { glUseProgram(ID); }

void Shader::del() { glDeleteProgram(ID); }

std::string Shader::readFile(const char *filePath) {
  std::string content;
  std::ifstream fileStream(filePath, std::ios::in);
  if (!fileStream.is_open()) {
    std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
    return "";
  }

  std::string line;
  std::stringstream contentStream;
  while (std::getline(fileStream, line)) {
    if (line.substr(0, 8) == "#include") {
      // Extract the file name
      std::string includeFile = line.substr(9);
      includeFile = includeFile.substr(1, includeFile.length() - 2);  // Remove quotes

      // Recursively load the included file content
      std::string includeContent = readFile(includeFile.c_str());
      contentStream << includeContent;
    } else {
      contentStream << line << "\n";
    }
  }
  fileStream.close();

  return contentStream.str();
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

void Shader::checkLinkErrors(GLuint shader, std::string type) {
  GLint success;
  GLchar infoLog[512];
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::" << type << "::LINKING_FAILED\n" << infoLog << std::endl;
  }
}
