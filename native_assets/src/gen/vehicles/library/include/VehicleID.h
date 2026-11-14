#pragma once
#include <cstdint>

namespace sphere::vehicles {

enum class VehicleID : uint32_t {
    // 1. Rigid Lorries (5)
    LORRY_RIGID_BOX = 0,
    LORRY_RIGID_FLATBED,
    LORRY_RIGID_TIPPER,
    LORRY_RIGID_CEMENT_MIXER,
    LORRY_RIGID_TANKER,

    // 2. Articulated Tractor Units (4)
    TRACTOR_CAB_OVER,
    TRACTOR_CONVENTIONAL,
    TRACTOR_EURO_CAB,
    TRACTOR_AMERICAN_CAB,

    // 3. City Transit Buses (4)
    BUS_CITY_STANDARD,
    BUS_CITY_DOUBLE_DECKER,
    BUS_CITY_ARTICULATED_BENDY,
    BUS_CITY_LOW_FLOOR,

    // 4. Intercity Coaches (4)
    COACH_HIGH_DECKER,
    COACH_SLEEPER,
    COACH_TOURING,
    COACH_LUXURY,

    // 5. Minibuses & Matatus (4)
    MATATU_14_SEATER,
    SPRINTER_VAN,
    SHUTTLE_BUS,
    SCHOOL_VAN,

    COUNT
};

const char* GetVehicleName(VehicleID id);
const char* GetVehicleCategory(VehicleID id);

} // namespace sphere::vehicles
