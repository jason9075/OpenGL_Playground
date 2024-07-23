#version 330 core

in vec3 Color;
in vec2 XYValue;
in vec2 Pixf;
in vec4 con_o;
in float Opacity;
in float ScaleModif;

out vec4 FragColor;

void main()
{
    vec2 diff = XYValue - Pixf + vec2(5.0, 5.0);
    float power = -0.5 * (con_o.x * diff.x * diff.x + con_o.z * diff.y * diff.y) - con_o.y * diff.x * diff.y;

    power *= ScaleModif;

    // if (power > 0.0){
    //     discard;
    // }

    float alpha = min(.99f, Opacity * exp(power));

    FragColor = vec4(Color*alpha, alpha);
}
