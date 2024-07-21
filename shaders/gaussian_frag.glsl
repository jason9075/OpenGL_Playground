#version 330 core

in vec3 Color;
in vec2 XYValue;
in vec2 Pixf;

out vec4 FragColor;

void main()
{
    vec2 diff = XYValue - Pixf;
    float power = -0.5 * dot(diff, diff);

    // if (power > 0.0){
    //     discard;
    // }

    FragColor = vec4(Color, 1.0);
}
