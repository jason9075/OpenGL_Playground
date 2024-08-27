#version 330 core
#include "shaders/hg_sdf.glsl"

out vec4 FragColor;

uniform float fov;  // eg. 45.0 degrees
uniform vec2 resolution;

uniform vec3 backgroundColor = vec3(0.4, 0.6, 0.95);  // sky blue
uniform sampler2D texture0;
uniform sampler2D texture1;

uniform vec3 sdfCenter;
uniform vec3 sdfColor = vec3(1.0);
uniform float sdfSize;
uniform float bumpFactor = 0.1;

uniform vec3 lightPosition;
uniform vec3 lightColor = vec3(1.0);
uniform vec3 camPosition;

uniform mat4 viewMatrix;
uniform bool isSphere = false;

const float MAX_DIST = 100.0;
const float STEPS = 128;
const float EPSILON = 0.001;

// Taipei 101 constant
const float BASE_HEIGHT = 2.8;
const float BASE_UP_WIDTH = 0.6;
const float PYRAMID_HEIGHT = 0.8;
const float TOP_HEIGHT = BASE_HEIGHT + 8.0 * PYRAMID_HEIGHT;
const float HALF_PI = PI / 2.0;

vec2 fOpUnion(vec2 res1, vec2 res2) { return (res1.x < res2.x) ? res1 : res2; }

float fOpDifferece(float d1, float d2) { return max(d1, -d2); }

vec3 triPlanar(sampler2D tex, vec3 p, vec3 normal) {
  normal = abs(normal);
  return (texture(tex, p.xy * 0.5 + 0.5) * normal.z + texture(tex, p.zy * 0.5 + 0.5) * normal.x).rgb;
}

float bumpMap(sampler2D tex, vec3 p, vec3 normal, float dist, float factor, float scale) {
  float bump = 0.0;
  if (dist < 0.1) {
    vec3 n = normal;
    bump = factor * triPlanar(tex, p * scale, n).r;
  }
  return bump;
}

// ref: https://www.shadertoy.com/view/Ws3SDl
float sdPyramid(in vec3 p, in float h, in float halfWidth) {
  // base
  if (p.y <= 0.0) return length(max(abs(p) - vec3(halfWidth, 0.0, halfWidth), 0.0));

  float m2 = h * h + halfWidth * halfWidth;

  // symmetry
  p.xz = abs(p.xz);  // do p=abs(p) instead for double pyramid
  p.xz = (p.z > p.x) ? p.zx : p.xz;
  p.xz -= halfWidth;

  // project into face plane (2D)
  vec3 q = vec3(p.z, h * p.y - halfWidth * p.x, h * p.x + halfWidth * p.y);

  float s = max(-q.x, 0.0);
  float t = clamp((q.y - halfWidth * q.x) / (m2 + halfWidth * halfWidth), 0.0, 1.0);

  float a = m2 * (q.x + s) * (q.x + s) + q.y * q.y;
  float b = m2 * (q.x + halfWidth * t) * (q.x + halfWidth * t) + (q.y - m2 * t) * (q.y - m2 * t);

  float d2 = max(-q.y, q.x * m2 + q.y * halfWidth) < 0.0 ? 0.0 : min(a, b);

  // recover 3D and scale, and add sign
  return sqrt((d2 + q.z * q.z) / m2) * sign(max(q.z, -p.y));
}

float fTruncatedPyramid(vec3 p, float halfBottomWidth, float halfUpWidth, float height) {
  // because sdPyramid use full height, so we need to adjust the height to half
  p.y += height / 2;

  // truncated the top part of the pyramid
  if (p.y > height) return length(max(abs(p) - vec3(halfUpWidth, height, halfUpWidth), 0.0));

  // p.y += height / 2;
  float ratio = halfUpWidth / halfBottomWidth;
  // calculate the height of the pyramid
  float h = ratio * height / (1 - ratio);
  // make the pyramid first
  float pyr = sdPyramid(p, height + h, halfBottomWidth);
  return pyr;
}

vec2 sdfFunc(vec3 p) {
  float planeID = 1.0;
  float windowsID = 2.0;
  float lightGrayID = 3.0;
  float darkGrayID = 4.0;
  // plane
  vec3 transP = p;
  float planeDist = fPlane(transP, vec3(0.0, 1.0, 0.0), 0.0);
  vec2 plane = vec2(planeDist, planeID);

  // taipei101
  transP = p;
  transP.y -= BASE_HEIGHT / 2;
  // transP.x -= 10;
  float baseDist = fTruncatedPyramid(transP, 1.0, BASE_UP_WIDTH, BASE_HEIGHT);
  baseDist += bumpMap(texture1, transP * 2, transP + bumpFactor, baseDist, bumpFactor, sdfSize);
  vec2 base = vec2(baseDist, windowsID);
  vec2 res = fOpUnion(base, plane);

  // cylinder
  pMirrorOctant(transP.xy, vec2(BASE_UP_WIDTH));  // maybe use later
  float grayID = 3.0;
  transP = p;
  transP.y -= BASE_HEIGHT;

  vec3 tPX = transP;
  pR(tPX.zy, HALF_PI);
  pMirror(tPX.y, BASE_UP_WIDTH);

  vec3 tPZ = transP;
  pR(tPZ.xy, HALF_PI);
  pMirror(tPZ.y, BASE_UP_WIDTH);

  float cylinderDist = fCylinder(tPX, 0.2, 0.1);
  res = fOpUnion(res, vec2(cylinderDist, grayID));

  cylinderDist = fCylinder(tPZ, 0.2, 0.1);
  res = fOpUnion(res, vec2(cylinderDist, grayID));

  transP = p;
  transP.y -= BASE_HEIGHT + PYRAMID_HEIGHT / 2;
  // upside down
  pR(transP.xy, PI);
  pModInterval1(transP.y, PYRAMID_HEIGHT, -7, 0);
  float pyrDist = fTruncatedPyramid(transP, 0.6, 0.5, PYRAMID_HEIGHT);
  vec2 pyr = vec2(pyrDist, windowsID);
  res = fOpUnion(res, pyr);

  // baseTop Decoration
  transP = p;
  transP.y -= BASE_HEIGHT;
  pMirrorOctant(transP.zx, vec2(BASE_UP_WIDTH));
  float baseTopDist = fBoxCheap(transP, vec3(BASE_UP_WIDTH, 0.15, 0.03));

  // baseTopDist += bumpMap(texture1, transP * 2, transP + bumpFactor, baseTopDist, bumpFactor, sdfSize);
  res = fOpUnion(res, vec2(baseTopDist, darkGrayID));

  // top building
  transP = p;
  transP.y -= TOP_HEIGHT;
  float topBuildingDist = fBoxCheap(transP, vec3(0.2, 0.4, 0.2));
  vec2 topBuilding = vec2(topBuildingDist, windowsID);
  res = fOpUnion(res, topBuilding);

  // top needle
  transP = p;
  transP.y -= TOP_HEIGHT;
  float boxDist = fBoxCheap(transP, vec3(0.05, 1.8, 0.05));
  vec2 needle = vec2(boxDist, darkGrayID);
  res = fOpUnion(res, needle);

  return res;
}

