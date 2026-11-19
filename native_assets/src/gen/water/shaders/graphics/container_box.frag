#version 460
// Vulkan 1.3 Graphics Fragment Shader: Container Wireframe & Caustic Floor

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform ContainerPush {
    vec4 baseColor;
    float timeSeconds;
    float causticsIntensity;
} pc;

void main() {
    float checker = mod(floor(inWorldPos.x * 2.5) + floor(inWorldPos.z * 2.5), 2.0);
    vec3 col = pc.baseColor.rgb * (0.85 + 0.15 * checker);
    float caustic = sin(inWorldPos.x * 6.0 + pc.timeSeconds * 2.3) * cos(inWorldPos.z * 6.0 - pc.timeSeconds * 1.9);
    col += vec3(0.18, 0.35, 0.45) * max(caustic, 0.0) * pc.causticsIntensity;
    outColor = vec4(col, pc.baseColor.a);
}
