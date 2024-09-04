#version 330 core

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

out vec4 fragColor;

uniform sampler2D texture0;

const vec3 lightColor = vec3(1.0f);
const vec3 lightPos = vec3(1.0, 3.0, 0.0);

void main()
{
  float ambient = 0.2f;
  vec3 norm = normalize(Normal);
  vec3 diffDir = normalize(lightPos - FragPos);
  float diffuse = max(dot(norm, diffDir), 0.0f);
  vec3 color = lightColor * (ambient + diffuse);
  fragColor = vec4(color,1.0);
}
