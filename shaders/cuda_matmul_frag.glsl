#version 330 core

in vec2 texCoord;
in vec3 normal;
in vec3 color;

out vec4 fragColor;

void main() { fragColor = vec4(color, 1.0f); }
