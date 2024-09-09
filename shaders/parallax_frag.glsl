#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoord;
in vec3 interior_coords;

out vec4 FragColor;

uniform sampler2D texture0;
uniform float farFactor = 1.0;

const vec2 resolution = vec2(1024.0, 768.0);

void main() {
  vec3 color;
  vec2 uv = (2.0 * gl_FragCoord.xy - resolution) / resolution.x;  // range [-1, 1]

  float depthScale = 1.0 / (1.0 - farFactor) - 1.0;
  vec3 pos = vec3(uv, -1);

  vec3 id = 1.0 / interior_coords.xyz;
  vec3 k = abs(id) - pos * id;
  float kMin = min(min(k.x, k.y), k.z);
  pos += kMin * interior_coords.xyz;

  float interp = pos.z * 0.5 + 0.5;
  float realZ = clamp(interp, 0, 1) / depthScale + 1.0;
  interp = 1.0 - (1.0 / realZ);
  interp *= depthScale + 1.0;

  vec2 interiorUV = pos.xy * mix(1.0, farFactor, interp);
  interiorUV = interiorUV * 0.5 + vec2(0.5);
  interiorUV.y = 1.0 - interiorUV.y;

  vec4 room_color = texture(texture0, interiorUV);

  // show color at thr back of the object
  if (gl_FrontFacing) {
    color = room_color.rgb;  // texture
  } else {
    color = Color;  // origin color
  }
  FragColor = vec4(color, 1.0);
}
