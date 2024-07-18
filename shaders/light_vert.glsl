#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 camMatrix;
uniform mat4 modelMatrix;

void main()
{
    fragPos =  vec3(modelMatrix * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    texCoord = mat2(1.0, 0.0, 0.0, -1.0) * aTexCoord;
    gl_Position = camMatrix * modelMatrix * vec4(aPos, 1.0);
}
