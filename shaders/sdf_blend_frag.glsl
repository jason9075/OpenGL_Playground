#version 330 core

out vec4 FragColor;

uniform float fov; // eg. 45.0 degrees
uniform vec2 resolution;

uniform vec3 sdfCenter;
uniform vec3 sdfColor = vec3(1.0);
uniform float sdfSize;

uniform vec3 lightPosition;
uniform vec3 lightColor = vec3(1.0);
uniform vec3 camPosition;

uniform mat4 viewMatrix;
uniform bool isSphere = false;

const float MAX_DIST = 500.0;
const float STEPS = 256;
const float EPSILON = 0.001;

float sdfSphere(vec3 p, vec3 center, float radius)
{
    return length(p - center) - radius;
}

float sdfCube(vec3 p, vec3 center, float size)
{
    vec3 d = abs(p - center) - vec3(size);
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

float sdfFunc(vec3 p, vec3 center, float size)
{
    if (isSphere)
    {
        return sdfSphere(p, center, size);
    }
    else
    {
        return sdfCube(p, center, size);
    }
}

// User tiny epsilon to calculate the gradient
vec3 getNormal(vec3 p)
{
    vec2 e = vec2(EPSILON, 0.0);
    vec3 n = vec3(sdfFunc(p, sdfCenter, sdfSize) - vec3(
        sdfFunc(p - e.xyy, sdfCenter, sdfSize), // gradient in x
        sdfFunc(p - e.yxy, sdfCenter, sdfSize), // gradient in y
        sdfFunc(p - e.yyx, sdfCenter, sdfSize)  // gradient in z
    ));
    return normalize(n);
}

vec3 getLight(vec3 lightSrc, vec3 p, vec3 rd, vec3 lightCol)
{
    vec3 l = normalize(lightSrc - p);       // light direction
    vec3 n = getNormal(p);                  // surface normal
    vec3 diffuse = lightCol * clamp(dot(l, n), 0.0, 1.0);
    vec3 ambient = vec3(0.2);
    return ambient + diffuse;

}

void main()
{
    vec2 uv = (2.0 * gl_FragCoord.xy - resolution) / resolution.y; // center origin point to the center of the screen
    vec3 rayOrigin = camPosition;
    // vec3 rayDirection = normalize(vec3(uv, 1.0 / tan(radians(fov) / 2.0)));

    mat3 newViewMatrix = mat3(viewMatrix);
    newViewMatrix[0][2] = -newViewMatrix[0][2];
    newViewMatrix[1][2] = -newViewMatrix[1][2];
    newViewMatrix[2][2] = -newViewMatrix[2][2];
    newViewMatrix = inverse(newViewMatrix);
    vec3 rayDirection = newViewMatrix * normalize(vec3(uv, 1.0 / tan(radians(fov) / 2.0)));

    // Ray marching

    float t = 0.0;
    for (int i = 0; i < STEPS; i++)
    {

        float d = sdfFunc(rayOrigin + rayDirection * t, sdfCenter, sdfSize);
        if (d < EPSILON) // Hit
        {
            break;
        }
        t += d; // Move along the ray
        if (t > MAX_DIST) // out of bounds
        {
            break;
        }
    }

    if (t < MAX_DIST) // Hit
    {
        vec3 light = getLight(lightPosition, rayOrigin + rayDirection * t, rayDirection, lightColor);
        FragColor = vec4(sdfColor *light, 1.0);
    }
    else // Miss
    {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}
