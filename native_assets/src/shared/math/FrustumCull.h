#pragma once
#include "scene/AABB.h"
#include "scene/Transform.h"
#include <array>
namespace sphere {
struct Plane {
    Vec3 normal{0.0f, 1.0f, 0.0f};
    float distance = 0.0f;
    float DistanceToPoint(const Vec3& p) const {
        return normal.x * p.x + normal.y * p.y + normal.z * p.z + distance;
    }
};
class FrustumCull {
public:
    static std::array<Plane, 6> ExtractPlanes(const Mat4& viewProjection);
    static bool IsAABBInside(const AABB& aabb, const std::array<Plane, 6>& planes);
    static bool IsSphereInside(const Vec3& center, float radius, const std::array<Plane, 6>& planes);
};
} // namespace spher
