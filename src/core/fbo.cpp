#include "core/fbo.hpp"

namespace gfx::core {
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
}  // namespace gfx::core
