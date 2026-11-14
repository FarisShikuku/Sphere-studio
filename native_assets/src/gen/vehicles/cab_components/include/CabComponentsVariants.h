#pragma once
#include "VehicleDef.h"

namespace sphere::vehicles::cab {

enum class CabPartID : uint32_t {
    DOOR_LEFT = 0,
    DOOR_RIGHT,
    WINDSHIELD,
    GRILLE_HEAVY,
    MIRROR_TRUCK,
    COUNT
};

struct CabPartSpec {
    CabPartID id;
    const char* name;
    float width_m;
    float height_m;
    float thickness_m;
};

CabPartSpec GetCabPartSpec(CabPartID id);

} // namespace sphere::vehicles::cab
