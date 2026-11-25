#version 460

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec2 inSurfaceUv;
layout(location = 2) in vec3 inDisplacement;

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

layout(set = 0, binding = 2) uniform sampler2D uNormalJacobianMap;

layout(location = 0) out vec4 outFragColor;

vec3 EvaluatePreethamSky(vec3 dir, vec3 sunDir, float turbidity) {
    float horizon = pow(1.0 - clamp(dir.y, 0.0, 1.0), 2.8);
    float sunWarmth = clamp(1.0 - sunDir.y * 1.15, 0.0, 1.0);
    vec3 zenith = vec3(0.14, 0.38, 0.78);
    vec3 horizonCol = mix(vec3(0.56, 0.74, 0.90), vec3(0.96, 0.58, 0.34), sunWarmth * clamp((turbidity - 1.8) / 6.7, 0.25, 1.0));
    vec3 sky = mix(zenith, horizonCol, horizon);
    float sunCos = max(0.0, dot(dir, sunDir));
    sky += mix(vec3(1.0, 0.95, 0.88), vec3(1.0, 0.64, 0.28), sunWarmth) * (pow(sunCos, 10.0) * 0.35 + pow(sunCos, 320.0) * 3.8);
    return sky;
}

void main() {
    if (ubo.cutoutBounds.x > 0.5) {
        vec2 halfExtent = max(vec2(1.0), vec2(ubo.cutoutBounds.y, ubo.cutoutBounds.w) * 0.5);
        if (abs(inWorldPos.x) > halfExtent.x || abs(inWorldPos.z) > halfExtent.y) {
            discard;
        }
    }
    vec4 nj = texture(uNormalJacobianMap, inSurfaceUv);
    vec3 normal = normalize(nj.xyz);
    float jacobian = nj.w;

    vec3 viewVec = normalize(ubo.cameraPosAndTime.xyz - inWorldPos);
    vec3 sunDir = normalize(ubo.sunDirAndTurbidity.xyz);
    float ior = max(1.01, ubo.opticsParams.x);
    float f0 = pow((ior - 1.0) / (ior + 1.0), 2.0);
    float fresnel = clamp(f0 + (1.0 - f0) * pow(1.0 - max(0.0, dot(normal, viewVec)), 5.0), 0.0, 1.0);
    float foam = clamp((ubo.opticsParams.w - jacobian) * 2.6, 0.0, 1.0);

    vec3 refractDir = refract(-viewVec, normal, 1.0 / ior);
    float depthT = max(0.3, (-ubo.patchAndWaveParams.z - inWorldPos.y) / min(-0.08, refractDir.y));
    vec3 transmittance = exp(-ubo.absorptionCoeffAndRefl.rgb * (depthT * ubo.opticsParams.y));
    vec3 refracted = mix(vec3(0.015, 0.16, 0.26), vec3(0.64, 0.60, 0.46) * max(0.2, sunDir.y), transmittance);

    vec3 reflectDir = reflect(-viewVec, normal);
    reflectDir.y = max(0.02, reflectDir.y);
    vec3 reflected = EvaluatePreethamSky(reflectDir, sunDir, ubo.sunDirAndTurbidity.w) * ubo.absorptionCoeffAndRefl.w;

    vec3 halfVec = normalize(sunDir + viewVec);
    float sunSpec = pow(max(0.0, dot(normal, halfVec)), 240.0) * 2.6;
    vec3 water = mix(refracted, reflected, fresnel) + vec3(1.0, 0.94, 0.82) * sunSpec;
    water = mix(water, vec3(0.92, 0.96, 0.98), foam * 0.88);

    outFragColor = vec4(water, 1.0);
}
