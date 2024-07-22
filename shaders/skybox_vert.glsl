#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform float scaleFactor;
uniform mat4 camMatrix;

void main()
{
    texCoords = aPos;
    vec4 pos = camMatrix * vec4(aPos * scaleFactor, 1.0);
    gl_Position = pos; // make sure the skybox is always rendered behind everything else
}
