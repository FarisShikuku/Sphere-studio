#version 460

layout(location = 0) in vec2 inGridXZ;

layout(std140, set = 0, binding = 0) uniform WaterSurfaceUniforms {
    mat4 view;
    mat4 proj;
    mat4 invViewProj;
    vec4 cameraPosAndTime;
    vec4 sunDirAndTurbidity;
    vec4 patchAndWaveParams;
    vec4 opticsParams;
    vec4 absorptionCoeffAndRefl;
    vec4 cutoutBounds;
    uint gridResolution;
    int debugMode;
    float seabedScale;
    float pad0;
} ubo;

layout(set = 0, binding = 1) uniform sampler2D uDisplacementMap;
layout(set = 0, binding = 2) uniform sampler2D uNormalJacobianMap;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec2 outSurfaceUv;
layout(location = 2) out vec3 outDisplacement;

void main() {
    float patchSize = max(1.0, ubo.patchAndWaveParams.x);
    vec2 uv = fract(inGridXZ / patchSize);
    vec3 disp = texture(uDisplacementMap, uv).xyz;

    vec3 worldPos = vec3(inGridXZ.x + disp.x, disp.y, inGridXZ.y + disp.z);
    outWorldPos = worldPos;
    outSurfaceUv = uv;
    outDisplacement = disp;

    gl_Position = ubo.proj * ubo.view * vec4(worldPos, 1.0);
}
