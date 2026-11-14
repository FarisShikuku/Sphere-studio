#include "TrailerID.h"

namespace sphere::trailers {

const char* GetTrailerCategoryName(TrailerCategory cat) {
    switch (cat) {
        case TrailerCategory::FLATBED:       return "Flatbeds";
        case TrailerCategory::TANKER:        return "Tankers";
        case TrailerCategory::BOX_VAN:       return "Box Vans";
        case TrailerCategory::CHASSIS:       return "Container Chassis";
        case TrailerCategory::LOGGING:       return "Logging";
        case TrailerCategory::CURTAINSIDER:  return "Curtainsiders";
        case TrailerCategory::BULK_HOPPER:   return "Bulk Hoppers";
        case TrailerCategory::CAR_CARRIER:   return "Car Carriers";
        case TrailerCategory::HEAVY_HAULAGE: return "Heavy Haulage";
        case TrailerCategory::DOLLY:         return "Dollies";
        case TrailerCategory::RECREATIONAL:  return "Recreational";
        case TrailerCategory::SPECIALTY:     return "Specialty";
        default:                             return "Unknown Trailer Category";
    }
}

TrailerCategory GetTrailerCategoryEnum(TrailerID id) {
    uint32_t v = static_cast<uint32_t>(id);
    if (v <= static_cast<uint32_t>(TrailerID::FLATBED_SIDE_LOADER)) return TrailerCategory::FLATBED;
    if (v <= static_cast<uint32_t>(TrailerID::TANKER_POWDER)) return TrailerCategory::TANKER;
    if (v <= static_cast<uint32_t>(TrailerID::BOX_DOUBLE_DECK)) return TrailerCategory::BOX_VAN;
    if (v <= static_cast<uint32_t>(TrailerID::CHASSIS_TIPPING)) return TrailerCategory::CHASSIS;
    if (v <= static_cast<uint32_t>(TrailerID::LOGGING_EXTENDABLE_LOG)) return TrailerCategory::LOGGING;
    if (v <= static_cast<uint32_t>(TrailerID::CURTAINSIDER_SIDE_LOADING)) return TrailerCategory::CURTAINSIDER;
    if (v <= static_cast<uint32_t>(TrailerID::HOPPER_SIDE_TIPPER)) return TrailerCategory::BULK_HOPPER;
    if (v <= static_cast<uint32_t>(TrailerID::CARCARRIER_ENCLOSED)) return TrailerCategory::CAR_CARRIER;
    if (v <= static_cast<uint32_t>(TrailerID::HEAVY_TUNNEL_TRANSPORTER)) return TrailerCategory::HEAVY_HAULAGE;
    if (v <= static_cast<uint32_t>(TrailerID::DOLLY_JEEP)) return TrailerCategory::DOLLY;
    if (v <= static_cast<uint32_t>(TrailerID::RECREATION_UTILITY_TRAILER)) return TrailerCategory::RECREATIONAL;
    return TrailerCategory::SPECIALTY;
}

const char* GetTrailerCategory(TrailerID id) {
    return GetTrailerCategoryName(GetTrailerCategoryEnum(id));
}

const char* GetTrailerName(TrailerID id) {
    switch (id) {
        case TrailerID::FLATBED_STD:                   return "Standard 40ft Flatbed";
        case TrailerID::FLATBED_EXTENDABLE:            return "Extendable Telescopic Flatbed";
        case TrailerID::FLATBED_STEP_DECK:             return "Step-Deck Drop Frame Flatbed";
        case TrailerID::FLATBED_DROP_DECK:             return "Deep Drop-Deck Flatbed";
        case TrailerID::FLATBED_SIDE_LOADER:           return "Self-Loading Crane Sideloader";

        case TrailerID::TANKER_WATER:                  return "Potable Water Tanker 30kL";
        case TrailerID::TANKER_FUEL_MULTI_COMPARTMENT: return "Multi-Compartment Fuel Tanker";
        case TrailerID::TANKER_MILK:                   return "Insulated Stainless Milk Tanker";
        case TrailerID::TANKER_CHEMICAL:               return "Corrosive Acid Chemical Tanker";
        case TrailerID::TANKER_LPG_GAS:                return "Pressurized LPG Gas Tanker";
        case TrailerID::TANKER_POWDER:                 return "Pressurized Dry Powder Tanker";

        case TrailerID::DRY_VAN_53FT:                  return "53ft Dry Freight Box Van";
        case TrailerID::BOX_REEFER:                    return "Refrigerated Reefer Van";
        case TrailerID::BOX_PARCEL_VAN:                return "Double-Stack Parcel Express Van";
        case TrailerID::BOX_MOVING_VAN:                return "Drop-Frame Household Moving Van";
        case TrailerID::BOX_DOUBLE_DECK:               return "Double-Deck High-Cube Van";

        case TrailerID::CHASSIS_20FT:                  return "20ft ISO Container Chassis";
        case TrailerID::CHASSIS_40FT:                  return "40ft Tri-Axle Container Chassis";
        case TrailerID::CHASSIS_GOOSENECK:             return "Gooseneck High-Cube Chassis";
        case TrailerID::CHASSIS_SLIDER:                return "Extendable Slider Container Chassis";
        case TrailerID::CHASSIS_TIPPING:               return "Hydraulic Tipping Container Chassis";

        case TrailerID::LOGGING_POLE_TRAILER:          return "Steerable Forestry Pole Trailer";
        case TrailerID::LOGGING_BUNK_TRAILER:          return "Heavy Bolster Logging Bunk Trailer";
        case TrailerID::LOGGING_EXTENDABLE_LOG:        return "Telescopic Long-Log Timber Trailer";

        case TrailerID::CURTAINSIDER_STANDARD:         return "Standard EU Tautliner Curtainsider";
        case TrailerID::CURTAINSIDER_XL:               return "EN 12642-XL Certified Curtainsider";
        case TrailerID::CURTAINSIDER_DOUBLE_DECK:      return "Double-Deck Lifting Curtainsider";
        case TrailerID::CURTAINSIDER_SIDE_LOADING:     return "Mega-Cube Side-Loading Curtainsider";

        case TrailerID::HOPPER_GRAIN:                  return "Twin-Hopper Grain Bulker";
        case TrailerID::HOPPER_CEMENT_POWDER:          return "Pneumatic Cement Silo Hopper";
        case TrailerID::HOPPER_AGGREGATE_TIPPER:       return "End-Dump Aggregate Tipper";
        case TrailerID::HOPPER_SIDE_TIPPER:            return "Quarry Side-Tipper Tub";

        case TrailerID::CARCARRIER_SINGLE_DECK:        return "Single-Deck 3-Car Transporter";
        case TrailerID::CARCARRIER_DOUBLE_DECK:        return "Hydraulic 8-Car Double-Deck Stinger";
        case TrailerID::CARCARRIER_OPEN_TRANSPORTER:   return "Open Steel Track Car Hauler";
        case TrailerID::CARCARRIER_ENCLOSED:           return "Enclosed Exotic Motorsport Carrier";

        case TrailerID::HEAVY_LOWBOY:                  return "Detachable Gooseneck 60T Lowboy";
        case TrailerID::HEAVY_MULTI_AXLE_MODULAR:      return "12-Axle Modular Heavy Platform";
        case TrailerID::HEAVY_BLADE_TRANSPORTER:       return "Wind Turbine Blade Lifter Trailer";
        case TrailerID::HEAVY_TUNNEL_TRANSPORTER:      return "TBM Shield Tunnel Segment Hauler";

        case TrailerID::DOLLY_CONVERTER:               return "Twin-Axle Road-Train Converter Dolly";
        case TrailerID::DOLLY_A_DOLLY:                 return "Single-Axle A-Frame Converter Dolly";
        case TrailerID::DOLLY_JEEP:                    return "Heavy Haulage Tandem Jeep Dolly";

        case TrailerID::RECREATION_BOAT_TRAILER:       return "Galvanized Keel-Roller Boat Trailer";
        case TrailerID::RECREATION_ATV_TRAILER:        return "Dual-Quad Utility ATV Mesh Trailer";
        case TrailerID::RECREATION_CAMPER_TRAILER:     return "Expedition Off-Road Camper Trailer";
        case TrailerID::RECREATION_UTILITY_TRAILER:    return "Tandem Box General Utility Trailer";

        case TrailerID::SPECIALTY_LIVESTOCK:           return "Triple-Deck Ventilated Livestock Hauler";
        case TrailerID::SPECIALTY_WASTE_TANKER:        return "Municipal Vacuum Waste Tanker";
        case TrailerID::SPECIALTY_MILITARY_TRANSPORT:  return "Heavy Tactical Tank Transporter";
        case TrailerID::SPECIALTY_ARMORED_CAR:         return "Armored Escort Security Trailer";

        default:                                       return "Unknown Procedural Trailer";
    }
}

} // namespace sphere::trailers
