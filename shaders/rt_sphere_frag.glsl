#version 330 core

out vec4 fragColor;

uniform float fov;  // eg. 45.0 degrees
uniform uint ticks = 1u;
uniform vec2 resolution;
uniform vec3 camPosition;
uniform mat4 viewMatrix;

const int MAX_SPHERES = 100;
uniform int numSpheres = 2;
uniform int numBounces = 2;
uniform int numRays = 1;
uniform bool isFirstSphereLight = true;
uniform bool isSpecularBounce = false;
uniform bool isSpecularWhite = false;

struct Sphere {
  vec3 center;
  float radius;
  // Material properties
  vec4 color;                 // 16 bytes
  vec4 emissionColor;         // 16 bytes
  float shininess;            // 4 bytes
  float smoothness;           // 4 bytes
  float specularProbability;  // 4 bytes
  float padding;              // 4 bytes
};
struct HitInfo {
  bool didHit;
  float dst;
  vec3 hitPos;
  vec3 normal;
  // Material properties
  vec4 color;
  vec4 emissionColor;
  float shininess;
  float smoothness;
  float specularProbability;
};

/*
  Uniform Buffer Object
  Note: The layout(std140) is used to ensure that the data is aligned correctly
  You have to align the data yourself, and the alignment is 16 bytes
*/
layout(std140) uniform sphereData { Sphere sphereList[MAX_SPHERES]; };

/*
  Copy from
  github.com/SebLague/Ray-Tracing/blob/main/Assets/Scripts/Shaders/RayTracer.shader
*/
float rand(inout uint state) {
  state = ticks * 719393u + state * 747796405u + 2891336453u;
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
  hitInfo.color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

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

HitInfo calcClosestHit(vec3 ro, vec3 rd) {
  HitInfo closestHit;
  closestHit.didHit = false;
  closestHit.dst = 1000000.0f;

  int index = isFirstSphereLight ? 0 : 1;
  for (int i = index; i < numSpheres; i++) {
    Sphere sphere = sphereList[i];
    // If the sphere is a light source, set the radius to 1
    if (i != 0 && 0 < sphere.shininess) {
      // sphere.emissionColor = vec4(1.0f);
      sphere.radius = 0.25f;
    }
    HitInfo hitInfo = raySphere(ro, rd, sphere.center, sphere.radius);
    if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
      closestHit = hitInfo;
      closestHit.color = sphere.color;
      closestHit.emissionColor = sphere.emissionColor;
      closestHit.shininess = sphere.shininess;
      closestHit.smoothness = sphere.smoothness;
      closestHit.specularProbability = sphere.specularProbability;
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
      // If the light hits the surface, return the direction of the reflected ray
      ro = hitInfo.hitPos;
      vec3 diffuseDir;
      diffuseDir = normalize(hitInfo.normal + randomDir(state));
      vec3 specularDir = reflect(rd, hitInfo.normal);
      bool isSpecular = isSpecularBounce ? rand(state) < hitInfo.specularProbability : false;
      rd = mix(diffuseDir, specularDir, hitInfo.smoothness * float(isSpecular));

      vec4 emittedLight = hitInfo.emissionColor * hitInfo.shininess;
      // add the color of the light
      incomingLight += rayColor * emittedLight;
      // absorb the color of the surface and prepared for the next bounce
      vec4 specularColor = isSpecularWhite ? vec4(1.0f) : hitInfo.color;
      rayColor *= mix(hitInfo.color, specularColor, float(isSpecular));
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
