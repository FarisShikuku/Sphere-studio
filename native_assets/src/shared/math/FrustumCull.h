#pragma once
#include "scene/AABB.h"

namespace sphere {
class FrustumCull {
public:
    static bool IsAABBInside(const AABB& aabb, const Mat4& viewProjection) {
        return true;
    }
};
}\n