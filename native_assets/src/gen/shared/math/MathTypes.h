#pragma once

#include <cmath>
#include <cstdint>

namespace procedural {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    constexpr Vec2() = default;
    constexpr Vec2(float inX, float inY) : x(inX), y(inY) {}

    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator-() const { return {-x, -y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }
    Vec2 operator/(float s) const {
        float inv = (std::abs(s) > 1e-8f) ? (1.0f / s) : 0.0f;
        return {x * inv, y * inv};
    }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    Vec2& operator*=(float s) { x *= s; y *= s; return *this; }

    float lengthSq() const { return x * x + y * y; }
    float length() const { return std::sqrt(lengthSq()); }
    float dot(const Vec2& o) const { return x * o.x + y * o.y; }
    Vec2 normalized() const {
        float l = length();
        return (l > 1e-6f) ? Vec2{x / l, y / l} : Vec2{0.0f, 0.0f};
    }
};

inline Vec2 operator*(float s, const Vec2& v) { return {v.x * s, v.y * s}; }

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    constexpr Vec3() = default;
    constexpr Vec3(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ) {}

    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator-() const { return {-x, -y, -z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    Vec3 operator/(float s) const {
        float inv = (std::abs(s) > 1e-8f) ? (1.0f / s) : 0.0f;
        return {x * inv, y * inv, z * inv};
    }
    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }

    float lengthSq() const { return x * x + y * y + z * z; }
    float length() const { return std::sqrt(lengthSq()); }
    float dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }
    Vec3 cross(const Vec3& o) const {
        return {
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x
        };
    }
    Vec3 normalized() const {
        float l = length();
        return (l > 1e-6f) ? Vec3{x / l, y / l, z / l} : Vec3{0.0f, 1.0f, 0.0f};
    }
};

inline Vec3 operator*(float s, const Vec3& v) { return {v.x * s, v.y * s, v.z * s}; }

struct Vec4 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    constexpr Vec4() = default;
    constexpr Vec4(float inX, float inY, float inZ, float inW = 1.0f)
        : x(inX), y(inY), z(inZ), w(inW) {}
};

namespace buildings {
using procedural::Vec2;
using procedural::Vec3;
using procedural::Vec4;
} // namespace buildings

} // namespace procedural
