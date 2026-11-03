#include "FrustumCull.h"
#include <cmath>
namespace sphere {
std::array<Plane, 6> FrustumCull::ExtractPlanes(const Mat4& vp) {
    std::array<Plane, 6> planes;
    const auto& m = vp.m;
    // Left plane
    planes[0].normal = { m[3] + m[0], m[7] + m[4], m[11] + m[8] };
    planes[0].distance = m[15] + m[12];
    // Right plane
    planes[1].normal = { m[3] - m[0], m[7] - m[4], m[11] - m[8] };
    planes[1].distance = m[15] - m[12];
    // Bottom plane
    planes[2].normal = { m[3] + m[1], m[7] + m[5], m[11] + m[9] };
    planes[2].distance = m[15] + m[13];
    // Top plane
    planes[3].normal = { m[3] - m[1], m[7] - m[5], m[11] - m[9] };
    planes[3].distance = m[15] - m[13];
    // Near plane
    planes[4].normal = { m[2], m[6], m[10] };
    planes[4].distance = m[14];
    // Far plane
    planes[5].normal = { m[3] - m[2], m[7] - m[6], m[11] - m[10] };
    planes[5].distance = m[15] - m[14];
    // Normalize planes
    for (auto& plane : planes) {
        float len = std::sqrt(plane.normal.x * plane.normal.x +
                              plane.normal.y * plane.normal.y +
                              plane.normal.z * plane.normal.z);
        if (len > 1e-6f) {
            plane.normal.x /= len;
            plane.normal.y /= len;
            plane.normal.z /= len;
            plane.distance /= len;
        }
    }
    return planes;
}
bool FrustumCull::IsAABBInside(const AABB& aabb, const std::array<Plane, 6>& planes) {
    for (const auto& plane : planes) {
        Vec3 positivePoint = {
            plane.normal.x > 0 ? aabb.max.x : aabb.min.x,
            plane.normal.y > 0 ? aabb.max.y : aabb.min.y,
            plane.normal.z > 0 ? aabb.max.z : aabb.min.z
        };
        if (plane.DistanceToPoint(positivePoint) < 0.0f) {
            return false;
        }
    }
    return true;
}
bool FrustumCull::IsSphereInside(const Vec3& center, float radius, const std::array<Plane, 6>& planes) {
    for (const auto& plane : planes) {
        if (plane.DistanceToPoint(center) < -radius) {
            return false;
        }
    }
    return true;
}
} // namespace spher
