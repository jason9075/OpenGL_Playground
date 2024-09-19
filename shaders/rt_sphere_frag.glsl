#version 330 core

out vec4 fragColor;

uniform float fov;  // eg. 45.0 degrees
uniform uint frameIdx = 1u;
uniform vec2 resolution;
uniform vec3 camPosition;
uniform mat4 viewMatrix;

// Ray tracing settings
uniform int numBounces = 2;
uniform int numRays = 1;
uniform bool isSpecularBounce = false;
uniform bool isSpecularWhite = false;
uniform float ambientLight = 0.0f;

// Sphere data
/*
MAX_SPHERES is const variable, if you want to dynamically change the number of spheres,
You can use SSBO (Shader Storage Buffer Object) instead of UBO (Uniform Buffer Object),
But to use SSBO, you need to use OpenGL 4.3 or higher. So we use constant here.
*/
const int MAX_SPHERES = 50;
uniform int numSpheres = 2;
uniform bool showSphereLight = true;  // index 0 is the light source

// Triangle data
const int MAX_TRIANGLES = 24;  // 6 Plane (12) + 1 Light Cuboid (12)
uniform bool showCornellPlanes = true;
uniform bool showCornellLight = true;

struct Material {             // 48 bytes
  vec4 color;                 // 16 bytes
  vec4 emissionColor;         // 16 bytes
  float shininess;            // 4 bytes
  float smoothness;           // 4 bytes
  float specularProbability;  // 4 bytes
  float padding;              // 4 bytes
};

struct Sphere {
  vec3 center;        // 12 bytes
  float radius;       // 4 bytes
  Material material;  // 48 bytes
};

struct Triangle {
  vec3 posA;       // 12 bytes
  float padding1;  // 4 bytes
  vec3 posB;       // 12 bytes
  float padding2;  // 4 bytes
  vec3 posC;       // 12 bytes
  float padding3;  // 4 bytes
  vec3 normal;     // 12 bytes
  float padding4;  // 4 bytes
  // Material properties
  Material material;  // 48 bytes
};

struct HitInfo {
  bool didHit;
  float dst;
  vec3 hitPos;
  vec3 normal;
  Material material;
};

/*
  Uniform Buffer Object
  Note: The layout(std140) is used to ensure that the data is aligned correctly
  You have to align the data yourself, and the alignment is 16 bytes
*/
layout(std140) uniform sphereData { Sphere sphereList[MAX_SPHERES]; };
layout(std140) uniform triangleData { Triangle triangleList[MAX_TRIANGLES]; };

/*
  Copy from
  github.com/SebLague/Ray-Tracing/blob/main/Assets/Scripts/Shaders/RayTracer.shader
*/
float rand(inout uint state) {
  state = (frameIdx * 719413u + state) * 747796405u + 2891336453u;
  uint result = ((state >> ((state >> 28) + 4u)) ^ state) * 277803737u;
  result = (result >> 22) ^ result;
  return result / 4294967295.0;  // 2^32 - 1;
}

// If the light hits the surface, return the direction of the reflected ray
vec3 randomDirOnSurface(inout uint state, vec3 normal) {
  for (int i = 0; i < 100; i++) {
    float x = rand(state) * 2.0 - 1.0;
    float y = rand(state) * 2.0 - 1.0;
    float z = rand(state) * 2.0 - 1.0;
    vec3 dir = normalize(vec3(x, y, z));

    // If the direction is under the surface, multiply it by -1
    return dir * sign(dot(dir, normal));
  }
  return vec3(0);
}

vec3 randomDir(inout uint state) {
  float x = rand(state) * 2.0 - 1.0;
  float y = rand(state) * 2.0 - 1.0;
  float z = rand(state) * 2.0 - 1.0;
  return normalize(vec3(x, y, z));
}

HitInfo raySphere(vec3 ro, vec3 rd, vec3 sphereCenter, float sphereRadius) {
  HitInfo hitInfo;
  hitInfo.didHit = false;
  hitInfo.dst = 0.0f;
  hitInfo.hitPos = vec3(0.0f);
  hitInfo.normal = vec3(0.0f);
  hitInfo.material.color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

  vec3 oc = ro - sphereCenter;
  float a = dot(rd, rd);
  float b = 2.0f * dot(oc, rd);
  float c = dot(oc, oc) - sphereRadius * sphereRadius;
  float discriminant = b * b - 4.0f * a * c;

  if (discriminant > 0.0f) {
    float dist = (-b - sqrt(discriminant)) / (2.0f * a);
    if (dist > 0.0f) {
      hitInfo.didHit = true;
      hitInfo.dst = dist;
      hitInfo.hitPos = ro + rd * dist;
      hitInfo.normal = (hitInfo.hitPos - sphereCenter) / sphereRadius;
    }
  }

  return hitInfo;
}

