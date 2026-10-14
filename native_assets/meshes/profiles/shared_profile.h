#pragma once
#include <cstdint>

namespace sphere::meshes {
constexpr uint32_t MAX_PROFILE_POINTS = 64;
struct CrossSectionProfile {
    float uCoords[MAX_PROFILE_POINTS];
    float vCoords[MAX_PROFILE_POINTS];
    uint32_t count = 0;
};
}\n