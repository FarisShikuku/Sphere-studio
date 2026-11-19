#version 460
// Vulkan 1.3 Graphics Fragment Shader: Depth-Aware Separable Bilateral Gaussian Filter
// Ported from talvinckb/OpenGL-Water-Simulation

layout(location = 0) in vec2 inUv;
layout(location = 0) out float outSmoothedDepth;

layout(set = 1, binding = 1) uniform sampler2D u_DepthTex;

layout(push_constant) uniform BlurPush {
    vec2 texelDir;      // (1/W, 0) for horizontal or (0, 1/H) for vertical
    int blurRadius;
    float spatialSigma;
    float depthSigma;
} pc;

void main() {
    float centerDepth = texture(u_DepthTex, inUv).r;
    if (centerDepth <= 0.0001 || centerDepth > 100.0) {
        outSmoothedDepth = centerDepth;
        return;
    }

    float sum = 0.0;
    float wSum = 0.0;
    float invTwoSpatialSq = 1.0 / (2.0 * pc.spatialSigma * pc.spatialSigma);
    float invTwoDepthSq = 1.0 / (2.0 * pc.depthSigma * pc.depthSigma);

    for (int i = -pc.blurRadius; i <= pc.blurRadius; ++i) {
        vec2 sampleUv = inUv + pc.texelDir * float(i);
        float sampleDepth = texture(u_DepthTex, sampleUv).r;
        if (sampleDepth <= 0.0001 || sampleDepth > 100.0) continue;

        float r2 = float(i * i);
        float spatialWeight = exp(-r2 * invTwoSpatialSq);

        float dDiff = sampleDepth - centerDepth;
        float rangeWeight = exp(-(dDiff * dDiff) * invTwoDepthSq);

        float w = spatialWeight * rangeWeight;
        sum += sampleDepth * w;
        wSum += w;
    }

    outSmoothedDepth = (wSum > 1e-6) ? (sum / wSum) : centerDepth;
}
