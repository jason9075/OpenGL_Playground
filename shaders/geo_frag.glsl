#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 pointColor;
uniform float smoothValue;
uniform float mask;

void main()
{
    float dist = dot(TexCoord - vec2(0.5), TexCoord - vec2(0.5));
    if (dist > mask)
    {
        discard;
    }
    float alpha = 1.0 - smoothstep(0.0, smoothValue, dist);
    FragColor = vec4(pointColor, alpha);
}
