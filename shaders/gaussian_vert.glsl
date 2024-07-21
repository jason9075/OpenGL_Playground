#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in float aOpacity;
layout(location = 3) in vec3 aCovA;
layout(location = 4) in vec3 aCovB;

uniform mat4 camMatrix;

out vec3 Color;
out vec2 XYValue;
out vec2 Pixf;

vec3 computeCov2D(vec3 mean, float[6] cov3DV, mat4 viewMatrix)
{
    vec4 t = viewMatrix * vec4(mean, 1.0);
    mat3 W = mat3(viewMatrix);
    mat3 Vrk = mat3(
        cov3DV[0], cov3DV[1], cov3DV[2],
        cov3DV[1], cov3DV[3], cov3DV[4],
        cov3DV[2], cov3DV[4], cov3DV[5]
    );

    mat3 cov = W * Vrk * transpose(W);
    cov[0][0] += .3;
    cov[1][1] += .3;
    return vec3(cov[0][0], cov[0][1], cov[1][1]);

}

float ndc2Pix(float v, float S) {
    return ((v + 1.) * S - 1.) * .5;
}

void main()
{
    vec4 p4 = camMatrix * vec4(aPos, 1.0);
    float pw = 1.0 / (p4.w + 1e-7);
    vec3 p_proj = p4.xyz * pw;

    float cov3D[6] = float[6](aCovA.x, aCovA.y, aCovA.z, aCovB.x, aCovB.y, aCovB.z);

    vec3 cov2D = computeCov2D(aPos, cov3D, camMatrix);

    float det = cov2D.x * cov2D.z - cov2D.y * cov2D.y;
    // if (det < 1e-7)
    // {
    //     discard;
    // }
    float det_inv = 1.0 / det;
    vec3 inv_cov2D = vec3(cov2D.z, -cov2D.y, cov2D.x) * det_inv;

    float mid = 0.5 * (cov2D.x + cov2D.z);
    float lambda1  = mid + sqrt(max(0.1, mid * mid - det));
    float lambda2  = mid - sqrt(max(0.1, mid * mid - det));
    float radius = ceil(3. * sqrt(max(lambda1, lambda2)))*10;
    vec2 point_image = vec2(ndc2Pix(p_proj.x, 1024), ndc2Pix(p_proj.y, 768));

    vec2 corner = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2) - 1.;

    vec2 screen_pos = point_image + corner * radius;


    Color = aColor;
    XYValue = point_image;
    Pixf = screen_pos;
    vec2 clipPos = screen_pos / vec2(1024, 768) * 2.0 - 1.0;
    gl_Position = vec4(clipPos, 0.0, 1.0);
}
