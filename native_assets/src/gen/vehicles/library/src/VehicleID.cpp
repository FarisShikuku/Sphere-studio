#include "VehicleID.h"

namespace sphere::vehicles {

const char* GetVehicleName(VehicleID id) {
    switch (id) {
        case VehicleID::LORRY_RIGID_BOX: return "Rigid Box Delivery Lorry (18T)";
        case VehicleID::LORRY_RIGID_FLATBED: return "Rigid Dropside Flatbed Lorry";
        case VehicleID::LORRY_RIGID_TIPPER: return "6x4 Heavy Quarry Tipper";
        case VehicleID::LORRY_RIGID_CEMENT_MIXER: return "8x4 Transit Cement Mixer";
        case VehicleID::LORRY_RIGID_TANKER: return "Rigid Fuel & Water Tanker (20KL)";
        case VehicleID::TRACTOR_CAB_OVER: return "Cab-Over-Engine 6x4 Tractor";
        case VehicleID::TRACTOR_CONVENTIONAL: return "Conventional Long-Nose Tractor";
        case VehicleID::TRACTOR_EURO_CAB: return "Euro Aero High-Roof Sleeper (4x2)";
        case VehicleID::TRACTOR_AMERICAN_CAB: return "American Condo Sleeper 6x4";
        case VehicleID::BUS_CITY_STANDARD: return "Standard 12m City Transit Bus";
        case VehicleID::BUS_CITY_DOUBLE_DECKER: return "Metropolitan Double-Decker Bus";
        case VehicleID::BUS_CITY_ARTICULATED_BENDY: return "18m Articulated Bendy Transit Bus";
        case VehicleID::BUS_CITY_LOW_FLOOR: return "Low-Floor Electric BRT Bus";
        case VehicleID::COACH_HIGH_DECKER: return "Intercity High-Decker Coach";
        case VehicleID::COACH_SLEEPER: return "Overnight Sleeper Express Coach";
        case VehicleID::COACH_TOURING: return "Panoramic Touring Coach";
        case VehicleID::COACH_LUXURY: return "Executive VIP Tri-Axle Coach";
        case VehicleID::MATATU_14_SEATER: return "Nairobi 14-Seater Matatu (Nganya)";
        case VehicleID::SPRINTER_VAN: return "High-Roof Sprinter Commuter Van";
        case VehicleID::SHUTTLE_BUS: return "Airport Terminal Shuttle Bus";
        case VehicleID::SCHOOL_VAN: return "Regional Yellow School Van";
        default: return "Unknown Vehicle";
    }
}

const char* GetVehicleCategory(VehicleID id) {
    uint32_t v = static_cast<uint32_t>(id);
    if (v <= static_cast<uint32_t>(VehicleID::LORRY_RIGID_TANKER)) return "Rigid Lorries";
    if (v <= static_cast<uint32_t>(VehicleID::TRACTOR_AMERICAN_CAB)) return "Articulated Tractors";
    if (v <= static_cast<uint32_t>(VehicleID::BUS_CITY_LOW_FLOOR)) return "City Buses";
    if (v <= static_cast<uint32_t>(VehicleID::COACH_LUXURY)) return "Intercity Coaches";
    return "Minibuses & Matatus";
}

} // namespace sphere::vehicles
