#version 460
layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec3 outDirection;

void main() {
    outDirection = inPosition;
    gl_Position = vec4(inPosition.xy, 1.0, 1.0);
}\n