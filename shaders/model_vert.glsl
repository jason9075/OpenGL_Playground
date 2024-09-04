#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoord;

out vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 camMatrix;

void main() {
  texCoord = aTexCoord;
  gl_Position = camMatrix * modelMatrix * vec4(aPos, 1.0);
}
