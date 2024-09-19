#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in mat4 aInstanceMatrix;

out vec2 texCoord;

uniform mat4 camMatrix;

void main() {
  texCoord = aTexCoord;
  gl_Position = camMatrix * aInstanceMatrix * vec4(aPos, 1.0);
}
