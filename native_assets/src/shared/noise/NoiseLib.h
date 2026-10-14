#pragma once

namespace sphere {
class NoiseLib {
public:
    static float Perlin2D(float x, float y);
    static float Simplex3D(float x, float y, float z);
    static float Worley2D(float x, float y);
};
}\n