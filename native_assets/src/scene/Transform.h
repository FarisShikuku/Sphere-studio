#pragma once
#include <array>
#include <cmath>
#include <cstdint>
namespace sphere {
struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
    Vec2() = default;
    constexpr Vec2(float _x, float _y) : x(_x), y(_y) {}
    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }
};
struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    Vec3() = default;
    constexpr Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator-() const { return {-x, -y, -z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    Vec3 operator/(float s) const { float inv = 1.0f / s; return {x * inv, y * inv, z * inv}; }
    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    float LengthSq() const { return x * x + y * y + z * z; }
    float Length() const { return std::sqrt(LengthSq()); }
    Vec3 Normalized() const {
        float len = Length();
        return (len > 1e-6f) ? (*this / len) : Vec3{0, 0, 0};
    }
    static float Dot(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    static Vec3 Cross(const Vec3& a, const Vec3& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }
    static Vec3 Lerp(const Vec3& a, const Vec3& b, float t) {
        return a + (b - a) * t;
    }
};
struct Vec4 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;
    Vec4() = default;
    constexpr Vec4(float _x, float _y, float _z, float _w = 1.0f) : x(_x), y(_y), z(_z), w(_w) {}
    constexpr Vec4(const Vec3& v, float _w = 1.0f) : x(v.x), y(v.y), z(v.z), w(_w) {}
    Vec3 ToVec3() const { return {x, y, z}; }
    Vec4 operator*(float s) const { return {x * s, y * s, z * s, w}; }
    Vec4 operator+(const Vec4& o) const { return {x + o.x, y + o.y, z + o.z, w}; }
};
struct Quat {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;
    Quat() = default;
    constexpr Quat(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    static Quat Identity() { return {0.0f, 0.0f, 0.0f, 1.0f}; }
    static Quat FromAxisAngle(const Vec3& axis, float angleRadians) {
        float halfAngle = angleRadians * 0.5f;
        float s = std::sin(halfAngle);
        Vec3 normAxis = axis.Normalized();
        return {normAxis.x * s, normAxis.y * s, normAxis.z * s, std::cos(halfAngle)};
    }
    static Quat FromEuler(float pitchRad, float yawRad, float rollRad) {
        float cp = std::cos(pitchRad * 0.5f);
        float sp = std::sin(pitchRad * 0.5f);
        float cy = std::cos(yawRad * 0.5f);
        float sy = std::sin(yawRad * 0.5f);
        float cr = std::cos(rollRad * 0.5f);
        float sr = std::sin(rollRad * 0.5f);
        return {
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy,
            cr * cp * cy + sr * sp * sy
        };
    }
};
struct Mat4 {
    // Column-major array layout for Vulkan & SPIR-V std430 compatibility
    std::array<float, 16> m{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    static Mat4 Identity() {
        return Mat4{};
    }
    static Mat4 Translation(const Vec3& t) {
        Mat4 res = Identity();
        res.m[12] = t.x;
        res.m[13] = t.y;
        res.m[14] = t.z;
        return res;
    }
    static Mat4 Scaling(const Vec3& s) {
        Mat4 res = Identity();
        res.m[0] = s.x;
        res.m[5] = s.y;
        res.m[10] = s.z;
        return res;
    }
    static Mat4 RotationX(float angleRad) {
        Mat4 res = Identity();
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        res.m[5] = c;
        res.m[6] = s;
        res.m[9] = -s;
        res.m[10] = c;
        return res;
    }
    static Mat4 RotationY(float angleRad) {
        Mat4 res = Identity();
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        res.m[0] = c;
        res.m[2] = -s;
        res.m[8] = s;
        res.m[10] = c;
        return res;
    }
    static Mat4 RotationZ(float angleRad) {
        Mat4 res = Identity();
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        res.m[0] = c;
        res.m[1] = s;
        res.m[4] = -s;
        res.m[5] = c;
        return res;
    }
    static Mat4 FromQuat(const Quat& q) {
        Mat4 res = Identity();
        float xx = q.x * q.x;
        float yy = q.y * q.y;
        float zz = q.z * q.z;
        float xy = q.x * q.y;
        float xz = q.x * q.z;
        float yz = q.y * q.z;
        float wx = q.w * q.x;
        float wy = q.w * q.y;
        float wz = q.w * q.z;
        res.m[0] = 1.0f - 2.0f * (yy + zz);
        res.m[1] = 2.0f * (xy + wz);
        res.m[2] = 2.0f * (xz - wy);
        res.m[4] = 2.0f * (xy - wz);
        res.m[5] = 1.0f - 2.0f * (xx + zz);
        res.m[6] = 2.0f * (yz + wx);
        res.m[8] = 2.0f * (xz + wy);
        res.m[9] = 2.0f * (yz - wx);
        res.m[10] = 1.0f - 2.0f * (xx + yy);
        return res;
    }
    static Mat4 Multiply(const Mat4& a, const Mat4& b) {
        Mat4 res;
        for (int c = 0; c < 4; ++c) {
            for (int r = 0; r < 4; ++r) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += a.m[k * 4 + r] * b.m[c * 4 + k];
                }
                res.m[c * 4 + r] = sum;
            }
        }
        return res;
    }
    Mat4 operator*(const Mat4& b) const {
        return Multiply(*this, b);
    }
    static Mat4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar) {
        Mat4 res;
        std::fill(res.m.begin(), res.m.end(), 0.0f);
        res.m[0] = 2.0f / (right - left);
        res.m[5] = -2.0f / (top - bottom); // Vulkan inverted Y
        res.m[10] = -1.0f / (zFar - zNear); // Vulkan clip depth [0, 1]
        res.m[12] = -(right + left) / (right - left);
        res.m[13] = -(top + bottom) / (top - bottom);
        res.m[14] = -zNear / (zFar - zNear);
        res.m[15] = 1.0f;
        return res;
    }
    static Mat4 Perspective(float fovRadians, float aspect, float zNear, float zFar) {
        Mat4 res;
        std::fill(res.m.begin(), res.m.end(), 0.0f);
        float tanHalfFov = std::tan(fovRadians * 0.5f);
        res.m[0] = 1.0f / (aspect * tanHalfFov);
        res.m[5] = -1.0f / tanHalfFov; // Invert Y for Vulkan clip coordinate convention
        res.m[10] = zFar / (zNear - zFar);
        res.m[11] = -1.0f;
        res.m[14] = -(zFar * zNear) / (zFar - zNear);
        return res;
    }
    static Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
        Vec3 f = (target - eye).Normalized();
        Vec3 s = Vec3::Cross(f, up).Normalized();
        Vec3 u = Vec3::Cross(s, f);
        Mat4 res = Identity();
        res.m[0] = s.x;
        res.m[4] = s.y;
        res.m[8] = s.z;
        res.m[1] = u.x;
        res.m[5] = u.y;
        res.m[9] = u.z;
        res.m[2] = -f.x;
        res.m[6] = -f.y;
        res.m[10] = -f.z;
        res.m[12] = -Vec3::Dot(s, eye);
        res.m[13] = -Vec3::Dot(u, eye);
        res.m[14] = Vec3::Dot(f, eye);
        return res;
    }
    Vec4 TransformVector(const Vec4& v) const {
        return {
            m[0]*v.x + m[4]*v.y + m[8]*v.z  + m[12]*v.w,
            m[1]*v.x + m[5]*v.y + m[9]*v.z  + m[13]*v.w,
            m[2]*v.x + m[6]*v.y + m[10]*v.z + m[14]*v.w,
            m[3]*v.x + m[7]*v.y + m[11]*v.z + m[15]*v.w
        };
    }
    Vec3 TransformPoint(const Vec3& p) const {
        Vec4 res = TransformVector(Vec4(p, 1.0f));
        if (std::abs(res.w) > 1e-6f) {
            return {res.x / res.w, res.y / res.w, res.z / res.w};
        }
        return {res.x, res.y, res.z};
    }
};
class Transform {
public:
    Vec3 position{0.0f, 0.0f, 0.0f};
    Quat rotation{0.0f, 0.0f, 0.0f, 1.0f};
    Vec3 scale{1.0f, 1.0f, 1.0f};
    Transform() = default;
    Transform(const Vec3& pos, const Quat& rot = Quat::Identity(), const Vec3& sc = {1.0f, 1.0f, 1.0f})
        : position(pos), rotation(rot), scale(sc) {}
    Mat4 GetMatrix() const;
    void SetEulerRotation(float pitchDeg, float yawDeg, float rollDeg);
};
} // namespace sphere