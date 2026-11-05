#version 450
// sky_atmosphere.frag - Physically Based Atmosphere Skybox Fragment Shader
// Referencing Gaukler/PlainRenderer and Bruneton/Nishita atmosphere model

#include "include/atmosphere_common.glsl"

layout(location = 0) in vec3 inViewRay;
layout(location = 0) out vec4 outSkyRadiance;

layout(set = 0, binding = 0) uniform AtmosphereUBO {
    GpuAtmosphereData u_atmosphere;
};

void main() {
    vec3 dir = normalize(inViewRay);
    vec3 radiance = evaluateSkyRadiance(u_atmosphere, dir);

    // Apply basic Reinhard tone mapping for HDR sky values
    vec3 toneMapped = radiance / (radiance + vec3(1.0));

    // Convert to gamma space
    toneMapped = pow(toneMapped, vec3(1.0 / 2.2));

    outSkyRadiance = vec4(toneMapped, 1.0);
}
