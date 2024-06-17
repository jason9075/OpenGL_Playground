#version 430 core

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D texture0;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

vec4 calcPointLight(vec3 normal, vec3 fragPos, vec3 lightPos, vec4 lightColor);

void main()
{
    fragColor = calcPointLight(normal, fragPos, lightPos, lightColor);
};

vec4 calcPointLight(vec3 normal, vec3 fragPos, vec3 lightPos, vec4 lightColor)
{
    // ambient lighting
    float ambient = 0.2f;

    // diffuse lighting
    vec3 norm = normalize(normal);
    vec3 diffDir = normalize(lightPos - fragPos);
    float diffuse = max(dot(norm, diffDir), 0.0f);

    // specular lighting
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(camPos - fragPos);
    vec3 reflectDir = reflect(-diffDir, norm);
    float specAmount = pow(max(dot(viewDir, reflectDir), 0.0f), 8);
    float specular = specularStrength * specAmount;
    return (texture(texture0, texCoord) * (ambient + diffuse + specular)) * lightColor;
}
