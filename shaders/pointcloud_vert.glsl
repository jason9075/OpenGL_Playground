#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 color;

uniform mat4 modelMatrix;
uniform mat4 camMatrix;

void main() {
    color = aColor;
    gl_Position = camMatrix * modelMatrix * vec4(aPos, 1.0);
    // set point size by camera distance
    float distance = length(gl_Position.xyz);
    float size = exp(-distance * 0.3) * 5.0;
    gl_PointSize = size;
}
