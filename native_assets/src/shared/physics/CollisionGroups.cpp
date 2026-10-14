#include "CollisionGroups.h"

namespace sphere {

bool CollisionFilter::CanCollide(uint32_t groupA, uint32_t maskA, uint32_t groupB, uint32_t maskB) {
    return (groupA & maskB) != 0 && (groupB & maskA) != 0;
}

const char* CollisionFilter::GetGroupName(CollisionGroup group) {
    switch (group) {
        case CollisionGroup_None:       return "None";
        case CollisionGroup_Default:    return "Default";
        case CollisionGroup_Static:     return "Static";
        case CollisionGroup_Terrain:    return "Terrain";
        case CollisionGroup_Water:      return "Water";
        case CollisionGroup_Road:       return "Road";
        case CollisionGroup_Vegetation: return "Vegetation";
        case CollisionGroup_Vehicle:    return "Vehicle";
        case CollisionGroup_Sensor:     return "Sensor";
        case CollisionGroup_All:        return "All";
        default:                        return "Custom";
    }
}

uint32_t CollisionFilter::GetDefaultMask(CollisionGroup group) {
    switch (group) {
        case CollisionGroup_Water:
            // Water collides with terrain, vehicles, and sensors, but does not block default raycasts
            return CollisionGroup_Terrain | CollisionGroup_Vehicle | CollisionGroup_Sensor;
        case CollisionGroup_Vegetation:
            // Foliage interacts with vehicles and sensors, permits camera pass-through
            return CollisionGroup_Vehicle | CollisionGroup_Sensor;
        case CollisionGroup_Sensor:
            return CollisionGroup_All;
        default:
            return CollisionGroup_All;
    }
}

} // namespace sphere
