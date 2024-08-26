#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 backgroundColor = vec3(0.4, 0.6, 0.95);  // sky blue
uniform int classId;
uniform sampler2D texture0;

uniform vec3 lightPosition;
uniform vec3 lightColor = vec3(1.0);
uniform vec3 camPosition;

vec4 hash4(vec2 p) {
  return fract(sin(vec4(1.0 + dot(p, vec2(37.0, 17.0)), 2.0 + dot(p, vec2(11.0, 47.0)), 3.0 + dot(p, vec2(41.0, 29.0)),
                        4.0 + dot(p, vec2(23.0, 31.0)))) *
               103.0);
}

// https://iquilezles.org/articles/texturerepetition/

vec4 textureNoTile1(sampler2D samp, in vec2 uv) {
  ivec2 iuv = ivec2(floor(uv));
  vec2 fuv = fract(uv);

  // generate per-tile transform
  vec4 ofa = hash4(iuv + ivec2(0, 0));
  vec4 ofb = hash4(iuv + ivec2(1, 0));
  vec4 ofc = hash4(iuv + ivec2(0, 1));
  vec4 ofd = hash4(iuv + ivec2(1, 1));

  vec2 ddx = dFdx(uv);
  vec2 ddy = dFdy(uv);

  // transform per-tile uvs
  ofa.zw = sign(ofa.zw - 0.5);
  ofb.zw = sign(ofb.zw - 0.5);
  ofc.zw = sign(ofc.zw - 0.5);
  ofd.zw = sign(ofd.zw - 0.5);

  // uv's, and derivatives (for correct mipmapping)
  vec2 uva = uv * ofa.zw + ofa.xy, ddxa = ddx * ofa.zw, ddya = ddy * ofa.zw;
  vec2 uvb = uv * ofb.zw + ofb.xy, ddxb = ddx * ofb.zw, ddyb = ddy * ofb.zw;
  vec2 uvc = uv * ofc.zw + ofc.xy, ddxc = ddx * ofc.zw, ddyc = ddy * ofc.zw;
  vec2 uvd = uv * ofd.zw + ofd.xy, ddxd = ddx * ofd.zw, ddyd = ddy * ofd.zw;

  // fetch and blend
  vec2 b = smoothstep(0.25, 0.75, fuv);

  return mix(mix(textureGrad(samp, uva, ddxa, ddya), textureGrad(samp, uvb, ddxb, ddyb), b.x),
             mix(textureGrad(samp, uvc, ddxc, ddyc), textureGrad(samp, uvd, ddxd, ddyd), b.x), b.y);
}
vec4 textureNoTile2(sampler2D samp, in vec2 uv) {
  vec2 p = floor(uv);
  vec2 f = fract(uv);

  // derivatives (for correct mipmapping)
  vec2 ddx = dFdx(uv);
  vec2 ddy = dFdy(uv);

  // voronoi contribution
  vec4 va = vec4(0.0);
  float wt = 0.0;
  for (int j = -1; j <= 1; j++)
    for (int i = -1; i <= 1; i++) {
      vec2 g = vec2(float(i), float(j));
      vec4 o = hash4(p + g);
      vec2 r = g - f + o.xy;
      float d = dot(r, r);
      float w = exp(-5.0 * d);
      vec4 c = textureGrad(samp, uv + o.zw, ddx, ddy);
      va += w * c;
      wt += w;
    }

  // normalization
  return va / wt;
}

vec3 map(int classId, vec3 p) {
  float scale = 0.5;
  p *= scale;
  vec3 norm;
  switch (classId) {
    case 0:  // checkerboard ground
      return vec3(0.2 + 0.4 * mod(floor(p.x) + floor(p.z), 2.0));
    case 1:  // room walls
      norm = abs(Normal);
      return (textureNoTile2(texture0, p.xy * 0.5 + 0.5).rgb * norm.z +
              textureNoTile2(texture0, p.zy * 0.5 + 0.5).rgb * norm.x)
          .rgb;
      // return (texture(texture0, p.xy * 0.5 + 0.5) * norm.z + texture(texture0, p.zy * 0.5 + 0.5) * norm.x).rgb;
    case 2:  // room floor
      p *= 0.5;
      norm = abs(Normal);
      // return (texture(texture0, p.xz * 0.5 + 0.5) * norm.y).rgb;
      return (textureNoTile1(texture0, p.xz * 0.5 + 0.5).rgb * norm.y).rgb;
    case 3:  // room ceiling
      return vec3(1.0, 1.0, 0.9);
    default:
      return vec3(1.0);
  }
}

void main() {
  // checkerboard pattern
  vec3 color = map(classId, FragPos);
  // add fog
  float dist = distance(camPosition, FragPos);
  color = mix(color, backgroundColor, smoothstep(0.0, 70.0, dist));

  FragColor = vec4(color, 1.0);
}
