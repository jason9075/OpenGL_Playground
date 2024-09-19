#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoord;

out vec4 fragColor;

uniform sampler2D texture0;

uniform bool enableLighting;
const vec3 lightColor = vec3(1.0f);
const vec3 lightPos = vec3(1.0, 3.0, 0.0);

void main()
{
  float ambient = 0.2f;
  vec3 norm = normalize(Normal);
  vec3 diffDir = normalize(lightPos - FragPos);
  float diffuse = max(dot(norm, diffDir), 0.0f);
  vec3 lighting = enableLighting ? lightColor * (ambient + diffuse) : vec3(1.0f);
  vec3 pixelColor = Color * lighting;
  fragColor = vec4(pixelColor, 1.0);
}
