#version 330 core

layout(location = 0) in vec3 aPos;

out vec3 texCoords;

uniform float scaleFactor;
uniform mat4 viewMatrix;
uniform mat4 camMatrix;
uniform bool fixSkybox;

void main() {
  texCoords = aPos;

  if (fixSkybox) {
    mat4 pvMatrix = camMatrix;  // projection * view
    // remove translation
    pvMatrix[3][0] = 0.0;
    pvMatrix[3][1] = 0.0;
    pvMatrix[3][2] = 0.0;
    pvMatrix[3][3] = 1.0;
    vec4 pos = pvMatrix * vec4(aPos * scaleFactor, 1.0);
    gl_Position = pos.xyww;   // let z be 1.0 to make sure skybox is rendered behind everything
    gl_Position.z -= 0.0001;  // avoid z-fighting with other objects
  } else {
    vec4 pos = camMatrix * vec4(aPos * scaleFactor, 1.0);
    gl_Position = pos;
  }
}
