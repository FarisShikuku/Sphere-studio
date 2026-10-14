#version 460
#include "include/vulkan_compat.glsl"
#include "include/atmosphere_common.glsl"

layout(location = 0) in vec3 inWorldDir;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform SkyFragPush {
    mat4 invViewProj;
    vec4 sunDirection; // xyz = dir, w = intensity
    vec4 sunColor;     // xyz = rgb, w = ambient
    float turbidity;
} sky;

layout(set = SCENE_SET, binding = 1) uniform sampler2D u_SkyLUT;

void main() {
    vec3 dir = normalize(inWorldDir);
    float altitude = asin(clamp(dir.y, 0.0, 1.0));
    float azimuth = atan(dir.x, dir.z);
    vec2 uv = vec2((azimuth + PI) / TWO_PI, altitude / (0.5 * PI));

    float cosGamma = dot(dir, normalize(sky.sunDirection.xyz));
    vec3 transmittance = ComputeAtmosphericTransmittance(sky.sunDirection.w, sky.turbidity);
    float rayleigh = RayleighPhase(cosGamma);
    float mie = MiePhase(cosGamma, 0.76);

    vec3 skyColor = (RAYLEIGH_SCATTERING * rayleigh + vec3(MIE_SCATTERING) * mie) * transmittance * sky.sunColor.rgb * sky.sunDirection.w;

    // Direct sun disk glow
    float sunAngularRadius = 0.00935; // ~0.53 degrees
    if (cosGamma > cos(sunAngularRadius)) {
        skyColor += sky.sunColor.rgb * 15.0 * transmittance;
    } else {
        float sunCorona = pow(max(0.0, cosGamma), 256.0);
        skyColor += sky.sunColor.rgb * sunCorona * 2.5;
    }

    // Tone map sky
    skyColor = skyColor / (skyColor + vec3(1.0));
    outColor = vec4(pow(skyColor, vec3(1.0 / 2.2)), 1.0);
}
