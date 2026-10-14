#pragma once
#include <array>

namespace sphere {
struct Vec3 { float x = 0, y = 0, z = 0; };
struct Vec4 { float x = 0, y = 0, z = 0, w = 1; };
struct Quat { float x = 0, y = 0, z = 0, w = 1; };
struct Mat4 { std::array<float, 16> m{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}; };

class Transform {
public:
    Vec3 position{0.0f, 0.0f, 0.0f};
    Quat rotation{0.0f, 0.0f, 0.0f, 1.0f};
    Vec3 scale{1.0f, 1.0f, 1.0f};

    Mat4 GetMatrix() const;
};
}\n