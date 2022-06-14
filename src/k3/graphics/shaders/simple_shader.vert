#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
  mat4 transform;
  mat4 normalMatrix;
} push;

const vec4 DIRECTION_TO_LIGHT = normalize(vec4(-1.0, -3.0, -1.0, 0));
const float AMBIENT = 0.03;

void main() {
  gl_Position = push.transform * vec4(position, 1.0);
  
  //vec4 normalWorldSpace = vec4(normalize(mat3(push.normalMatrix) * normal), 0);

  vec4 normalWorldSpace = normalize(push.transform * vec4(normal,0));

  float lightIntensity = AMBIENT + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);

  fragColor = lightIntensity * color;
}