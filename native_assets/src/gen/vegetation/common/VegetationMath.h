#pragma once
// ============================================================================
// VegetationMath.h - Native C++17 & C Vector Math, Noise & Geometric Utilities
// 100% C++17 / C implementation for botanical simulation and procedural synthesis.
// ============================================================================

#include <cmath>
#include <cstdint>
#include <vector>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

// Plain C 3D vector for C API compatibility
typedef struct VegVec3_C {
    float x;
    float y;
    float z;
} VegVec3_C;

typedef struct VegVec2_C {
    float u;
    float v;
} VegVec2_C;

typedef struct VegColor_C {
    float r;
    float g;
    float b;
    float a;
} VegColor_C;

typedef struct VegVertex_C {
    VegVec3_C position;
    VegVec3_C normal;
    VegVec2_C texCoord;
    VegColor_C color;
    float windWeight;
} VegVertex_C;

// Pure C deterministic 3D noise functions
float veg_noise3d(float x, float y, float z);
float veg_fbm3d(float x, float y, float z, int octaves, float lacunarity, float gain);
float veg_voronoi3d(float x, float y, float z, float* outCellX, float* outCellY, float* outCellZ);

#ifdef __cplusplus
} // extern "C"
#endif

#ifndef ENGINE_CORE_ASSERT
#define ENGINE_CORE_ASSERT(cond, ...) ((void)0)
#endif

using Vec3_C = VegVec3_C;

namespace vegetation {

struct Vec2 {
    float x{0.0f}, y{0.0f};
    constexpr Vec2() = default;
    constexpr Vec2(float x_, float y_) : x(x_), y(y_) {}
};

struct Vec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};

    constexpr Vec3() = default;
    constexpr Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    Vec3 operator/(float s) const { float inv = 1.0f / s; return {x * inv, y * inv, z * inv}; }
    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }

    float lengthSq() const { return x * x + y * y + z * z; }
    float length() const { return std::sqrt(lengthSq()); }

    Vec3 normalized() const {
        float l = length();
        return (l > 1e-6f) ? (*this * (1.0f / l)) : Vec3(0, 1, 0);
    }

    VegVec3_C toC() const { return {x, y, z}; }

    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }
    static float dot(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    static Vec3 lerp(const Vec3& a, const Vec3& b, float t) {
        return a + (b - a) * t;
    }
};

inline float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

inline Vec3 lerp(const Vec3& a, const Vec3& b, float t) {
    return a + (b - a) * t;
}

inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

struct Mat3 {
    Vec3 col0{1, 0, 0};
    Vec3 col1{0, 1, 0};
    Vec3 col2{0, 0, 1};

    Vec3 transform(const Vec3& v) const {
        return col0 * v.x + col1 * v.y + col2 * v.z;
    }

    static Mat3 fromEuler(float pitchRad, float yawRad, float rollRad);
    static Mat3 fromAxisAngle(const Vec3& axis, float angleRad);
};

// Deterministic Pseudo-Random Number Generator (PCG32)
class FastRng {
public:
    explicit FastRng(uint64_t seed = 0x853c49e6748fea9bULL) {
        state = seed + 0xda3e39cb94b95bdbULL;
        inc = (seed << 1u) | 1u;
        nextUint();
    }

    uint32_t nextUint() {
        uint64_t oldstate = state;
        state = oldstate * 6364136223846793005ULL + inc;
        uint32_t xorshifted = static_cast<uint32_t>(((oldstate >> 18u) ^ oldstate) >> 27u);
        uint32_t rot = static_cast<uint32_t>(oldstate >> 59u);
        return (xorshifted >> rot) | (xorshifted << ((-static_cast<int>(rot)) & 31));
    }

    float nextFloat01() {
        return (nextUint() & 0x00FFFFFF) * (1.0f / 16777216.0f);
    }

    float nextRange(float minVal, float maxVal) {
        return minVal + nextFloat01() * (maxVal - minVal);
    }

private:
    uint64_t state{0};
    uint64_t inc{1};
};

} // namespace vegetation
