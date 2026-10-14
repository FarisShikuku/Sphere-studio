#pragma once
#include <cstdint>

namespace sphere {

enum CollisionGroup : uint32_t {
    CollisionGroup_None       = 0,
    CollisionGroup_Default    = 1 << 0,
    CollisionGroup_Static     = 1 << 1,
    CollisionGroup_Terrain    = 1 << 2,
    CollisionGroup_Water      = 1 << 3,
    CollisionGroup_Road       = 1 << 4,
    CollisionGroup_Vegetation = 1 << 5,
    CollisionGroup_Vehicle    = 1 << 6,
    CollisionGroup_Sensor     = 1 << 7,
    CollisionGroup_All        = 0xFFFFFFFF
};

class CollisionFilter {
public:
    static bool CanCollide(uint32_t groupA, uint32_t maskA, uint32_t groupB, uint32_t maskB);
    static const char* GetGroupName(CollisionGroup group);
    static uint32_t GetDefaultMask(CollisionGroup group);
};

} // namespace sphere
