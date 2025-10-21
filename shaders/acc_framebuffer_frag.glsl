#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D oldFrame;
uniform sampler2D newFrame;
uniform int numFrames = 0;

void main() {
  vec4 oldColor = texture(oldFrame, TexCoord);
  vec4 newColor = texture(newFrame, TexCoord);

  if (numFrames < 0) {
    FragColor = oldColor;
    return;
  }
  // accumulate the color
  float weight = 1.0 / float(numFrames + 1);
  FragColor = mix(oldColor, newColor, weight);
}
