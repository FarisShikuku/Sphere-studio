#version 460
// Vulkan 1.3 Graphics Fragment Shader: Additive Spherical Fluid Thickness
// Ported from talvinckb/OpenGL-Water-Simulation

layout(location = 0) in vec2 inQuadUv;
layout(location = 0) out float outThickness;

void main() {
    float r2 = dot(inQuadUv, inQuadUv);
    if (r2 > 1.0) discard;

    float sphereChord = sqrt(1.0 - r2);
    outThickness = sphereChord * 0.14;
}
