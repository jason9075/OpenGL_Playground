#version 430 core

in vec3 texCoords;

out vec4 fragColor;

uniform samplerCube cubemap;

void main()
{
    fragColor = texture(cubemap, texCoords);
}
