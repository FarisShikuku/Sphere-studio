#include "SplineLib.h"
#include <cmath>

namespace sphere {

Vec3 SplineLib::CatmullRom(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    Vec3 r;
    r.x = 0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * t + (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 + (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
    r.y = 0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * t + (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 + (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
    r.z = 0.5f * ((2.0f * p1.z) + (-p0.z + p2.z) * t + (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t2 + (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t3);
    return r;
}

Vec3 SplineLib::CubicBezier(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    Vec3 p;
    p.x = uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x;
    p.y = uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y;
    p.z = uuu * p0.z + 3 * uu * t * p1.z + 3 * u * tt * p2.z + ttt * p3.z;
    return p;
}

Vec3 SplineLib::EvaluateTangent(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t) {
    float t2 = t * t;
    Vec3 r;
    r.x = 0.5f * ((-p0.x + p2.x) + 2.0f * (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t + 3.0f * (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t2);
    r.y = 0.5f * ((-p0.y + p2.y) + 2.0f * (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t + 3.0f * (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t2);
    r.z = 0.5f * ((-p0.z + p2.z) + 2.0f * (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t + 3.0f * (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t2);

    float len = std::sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
    if (len > 1e-6f) { r.x /= len; r.y /= len; r.z /= len; }
    return r;
}

std::vector<Vec3> SplineLib::SampleEquidistant(const std::vector<Vec3>& controlPoints, float stepDistance) {
    std::vector<Vec3> samples;
    if (controlPoints.size() < 4) return samples;

    for (size_t i = 0; i + 3 < controlPoints.size(); ++i) {
        for (float t = 0.0f; t < 1.0f; t += 0.05f) {
            samples.push_back(CatmullRom(controlPoints[i], controlPoints[i+1], controlPoints[i+2], controlPoints[i+3], t));
        }
    }
    return samples;
}

} // namespace sphere
