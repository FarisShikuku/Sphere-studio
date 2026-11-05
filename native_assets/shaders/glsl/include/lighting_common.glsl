// lighting_common.glsl - Physically-based multi-light evaluation
// Reference: Gaukler/PlainRenderer and SaschaWillems/Vulkan

struct GpuLightData {
    vec4 positionAndType;       // xyz = worldPos, w = LightType (0=Dir, 1=Point, 2=Spot, 3=Area)
    vec4 directionAndRange;     // xyz = worldDir, w = range
    vec4 colorAndIntensity;     // xyz = rgb, w = intensity (Lumens/Lux)
    vec4 spotParams;            // x = cos(inner), y = cos(outer), z = spotFalloff, w = radius
    vec4 areaParams;            // x = areaWidth, y = areaHeight, z = volumetricScattering, w = castShadows
    mat4 lightSpaceMatrix;
};

// Inverse-square distance attenuation with smooth radius windowing (Gaukler/PlainRenderer)
float computeDistanceAttenuation(float dist, float range) {
    float invSq = 1.0 / max(dist * dist, 0.01);
    float factor = dist / range;
    float smoothFactor = clamp(1.0 - factor * factor * factor * factor, 0.0, 1.0);
    return invSq * (smoothFactor * smoothFactor);
}

// Spot light angular attenuation (Flashlight smooth penumbra)
float computeSpotAttenuation(vec3 lightToPos, vec3 spotDir, float cosInner, float cosOuter, float exponent) {
    float cosAngle = dot(normalize(lightToPos), normalize(spotDir));
    float angleAttenuation = clamp((cosAngle - cosOuter) / max(cosInner - cosOuter, 0.001), 0.0, 1.0);
    return pow(angleAttenuation, exponent);
}

// Area light representative point approximation on rectangular emitter panel
vec3 computeAreaLightL(vec3 worldPos, vec3 lightCenter, vec3 lightDir, vec3 normal, float width, float height) {
    vec3 right = normalize(cross(lightDir, vec3(0.0, 1.0, 0.0)));
    if (length(right) < 0.1) {
        right = normalize(cross(lightDir, vec3(0.0, 0.0, 1.0)));
    }
    vec3 up = cross(right, lightDir);

    // Vector to center
    vec3 toCenter = lightCenter - worldPos;
    
    // Project onto panel local coordinate plane
    float projX = clamp(dot(toCenter, right), -width * 0.5, width * 0.5);
    float projY = clamp(dot(toCenter, up), -height * 0.5, height * 0.5);
    
    // Representative emitter point closest to reflection ray
    vec3 closestPoint = lightCenter + right * projX + up * projY;
    return normalize(closestPoint - worldPos);
}

// Evaluate complete illumination from a single GpuLightData
vec3 evaluateLightIllumination(
    GpuLightData light,
    vec3 worldPos,
    vec3 N,
    vec3 V,
    vec3 albedo,
    float roughness,
    float metallic
) {
    int lightType = int(light.positionAndType.w);
    vec3 lightPos = light.positionAndType.xyz;
    vec3 lightDir = light.directionAndRange.xyz;
    float range = light.directionAndRange.w;
    vec3 lightColor = light.colorAndIntensity.xyz;
    float intensity = light.colorAndIntensity.w;

    vec3 L;
    float attenuation = 1.0;

    if (lightType == 0) {
        // Directional Sun
        L = normalize(-lightDir);
        attenuation = 1.0;
    } else if (lightType == 1) {
        // Point Light (Bulb)
        vec3 toLight = lightPos - worldPos;
        float dist = length(toLight);
        L = normalize(toLight);
        attenuation = computeDistanceAttenuation(dist, range);
    } else if (lightType == 2) {
        // Spot Light (Flashlight)
        vec3 toLight = lightPos - worldPos;
        float dist = length(toLight);
        L = normalize(toLight);
        attenuation = computeDistanceAttenuation(dist, range);
        
        float cosInner = light.spotParams.x;
        float cosOuter = light.spotParams.y;
        float falloff = light.spotParams.z;
        attenuation *= computeSpotAttenuation(-L, lightDir, cosInner, cosOuter, falloff);
    } else if (lightType == 3) {
        // Area Light (Rectangular softbox)
        float areaW = light.areaParams.x;
        float areaH = light.areaParams.y;
        vec3 toLight = lightPos - worldPos;
        float dist = length(toLight);
        L = computeAreaLightL(worldPos, lightPos, lightDir, N, areaW, areaH);
        attenuation = computeDistanceAttenuation(dist, range);
    }

    float NdotL = max(dot(N, L), 0.0);
    if (NdotL <= 0.0 || attenuation <= 0.0) {
        return vec3(0.0);
    }

    // Cook-Torrance Microfacet Specular BRDF (GGX + Smith + Schlick)
    vec3 H = normalize(V + L);
    float NdotV = max(dot(N, V), 0.001);
    float NdotH = max(dot(N, H), 0.0);
    float VdotH = max(dot(V, H), 0.0);

    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;

    // Normal Distribution (GGX)
    float denom = (NdotH * NdotH * (alpha2 - 1.0) + 1.0);
    float D = alpha2 / (3.14159265 * denom * denom + 1e-5);

    // Geometric Shadowing (Smith Schlick-GGX)
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    float g1 = NdotL / (NdotL * (1.0 - k) + k);
    float g2 = NdotV / (NdotV * (1.0 - k) + k);
    float G = g1 * g2;

    // Fresnel (Schlick)
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = F0 + (vec3(1.0) - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);

    vec3 specular = (D * G * F) / max(4.0 * NdotL * NdotV, 0.001);
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
    vec3 diffuse = kD * albedo / 3.14159265;

    // Radiance contribution
    vec3 radiance = lightColor * (intensity * 0.01) * attenuation;
    return (diffuse + specular) * radiance * NdotL;
}
