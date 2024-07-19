#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 camMatrix;
uniform float geoRadius; 

out vec2 TexCoord;

void main()
{

    mat4 model = mat4(1.0);

    vec4 pos = camMatrix * model * gl_in[0].gl_Position;

    float halfSize = geoRadius;

    // 左下角
    TexCoord = vec2(0.0, 0.0);
    gl_Position = pos + vec4(-halfSize, -halfSize, 0.0, 0.0);
    EmitVertex();

    // 左上角
    TexCoord = vec2(0.0, 1.0);
    gl_Position = pos + vec4(-halfSize, halfSize, 0.0, 0.0);
    EmitVertex();

    // 右下角
    TexCoord = vec2(1.0, 0.0);
    gl_Position = pos + vec4(halfSize, -halfSize, 0.0, 0.0);
    EmitVertex();

    // 右上角
    TexCoord = vec2(1.0, 1.0);
    gl_Position = pos + vec4(halfSize, halfSize, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
