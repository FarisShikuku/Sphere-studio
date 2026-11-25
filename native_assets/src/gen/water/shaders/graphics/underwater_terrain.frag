#version 460

layout(location = 0) in vec3 inBedPos;
layout(location = 0) out vec4 outFragColor;

layout(std140, set = 0, binding = 0) uniform WaterSurfaceUniforms {
    mat4 view;
    mat4 proj;
    mat4 invViewProj;
    vec4 cameraPosAndTime;
    vec4 sunDirAndTurbidity;
    vec4 patchAndWaveParams;
    vec4 opticsParams;
    vec4 absorptionCoeffAndRefl;
    uint gridResolution;
    int debugMode;
    float seabedScale;
    float pad0;
} ubo;

void main() {
    float ripple = 0.5 + 0.5 * sin(inBedPos.x * 2.4 + cos(inBedPos.z * 1.9));
    vec3 sand = mix(vec3(0.58, 0.54, 0.42), vec3(0.76, 0.72, 0.56), ripple);
    float light = max(0.20, normalize(ubo.sunDirAndTurbidity.xyz).y);
    outFragColor = vec4(sand * light, 1.0);
}
