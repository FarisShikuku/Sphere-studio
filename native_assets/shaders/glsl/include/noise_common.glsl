#ifndef NOISE_COMMON_GLSL
#define NOISE_COMMON_GLSL

// Hash function
float Hash(vec2 p) {
    p = 50.0 * fract(p * 0.3183099 + vec2(0.71, 0.113));
    return -1.0 + 2.0 * fract(p.x * p.y * (p.x + p.y));
}

float Hash3D(vec3 p) {
    p = fract(p * 0.3183099 + vec3(0.1, 0.2, 0.3));
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

// 2D Value Noise
float ValueNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(mix(Hash(i + vec2(0.0, 0.0)), Hash(i + vec2(1.0, 0.0)), u.x),
               mix(Hash(i + vec2(0.0, 1.0)), Hash(i + vec2(1.0, 1.0)), u.x), u.y);
}

// 2D Fractional Brownian Motion (FBM)
float FBM2D(vec2 p, int octaves, float lacunarity, float gain) {
    float total = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    for (int i = 0; i < octaves; ++i) {
        total += amplitude * ValueNoise(p * frequency);
        frequency *= lacunarity;
        amplitude *= gain;
    }
    return total;
}

// Worley / Cellular Noise
float Worley2D(vec2 p) {
    vec2 n = floor(p);
    vec2 f = fract(p);
    float minDist = 1.0;
    for (int y = -1; y <= 1; ++y) {
        for (int x = -1; x <= 1; ++x) {
            vec2 g = vec2(float(x), float(y));
            vec2 o = vec2(Hash(n + g), Hash(n + g + 13.5));
            vec2 delta = g + o - f;
            minDist = min(minDist, dot(delta, delta));
        }
    }
    return sqrt(minDist);
}

#endif // NOISE_COMMON_GLSL
