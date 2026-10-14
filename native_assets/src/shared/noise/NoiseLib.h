#pragma once
#include <cstdint>

namespace sphere {

class NoiseLib {
public:
    static float Perlin2D(float x, float y);
    static float Perlin3D(float x, float y, float z);
    static float Simplex2D(float x, float y);
    static float Simplex3D(float x, float y, float z);
    static float Worley2D(float x, float y);
    static float FBM(float x, float y, int octaves = 4, float lacunarity = 2.0f, float gain = 0.5f);
};

} // namespace sphere
