#pragma once
#include "scene/Transform.h"

namespace sphere {
class SplineLib {
public:
    static Vec3 CatmullRom(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t);
    static Vec3 CubicBezier(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t);
};
}\n