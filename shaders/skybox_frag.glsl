#version 330 core

in vec3 texCoords;

out vec4 fragColor;

uniform samplerCube cubemapTxt;

void main()
{
    fragColor = texture(cubemapTxt, texCoords);
}
