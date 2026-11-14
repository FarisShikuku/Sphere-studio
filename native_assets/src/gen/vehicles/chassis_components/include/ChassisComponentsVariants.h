#pragma once
#include "VehicleDef.h"

namespace sphere::vehicles::chassis {

enum class ChassisPartID : uint32_t {
    FUEL_TANK = 0,
    EXHAUST_STACK,
    BATTERY_BOX,
    ENGINE_BLOCK,
    DRIVETRAIN,
    COUNT
};

struct ChassisPartSpec {
    ChassisPartID id;
    const char* name;
    float length_m;
    float width_m;
    float height_m;
};

ChassisPartSpec GetChassisPartSpec(ChassisPartID id);

} // namespace sphere::vehicles::chassis
