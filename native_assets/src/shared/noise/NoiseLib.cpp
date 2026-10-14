#include "NoiseLib.h"
#include <cmath>

namespace sphere {

static inline float Hash(int x, int y) {
    int n = x + y * 57;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float NoiseLib::Perlin2D(float x, float y) {
    int ix = (int)std::floor(x);
    int iy = (int)std::floor(y);
    float fx = x - ix;
    float fy = y - iy;

    float s = Hash(ix, iy);
    float t = Hash(ix + 1, iy);
    float u = Hash(ix, iy + 1);
    float v = Hash(ix + 1, iy + 1);

    float cx = fx * fx * (3.0f - 2.0f * fx);
    float cy = fy * fy * (3.0f - 2.0f * fy);

    float a = s * (1.0f - cx) + t * cx;
    float b = u * (1.0f - cx) + v * cx;
    return a * (1.0f - cy) + b * cy;
}

float NoiseLib::Perlin3D(float x, float y, float z) {
    return (Perlin2D(x, y) + Perlin2D(y, z) + Perlin2D(z, x)) / 3.0f;
}

float NoiseLib::FBM(float x, float y, int octaves, float lacunarity, float gain) {
    float total = 0.0f;
    float amplitude = 0.5f;
    float frequency = 1.0f;
    for (int i = 0; i < octaves; ++i) {
        total += amplitude * Perlin2D(x * frequency, y * frequency);
        frequency *= lacunarity;
        amplitude *= gain;
    }
    return total;
}

} // namespace sphere
