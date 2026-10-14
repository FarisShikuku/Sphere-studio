#pragma once
#include "scene/Transform.h"
#include <vector>

namespace sphere {

class SplineLib {
public:
    static Vec3 CatmullRom(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t);
    static Vec3 CubicBezier(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t);
    static Vec3 EvaluateTangent(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t);
    static std::vector<Vec3> SampleEquidistant(const std::vector<Vec3>& controlPoints, float stepDistance);
};

} // namespace sphere