// Möller–Trumbore algorithm
HitInfo rayTriangle(vec3 ro, vec3 rd, Triangle triangle) {
  HitInfo hitInfo;
  hitInfo.didHit = false;
  hitInfo.dst = 0.0f;
  hitInfo.hitPos = vec3(0.0f);
  hitInfo.normal = vec3(0.0f);
  hitInfo.material.color = vec4(0.0f, 0.0f, 0.0f, 1.0f);  // black

  vec3 edge1 = triangle.posB - triangle.posA;
  vec3 edge2 = triangle.posC - triangle.posA;
  vec3 h = cross(rd, edge2);
  float a = dot(edge1, h);
  // Check if the ray is parallel to the triangle
  if (a > -0.00001f && a < 0.00001f) {
    return hitInfo;
  }

  // Backface culling: check if the ray is hitting the back of the triangle
  vec3 normal = triangle.normal;
  if (dot(rd, normal) > 0.0f) {
    return hitInfo;  // ray is coming from the back
  }

  float f = 1.0f / a;
  vec3 s = ro - triangle.posA;
  float u = f * dot(s, h);
  if (u < 0.0f || u > 1.0f) {
    return hitInfo;
  }

  vec3 q = cross(s, edge1);
  float v = f * dot(rd, q);
  if (v < 0.0f || u + v > 1.0f) {
    return hitInfo;
  }

  float t = f * dot(edge2, q);
  if (t > 0.00001f) {
    hitInfo.didHit = true;
    hitInfo.dst = t;
    hitInfo.hitPos = ro + rd * t;
    hitInfo.normal = triangle.normal;
  }

  return hitInfo;
}

HitInfo calcClosestHit(vec3 ro, vec3 rd) {
  HitInfo closestHit;
  closestHit.didHit = false;
  closestHit.dst = 1000000.0f;

  // Sphere intersection
  int index = showSphereLight ? 0 : 1;
  for (int i = index; i < numSpheres; i++) {
    Sphere sphere = sphereList[i];
    HitInfo hitInfo = raySphere(ro, rd, sphere.center, sphere.radius);
    if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
      closestHit = hitInfo;
      closestHit.material = sphere.material;
    }
  }

  // Triangle intersection
  int startIndex = showCornellPlanes ? 0 : 12;
  int endIndex = showCornellLight ? MAX_TRIANGLES : MAX_TRIANGLES - 12;
  for (int i = startIndex; i < endIndex; i++) {
    Triangle triangle = triangleList[i];
    HitInfo hitInfo = rayTriangle(ro, rd, triangle);
    if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
      closestHit = hitInfo;
      closestHit.material = triangle.material;
    }
  }

  return closestHit;
}

vec3 trace(vec3 ro, vec3 rd, inout uint state) {
  vec4 incomingLight = vec4(0.0f);
  vec4 rayColor = vec4(1.0f);
  for (int i = 0; i < numBounces; i++) {
    HitInfo hitInfo = calcClosestHit(ro, rd);
    if (hitInfo.didHit) {
      Material material = hitInfo.material;
      // If the light hits the surface, return the direction of the reflected ray
      ro = hitInfo.hitPos;
      vec3 diffuseDir = normalize(hitInfo.normal + randomDir(state));
      vec3 specularDir = reflect(rd, hitInfo.normal);
      bool isSpecular = isSpecularBounce ? rand(state) < material.specularProbability : false;
      rd = mix(diffuseDir, specularDir, material.smoothness * float(isSpecular));

      vec4 emittedLight = material.emissionColor * (material.shininess + ambientLight);
      // add the color of the light
      incomingLight += rayColor * emittedLight;
      // absorb the color of the surface and prepared for the next bounce
      vec4 specularColor = isSpecularWhite ? vec4(1.0f) : material.color;
      rayColor *= mix(material.color, specularColor, float(isSpecular));
    } else {
      break;
    }
  }
  return incomingLight.rgb;
}

void main() {
  vec2 uv = (2.0 * gl_FragCoord.xy - resolution) / resolution.y;  // center origin point to the center of the screen
  uint state = uint(gl_FragCoord.x) + uint(gl_FragCoord.y) * uint(resolution.x);
  vec3 rayOrigin = camPosition;

  mat3 newViewMatrix = mat3(viewMatrix);
  newViewMatrix[0][2] = -newViewMatrix[0][2];
  newViewMatrix[1][2] = -newViewMatrix[1][2];
  newViewMatrix[2][2] = -newViewMatrix[2][2];
  newViewMatrix = inverse(newViewMatrix);
  vec3 rayDirection = newViewMatrix * normalize(vec3(uv, 1.0 / tan(radians(fov) / 2.0)));

  // split incoming light into 3 channels
  vec3 totalIncomingLight = vec3(0.0f);

  for (int i = 0; i < numRays; i++) {
    totalIncomingLight += trace(rayOrigin, rayDirection, state);
  }

  vec3 pixelColor = totalIncomingLight / float(numRays);
  fragColor = vec4(pixelColor, 1.0);
}
