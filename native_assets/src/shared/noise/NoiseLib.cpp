#include "NoiseLib.h"
#include <cmath>

namespace sphere {
float NoiseLib::Perlin2D(float x, float y) {
    return std::sin(x) * std::cos(y);
}

float NoiseLib::Simplex3D(float x, float y, float z) {
    return std::sin(x + y + z);
}

float NoiseLib::Worley2D(float x, float y) {
    return std::fmod(std::abs(x * y), 1.0f);
}
}\n