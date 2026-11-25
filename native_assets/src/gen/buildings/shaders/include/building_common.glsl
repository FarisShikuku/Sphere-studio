#ifndef BUILDING_COMMON_GLSL
#define BUILDING_COMMON_GLSL

struct BuildingVertex {
    vec3 position;
    vec3 normal;
    vec2 uv;
    uint materialId;
};

struct BuildingInstanceData {
    mat4 modelMatrix;
    vec4 dimensions;    // x=width, y=depth, z=totalHeight, w=floorHeight
    uvec4 styleFlags;   // x=shape, y=wallMaterial, z=glassType, w=floorCount
};

float buildingHash21(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

vec3 computeProceduralBrickMortar(vec2 uv, float brickRows, float brickCols, vec3 brickCol, vec3 mortarCol) {
    float row = floor(uv.y * brickRows);
    float offset = mod(row, 2.0) * 0.5;
    vec2 cell = vec2(fract(uv.x * brickCols + offset), fract(uv.y * brickRows));
    float mortarX = step(0.06, cell.x) * step(cell.x, 0.94);
    float mortarY = step(0.10, cell.y) * step(cell.y, 0.90);
    float mask = mortarX * mortarY;
    float tint = 0.90 + 0.18 * buildingHash21(vec2(floor(uv.x * brickCols + offset), row));
    return mix(mortarCol, brickCol * tint, mask);
}

vec3 computeGlassBaseTint(uint glassType) {
    if (glassType == 1u) return vec3(0.12, 0.15, 0.20);      // SolarTintedSmoke
    if (glassType == 2u) return vec3(0.58, 0.66, 0.74);      // ReflectiveChromeMirror
    if (glassType == 3u) return vec3(0.08, 0.48, 0.36);      // LowEEmerald
    if (glassType == 4u) return vec3(0.42, 0.25, 0.12);      // BronzeArchitectural
    if (glassType == 5u) return vec3(0.82, 0.87, 0.92);      // FrostedSatinPrivacy
    if (glassType == 6u) return vec3(0.86, 0.64, 0.18);      // GoldMirroredLuxury
    if (glassType == 7u) return vec3(0.42, 0.28, 0.78);      // StainedGlassArtisan
    return vec3(0.14, 0.42, 0.62);                           // ClearFloatArchitectural
}

float computeSchlickFresnelIor(float nDotV, float ior, uint glassType) {
    float safeIor = clamp(ior, 1.0, 2.8);
    float r0Sqrt = (safeIor - 1.0) / (safeIor + 1.0);
    float f0 = r0Sqrt * r0Sqrt;
    float reflControl = clamp((safeIor - 1.0) / 1.8, 0.0, 1.0);
    float coatBoost = reflControl * reflControl * 0.58;
    if (glassType == 2u || glassType == 6u) {
        coatBoost += 0.14 * reflControl;
    } else if (glassType == 5u) {
        coatBoost *= 0.35;
    }
    float baseF0 = clamp(f0 + coatBoost, 0.0, 0.92);
    return clamp(baseF0 + (1.0 - baseF0) * pow(clamp(1.0 - nDotV, 0.0, 1.0), 4.2), 0.0, 0.99);
}

float computeGlassPrivacyOpacity(float ior, float fresnel, uint glassType) {
    if (glassType == 5u) {
        // Frosted glass always scatters & obscures interior view
        return clamp(0.88 + 0.10 * clamp((ior - 1.0) / 1.8, 0.0, 1.0), 0.88, 0.99);
    }
    // From transparent at IOR ~1.0 (alpha ~0.26) smoothly up to completely opaque non-see-through at IOR >= 1.50
    float iorNorm = clamp((ior - 1.0) / 0.55, 0.0, 1.0);
    float smoothOpaque = iorNorm * iorNorm * (3.0 - 2.0 * iorNorm);
    return clamp(0.26 + 0.72 * smoothOpaque + 0.12 * fresnel, 0.26, 1.0);
}

vec3 computeTemperedGlassNormal(vec3 N, vec3 worldPos, vec2 paneUv, uint glassType) {
    vec3 upRef = (abs(N.y) < 0.92) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 T = normalize(cross(upRef, N));
    vec3 B = cross(N, T);
    vec2 pCoord = vec2(dot(worldPos, T), worldPos.y) * 0.45 + paneUv;
    float wX = sin(pCoord.x * 12.56637 + sin(pCoord.y * 6.28318) * 0.6) * 0.024;
    float wY = cos(pCoord.y * 12.56637 + cos(pCoord.x * 6.28318) * 0.5) * 0.028;
    if (glassType == 5u) {
        float grain = buildingHash21(floor(pCoord * 64.0)) - 0.5;
        wX += grain * 0.14;
        wY += (buildingHash21(floor(pCoord * 64.0) + 17.3) - 0.5) * 0.14;
    }
    return normalize(N + T * wX + B * wY);
}

vec3 sampleArchitecturalSkylineEnv(vec3 R, vec3 sunDir) {
    vec3 rDir = normalize(R);
    vec3 lDir = normalize(sunDir);
    float ry = rDir.y;

    // Horizon skyline silhouette height profile
    float azimuth = atan(rDir.z, rDir.x);
    float blockIdx = floor((azimuth + 3.14159265) * 5.0);
    float towerH = 0.02 + 0.11 * buildingHash21(vec2(blockIdx, 3.7))
                        + 0.05 * step(0.65, buildingHash21(vec2(blockIdx, 9.1)));

    vec3 skyZenith = vec3(0.11, 0.30, 0.62);
    vec3 skyHorizon = vec3(0.56, 0.76, 0.94);
    vec3 cloudWarm = vec3(0.95, 0.96, 0.99);

    vec3 envColor;
    if (ry >= towerH) {
        float hT = pow(clamp((ry - towerH) / (1.0 - towerH), 0.0, 1.0), 0.55);
        envColor = mix(skyHorizon, skyZenith, hT);
        float cloudWave = smoothstep(0.35, 0.85,
            0.5 + 0.35 * sin(azimuth * 3.0 + ry * 9.0) + 0.25 * cos(azimuth * 7.0 - ry * 14.0));
        envColor = mix(envColor, cloudWarm, cloudWave * 0.32 * (1.0 - hT * 0.6));
    } else if (ry >= -0.04) {
        // Distant architectural towers reflected near horizon
        vec3 skylineCol = vec3(0.08, 0.13, 0.21);
        float winGrid = step(0.45, fract(azimuth * 22.0)) * step(0.42, fract(ry * 48.0));
        envColor = skylineCol + vec3(0.14, 0.19, 0.26) * winGrid;
    } else {
        float gT = clamp((-ry - 0.04) / 0.96, 0.0, 1.0);
        envColor = mix(vec3(0.10, 0.14, 0.20), vec3(0.04, 0.06, 0.10), gT);
    }

    float rDotL = max(dot(rDir, lDir), 0.0);
    float sunHalo = pow(rDotL, 28.0) * 0.45 + pow(rDotL, 196.0) * 1.15;
    envColor += vec3(1.0, 0.95, 0.84) * sunHalo;
    return clamp(envColor, 0.0, 1.5);
}

vec3 evaluateRefractiveArchitecturalGlassPBR(
    uint glassType,
    vec3 worldPos,
    vec3 N,
    vec3 V,
    vec3 sunDir,
    vec2 paneUv,
    float windowIor,
    out float outAlpha
) {
    vec3 Ng = computeTemperedGlassNormal(N, worldPos, paneUv, glassType);
    float nDotV = clamp(abs(dot(Ng, normalize(V))), 0.02, 1.0);
    float fresnel = computeSchlickFresnelIor(nDotV, windowIor, glassType);
    outAlpha = computeGlassPrivacyOpacity(windowIor, fresnel, glassType);

    vec3 baseTint = computeGlassBaseTint(glassType);
    // Refracted interior absorption ray (dark non-see-through parallax depth inside shadow box)
    vec3 refractRay = refract(-normalize(V), Ng, 1.0 / max(windowIor, 1.02));
    float shadowBoxDepth = 0.07 + 0.08 * clamp(0.5 + 0.5 * refractRay.y, 0.0, 1.0);
    float slatPattern = 0.88 + 0.12 * sin(worldPos.y * 14.0 + refractRay.y * 6.0);
    vec3 refractedInterior = baseTint * shadowBoxDepth * slatPattern;
    if (glassType == 5u) {
        refractedInterior = baseTint * (0.42 + 0.14 * max(dot(Ng, normalize(sunDir)), 0.0));
    }

    // Reflected environmental skyline + sun specular
    vec3 R = reflect(-normalize(V), Ng);
    vec3 envRefl = sampleArchitecturalSkylineEnv(R, sunDir);
    vec3 metalReflTint = mix(vec3(1.0), baseTint * 1.25 + vec3(0.25), clamp((windowIor - 1.3) / 1.5, 0.0, 0.65));
    if (glassType == 6u) metalReflTint = vec3(1.0, 0.84, 0.42);
    else if (glassType == 2u) metalReflTint = vec3(0.92, 0.96, 1.0);

    vec3 glassColor = mix(refractedInterior, envRefl * metalReflTint, fresnel);
    float edgeSeal = smoothstep(0.0, 0.06, min(min(paneUv.x, 1.0 - paneUv.x), min(paneUv.y, 1.0 - paneUv.y)));
    return glassColor * (0.82 + 0.18 * edgeSeal);
}

vec3 computeArchitecturalGlassTint(uint glassType, vec3 viewDir, vec3 normal) {
    float dummyAlpha = 1.0;
    return evaluateRefractiveArchitecturalGlassPBR(
        glassType,
        vec3(0.0),
        normal,
        viewDir,
        normalize(vec3(0.45, 0.72, 0.52)),
        vec2(0.5),
        1.85,
        dummyAlpha
    );
}

#endif // BUILDING_COMMON_GLSL
