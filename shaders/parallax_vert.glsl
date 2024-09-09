#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Color;
out vec3 Normal;
out vec2 TexCoord;
out vec3 interior_coords;

uniform mat4 modelMatrix;
uniform mat4 camMatrix;
uniform vec3 camPosition;

void main() {
  FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
  Normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
  Color = aColor;
  TexCoord = aTexCoord;

  vec3 windowsPos = (modelMatrix * vec4(aPos, 1.0)).xyz;     // 窗戶的座標
  vec3 n = normalize(modelMatrix * vec4(aNormal, 0.0)).xyz;  // 窗戶對外的法向量
  vec3 i = normalize(windowsPos - camPosition);              // 從攝影機到窗戶的向量
  vec3 r = reflect(i, n);                                    // 反射向量

  interior_coords = r;

  /*
  (0, 1) (1, 1)
  (0, 0) (1, 0)
        */

  gl_Position = camMatrix * modelMatrix * vec4(aPos, 1.0);
}
