#version 450
// ============================================================================
// tree_vulkan.frag - Vulkan 1.2/1.3 Real-Time Procedural Tree Fragment Shader
// Implements Stojanovic (2016) Bark Texturing & Leaf Translucency
// ============================================================================

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inColor;
layout(location = 4) in flat uint inIsLeaf;
layout(location = 5) in float inSunlitFactor;

layout(set = 0, binding = 0) uniform TreeVulkanUBO {
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 windParams;
    vec4 sunDirection;
    vec4 ambientColor;
} ubo;

layout(set = 0, binding = 1) uniform sampler2D barkTexture;
layout(set = 0, binding = 2) uniform sampler2D barkNormalMap;

layout(location = 0) out vec4 outFragColor;

void main() {
    vec3 N = normalize(inNormal);
    vec3 L = normalize(ubo.sunDirection.xyz);
    vec3 V = normalize(-inWorldPos);

    if (inIsLeaf == 1u) {
        // Two-sided foliage leaf illumination with subsurface translucency
        float NdotL = dot(N, L);
        float directDiffuse = max(NdotL, 0.0);
        float transmission = max(-NdotL, 0.0) * 0.45; // Subsurface scattering through leaf blade
        float totalDiffuse = directDiffuse + transmission;

        vec3 leafColor = inColor.rgb * (totalDiffuse * 0.75 + ubo.ambientColor.rgb * 0.35);

        // Subtle specular highlight on waxy cuticle
        vec3 H = normalize(L + V);
        float NdotH = max(dot(N, H), 0.0);
        float spec = pow(NdotH, 24.0) * 0.25 * directDiffuse;

        outFragColor = vec4(leafColor + vec3(spec), inColor.a);
    } else {
        // Wood bark internode cylinder (Stojanovic 2016 Equations 2-4)
        // Multi-octave sinusoidal modulation on bark ridges
        float u = inTexCoord.x;
        float v = inTexCoord.y;

        float kMod = sin(u * 20.0) * cos(v * 40.0) * 0.5;
        float barkRidge = sin(u * 25.0 + kMod * 2.0) + sin(v * 50.0 + kMod);
        float barkShade = clamp(1.0 + barkRidge * 0.15, 0.65, 1.35);

        float NdotL = max(dot(N, L), 0.0);
        vec3 diffuse = inColor.rgb * barkShade * (NdotL * 0.8 + ubo.ambientColor.rgb * 0.3);

        outFragColor = vec4(diffuse, 1.0);
    }
}