vec3 getMaterial(vec3 point, float id, vec3 normal) {
  switch (int(id)) {
    case 1:  // checkboard floor
      return vec3(0.2 + 0.4 * mod(floor(point.x) + floor(point.z), 2.0));
    case 2:
      return triPlanar(texture0, point * 2, normal);
    case 3:
      return vec3(0.75, 0.75, 0.75);  // light gray
    case 4:
      return vec3(0.3, 0.3, 0.3);  // dark gray
    default:
      return vec3(1.0, 1.0, 1.0);
  }
}

// User tiny epsilon to calculate the gradient
vec3 getNormal(vec3 p) {
  vec2 e = vec2(EPSILON, 0.0);
  vec3 n = vec3(sdfFunc(p).x - vec3(sdfFunc(p - e.xyy).x,  // gradient in x
                                    sdfFunc(p - e.yxy).x,  // gradient in y
                                    sdfFunc(p - e.yyx).x   // gradient in z
                                    ));
  return normalize(n);
}

vec2 rayMarch(vec3 rayOrigin, vec3 rayDirection) {
  float dist = 0.0;
  float hitClass = 0.0;

  for (int i = 0; i < STEPS; i++) {
    vec3 p = rayOrigin + rayDirection * dist;
    vec2 hitObj = sdfFunc(p);
    dist += hitObj.x;
    hitClass = hitObj.y;
    if (dist > MAX_DIST || hitObj.x < EPSILON) break;
  }
  return vec2(dist, hitClass);
}

vec3 getLight(vec3 lightSrc, vec3 p, vec3 n, vec3 lightCol, vec3 rayDir) {
  vec3 l = normalize(lightSrc - p);  // light direction
  vec3 v = -rayDir;                  // view direction
  vec3 r = reflect(-l, n);           // reflection direction

  vec3 specColor = vec3(0.6);
  vec3 specular = specColor * pow(clamp(dot(r, v), 0.0, 1.0), 10.0);
  vec3 diffuse = lightCol * clamp(dot(l, n), 0.0, 1.0);
  vec3 ambient = vec3(0.2);

  // shadow
  // 計算該點到光源的距離，如果有物體在中間(RayMarching 到光源途中撞到物體的距離< 光源與該點的距離)，則該點為陰影
  float d = rayMarch(p + n * 0.02, l).x;
  if (d < length(lightSrc - p)) {
    return ambient;
  }
  return specular + diffuse + ambient;
}

vec3 getColor(vec3 rayOrigin, vec3 rayDirection) {
  vec2 obj = rayMarch(rayOrigin, rayDirection);
  if (obj.x < MAX_DIST) {                           // hit
    vec3 point = rayOrigin + rayDirection * obj.x;  // the point where the ray hits the object
    vec3 normal = getNormal(point);
    vec3 baseColor = getMaterial(point, obj.y, normal);  // get the color of the object
    vec3 light = getLight(lightPosition, point, normal, lightColor, rayDirection);
    // vec3 light = vec3(1.0);  // for perf
    vec3 col = baseColor * light;
    // add fog
    col = mix(col, backgroundColor, smoothstep(0.0, 100.0, obj.x));
    return col;
  } else {  // no hit
    return backgroundColor;
  }
}

void main() {
  vec2 uv = (2.0 * gl_FragCoord.xy - resolution) / resolution.y;  // center origin point to the center of the screen
  vec3 rayOrigin = camPosition;

  mat3 newViewMatrix = mat3(viewMatrix);
  newViewMatrix[0][2] = -newViewMatrix[0][2];
  newViewMatrix[1][2] = -newViewMatrix[1][2];
  newViewMatrix[2][2] = -newViewMatrix[2][2];
  newViewMatrix = inverse(newViewMatrix);
  vec3 rayDirection = newViewMatrix * normalize(vec3(uv, 1.0 / tan(radians(fov) / 2.0)));

  vec3 color = getColor(rayOrigin, rayDirection);
  // gamma correction
  color = pow(color, vec3(1.0 / 2.2));
  FragColor = vec4(color, 1.0);
}
