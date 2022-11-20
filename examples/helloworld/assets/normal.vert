#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

out vec4 fragColor;

void main() {
  mat4 MVP = projMatrix * viewMatrix * modelMatrix;

  gl_Position = MVP * vec4(inPosition, 1.0);

  vec3 N = inNormal;  // Object space
  // vec3 N = normalMatrix * inNormal; // Eye space

  // Convert from [-1,1] to [0,1]
  
  vec3 rubikColor = vec3(0.5, 0.5, 0.5);
  if (N.y > 0.0 && N.x == 0.0 && N.z == 0.0) {
    // white top
    rubikColor = vec3(1.0, 1.0, 1.0);
  }
  else if (N.y < 0.0 && N.x == 0.0 && N.z == 0.0) {
    // yellow bottom
    rubikColor = vec3(1.0, 1.0, 0.0);
  }

  else if (N.x > 0.0 && N.y == 0.0 && N.z == 0.0) {
    // green side
    rubikColor = vec3(0.0, 0.5, 0.0);
  }
  else if (N.x < 0.0 && N.y == 0.0 && N.z == 0.0) {
    // blue side
    rubikColor = vec3(0.0, 0.0, 1.0);
  }

  else if (N.z > 0.0 && N.x == 0.0 && N.y == 0.0) {
    // orange side
    rubikColor = vec3(1.0, 0.666, 0.0);
  }
  else if (N.z < 0.0 && N.x == 0.0 && N.y == 0.0) {
    // red side
    rubikColor = vec3(1.0, 0.0, 0.0);
  }


  fragColor = vec4(rubikColor, 1.0);
}