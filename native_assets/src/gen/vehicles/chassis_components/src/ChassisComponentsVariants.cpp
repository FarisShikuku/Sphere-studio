#include "ChassisComponentsVariants.h"

namespace sphere::vehicles::chassis {

ChassisPartSpec GetChassisPartSpec(ChassisPartID id) {
    switch (id) {
        case ChassisPartID::FUEL_TANK:     return {id, "600L Brushed Aluminum Fuel Tank", 1.55f, 0.62f, 0.62f};
        case ChassisPartID::EXHAUST_STACK: return {id, "Dual Vertical Chrome Exhaust Stacks", 0.22f, 0.22f, 2.55f};
        case ChassisPartID::BATTERY_BOX:   return {id, "Sealed Twin-Battery & DEF AdBlue Pack", 0.72f, 0.48f, 0.42f};
        case ChassisPartID::ENGINE_BLOCK:  return {id, "12.8L Inline-6 Turbo-Diesel Block", 1.35f, 0.85f, 1.05f};
        case ChassisPartID::DRIVETRAIN:    return {id, "Propeller Shaft & Rear Differential", 3.60f, 0.44f, 0.44f};
        default:                           return {id, "Unknown Chassis Part", 1.0f, 0.5f, 0.5f};
    }
}

} // namespace sphere::vehicles::chassis
