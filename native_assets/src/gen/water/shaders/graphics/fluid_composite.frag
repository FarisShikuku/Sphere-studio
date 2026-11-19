#version 460
// Vulkan 1.3 Graphics Fragment Shader: Screen-Space Fluid Rendering (SSFR) Composite
// Ported from talvinckb/OpenGL-Water-Simulation

layout(location = 0) in vec2 inUv;
layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform sampler2D u_SmoothedDepth;
layout(set = 1, binding = 2) uniform sampler2D u_ThicknessTex;
layout(set = 1, binding = 3) uniform sampler2D u_SceneBackgroundTex;

layout(std140, set = 1, binding = 0) uniform FluidUbo {
    mat4 view;
    mat4 proj;
    mat4 invProj;
    vec4 cameraPosAndTime;
    vec4 boundsHalfAndRadius;
    vec4 gravityAndDt;
    vec4 sphConstants;
    vec4 sphForces;
    vec4 windAndObstacle;
    vec4 obstacleSphere;
    vec4 waterColorAndRefract;
    vec4 absorptionAndStrength;
    vec4 opticsParams;
    vec4 sunDirAndFoam;
    uint screenWidth;
    uint screenHeight;
    int debugMode;
    int blurRadius;
    float blurSpatialSigma;
    float blurDepthSigma;
    float causticsIntensity;
    float ior;
} ubo;

vec3 ReconstructViewPos(vec2 uv, float linearDepth) {
    vec2 ndc = uv * 2.0 - 1.0;
    float tanHalfFov = 1.0 / ubo.proj[1][1];
    float aspect = ubo.proj[1][1] / ubo.proj[0][0];
    return vec3(ndc.x * aspect * tanHalfFov * linearDepth, -ndc.y * tanHalfFov * linearDepth, -linearDepth);
}

vec3 SampleProceduralSky(vec3 dir, vec3 sunDir) {
    float up = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 horizon = vec3(0.58, 0.76, 0.92);
    vec3 zenith = vec3(0.08, 0.26, 0.58);
    vec3 sky = mix(horizon, zenith, pow(up, 0.65));
    float sunAmt = max(dot(dir, sunDir), 0.0);
    sky += vec3(1.0, 0.94, 0.80) * pow(sunAmt, 96.0) * 1.4;
    return sky;
}

void main() {
    float depth = texture(u_SmoothedDepth, inUv).r;
    vec4 bg = texture(u_SceneBackgroundTex, inUv);
    if (depth <= 0.0001 || depth > 90.0) {
        outColor = bg;
        return;
    }

    vec2 texel = 1.0 / vec2(float(ubo.screenWidth), float(ubo.screenHeight));
    vec3 viewPos = ReconstructViewPos(inUv, depth);

    float dR = texture(u_SmoothedDepth, inUv + vec2(texel.x, 0.0)).r;
    float dL = texture(u_SmoothedDepth, inUv - vec2(texel.x, 0.0)).r;
    float dU = texture(u_SmoothedDepth, inUv + vec2(0.0, texel.y)).r;
    float dD = texture(u_SmoothedDepth, inUv - vec2(0.0, texel.y)).r;

    vec3 ddx = (abs(dR - depth) < abs(depth - dL) && dR < 90.0)
        ? (ReconstructViewPos(inUv + vec2(texel.x, 0.0), dR) - viewPos)
        : (dL < 90.0 ? (viewPos - ReconstructViewPos(inUv - vec2(texel.x, 0.0), dL)) : vec3(texel.x, 0.0, 0.0));

    vec3 ddy = (abs(dU - depth) < abs(depth - dD) && dU < 90.0)
        ? (ReconstructViewPos(inUv + vec2(0.0, texel.y), dU) - viewPos)
        : (dD < 90.0 ? (viewPos - ReconstructViewPos(inUv - vec2(0.0, texel.y), dD)) : vec3(0.0, texel.y, 0.0));

    vec3 viewNormal = normalize(cross(ddx, ddy));
    if (viewNormal.z < 0.0) viewNormal = -viewNormal;

    float thickness = texture(u_ThicknessTex, inUv).r;

    if (ubo.debugMode == 1 || ubo.debugMode == 2) {
        float vis = clamp(1.0 - (depth - 1.5) / 6.5, 0.0, 1.0);
        outColor = vec4(vec3(vis), 1.0);
        return;
    } else if (ubo.debugMode == 3) {
        outColor = vec4(viewNormal * 0.5 + 0.5, 1.0);
        return;
    } else if (ubo.debugMode == 4) {
        outColor = vec4(vec3(clamp(thickness * 0.65, 0.0, 1.0)), 1.0);
        return;
    }

    // Beer-Lambert optical absorption
    vec3 absorption = exp(-ubo.absorptionAndStrength.rgb * thickness * ubo.absorptionAndStrength.w);
    vec2 refractUv = clamp(inUv + viewNormal.xy * ubo.waterColorAndRefract.w * (0.4 + thickness * 0.35), 0.001, 0.999);
    vec3 refractedScene = texture(u_SceneBackgroundTex, refractUv).rgb;
    vec3 transmitted = refractedScene * absorption + ubo.waterColorAndRefract.rgb * (1.0 - absorption) * 0.55;

    // Fresnel & Specular
    vec3 viewDir = normalize(-viewPos);
    float ndotv = clamp(dot(viewNormal, viewDir), 0.0, 1.0);
    float fresnel = ubo.opticsParams.y + (1.0 - ubo.opticsParams.y) * pow(1.0 - ndotv, ubo.opticsParams.x);

    mat3 invViewRot = transpose(mat3(ubo.view));
    vec3 worldNormal = normalize(invViewRot * viewNormal);
    vec3 worldViewDir = normalize(invViewRot * viewDir);
    vec3 reflDir = reflect(-worldViewDir, worldNormal);
    vec3 sunDir = normalize(ubo.sunDirAndFoam.xyz);
    vec3 skyRefl = SampleProceduralSky(reflDir, sunDir) * ubo.opticsParams.w;

    vec3 halfVec = normalize(sunDir + worldViewDir);
    float spec = pow(max(dot(worldNormal, halfVec), 0.0), ubo.opticsParams.z);

    vec3 finalColor = mix(transmitted, skyRefl, clamp(fresnel, 0.0, 0.96)) + vec3(1.0, 0.97, 0.92) * spec * 1.15;
    outColor = vec4(finalColor, 1.0);
}
