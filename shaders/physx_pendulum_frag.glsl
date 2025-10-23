#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoord;

out vec4 FragColor;

uniform bool useTexture = false;
uniform vec3 lightColor = vec3(1.0f);
uniform vec3 lightPosition = vec3(3.0f,5.0f,1.0f);
uniform vec3 camPosition;
uniform sampler2D texture0;

uniform bool ambientEnabled = true;
uniform bool diffuseEnabled = true;
uniform bool specularEnabled = true;

vec4 calcPointLight(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor);

void main() { FragColor = calcPointLight(Normal, FragPos, lightPosition, lightColor); }

vec4 calcPointLight(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightCol) {
  // ambient lighting
  float ambient = 0.2f;

  // diffuse lighting
  vec3 norm = normalize(Normal);
  vec3 diffDir = normalize(lightPos - fragPos);
  float diffuse = max(dot(norm, diffDir), 0.0f);

  // specular lighting
  float specularStrength = 0.5f;
  vec3 viewDir = normalize(camPosition - fragPos);
  vec3 reflectDir = reflect(-diffDir, norm);
  float specAmount = pow(max(dot(viewDir, reflectDir), 0.0f), 8);
  float specular = specularStrength * specAmount;

  ambient = ambientEnabled ? ambient : 0.0f;
  diffuse = diffuseEnabled ? diffuse : 0.0f;
  specular = specularEnabled ? specular : 0.0f;

  vec4 baseColor = useTexture ? texture(texture0, TexCoord) : vec4(Color, 1.0f);

  return baseColor * (ambient + diffuse + specular) * vec4(lightCol, 1.0f);
}
