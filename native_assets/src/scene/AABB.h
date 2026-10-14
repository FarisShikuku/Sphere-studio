#pragma once
#include "Transform.h"

namespace sphere {
struct AABB {
    Vec3 min{-1.0f, -1.0f, -1.0f};
    Vec3 max{ 1.0f,  1.0f,  1.0f};

    bool Contains(const Vec3& p) const {
        return p.x >= min.x && p.x <= max.x &&
               p.y >= min.y && p.y <= max.y &&
               p.z >= min.z && p.z <= max.z;
    }
};
}\n