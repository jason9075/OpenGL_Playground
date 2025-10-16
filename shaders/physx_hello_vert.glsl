#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

uniform mat4 modelMatrix;
uniform mat4 camMatrix;

void main() {
  FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
  Normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
  Color = aColor;
  gl_Position = camMatrix * modelMatrix * vec4(aPos, 1.0);
}
