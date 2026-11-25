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
    uint gridResolution;
    int debugMode;
    float seabedScale;
    float pad0;
} ubo;

layout(location = 0) out vec3 outBedPos;

float hash21(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float valueNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    float a = hash21(i);
    float b = hash21(i + vec2(1.0, 0.0));
    float c = hash21(i + vec2(0.0, 1.0));
    float d = hash21(i + vec2(1.0, 1.0));
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y) * 2.0 - 1.0;
}

void main() {
    float s = ubo.seabedScale;
    float h = valueNoise(inGridXZ * s) * 0.62 + valueNoise(inGridXZ * s * 2.15 + 13.7) * 0.28;
    float bedY = min(-0.45, -ubo.patchAndWaveParams.z + h * ubo.patchAndWaveParams.w);
    vec3 pos = vec3(inGridXZ.x, bedY, inGridXZ.y);
    outBedPos = pos;
    gl_Position = ubo.proj * ubo.view * vec4(pos, 1.0);
}
