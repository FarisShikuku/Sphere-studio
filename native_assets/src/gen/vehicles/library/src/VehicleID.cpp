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
        case VehicleID::COACH_HIGH_DECKER: return "Volvo UNVI High-Decker Coach";
        case VehicleID::COACH_SLEEPER: return "Volvo 9900 Theater-Floor 6x2 Coach";
        case VehicleID::COACH_TOURING: return "Volvo BZR Electric Tri-Axle Coach";
        case VehicleID::COACH_LUXURY: return "Volvo 9700 US/CAN Aero 6x2 Coach";
        case VehicleID::MATATU_14_SEATER: return "Nairobi 14-Seater Matatu (Nganya)";
        case VehicleID::SPRINTER_VAN: return "High-Roof Sprinter Commuter Van";
        case VehicleID::SHUTTLE_BUS: return "Airport Terminal Shuttle Bus";
        case VehicleID::SCHOOL_VAN: return "Regional Yellow School Van";
        case VehicleID::COACH_VOLVO_9700_DD: return "Volvo 9700DD Double-Decker 6x2 (14.8m)";
        case VehicleID::COACH_VOLVO_9800_DD: return "Volvo 9800 Double-Decker 6x2 (15.0m)";
        case VehicleID::COACH_VOLVO_9700_TEAL: return "Volvo 9700 13.1m Modern Coach";
        case VehicleID::COACH_VOLVO_9800_AERO: return "Volvo 9800 2026 Aero 6x2 (14.4m)";
        case VehicleID::COACH_YUTONG_TOURING: return "Yutong T13 Touring 6x2 (13.8m)";
        case VehicleID::COACH_AERO_BLADE_VIP: return "Aero Blade VIP 6x2 Concept (13.5m)";
        case VehicleID::COACH_CHAMPAGNE_SR2: return "Champagne SR2 High-Deck (12.8m)";
        case VehicleID::COACH_NEOPLAN_TOURLINER: return "Neoplan Tourliner 6x2 (13.9m)";
        case VehicleID::COACH_TWIN_GLASS_DOME: return "Twin-Dome Panoramic Coach (12.6m)";
        case VehicleID::COACH_OBSIDIAN_VIP_6X2: return "Obsidian Black VIP 6x2 (13.6m)";
        case VehicleID::COACH_EXECUTIVE_I6: return "Executive Coach";
        case VehicleID::COACH_LUXURY_VIP_4X2: return "Luxury Coach";
        case VehicleID::LORRY_UD_QUESTER_TIPPER: return "UD Trucks Quester 8x4 Tipper";
        case VehicleID::LORRY_MAN_TGX_BOX: return "MAN TGX 26.440 6x2 Box";
        case VehicleID::LORRY_SCANIA_P320_CURTAINSIDER: return "Scania P 320 6x2 Curtainsider";
        case VehicleID::TRACTOR_VOLVO_FH_ELECTRIC: return "Volvo FH Electric 4x2";
        case VehicleID::TRACTOR_MERCEDES_EACTROS_600: return "Mercedes-Benz eActros 600";
        case VehicleID::TRACTOR_MERCEDES_GENH2: return "Mercedes-Benz GenH2 4x2";
        case VehicleID::TRACTOR_SCANIA_45R_ELECTRIC: return "Scania 45 R Electric 4x2";
        case VehicleID::TRACTOR_SCANIA_R560_SUPER: return "Scania R 560 Super 4x2";
        case VehicleID::TRACTOR_SCANIA_S770_LONGLINE: return "Scania S 770 Longline 6x4";
        case VehicleID::TRACTOR_SCANIA_40R_XT_ELECTRIC: return "Scania 40 R XT Electric 4x2";
        case VehicleID::TRACTOR_FORD_FMAX_500: return "Ford Trucks F-MAX 500 4x2";
        case VehicleID::TRACTOR_IVECO_SEWAY_ELECTRIC: return "Iveco S-eWay Electric 4x2";
        case VehicleID::TRACTOR_VW_CONSTELLATION_25390: return "VW Constellation 25.390 6x2";
        case VehicleID::TRACTOR_VW_METEOR_28460: return "VW Meteor 28.460 6x4";
        case VehicleID::TRACTOR_MAN_TGX_18510: return "MAN TGX 18.510 4x2";
        case VehicleID::TRACTOR_VOLVO_FH16_800: return "Volvo FH16 800 Aero 4x2";
        case VehicleID::TRACTOR_VOLVO_FH16_780: return "Volvo FH16 780 Aero 4x2";
        case VehicleID::TRACTOR_VOLVO_FH16_750_XXL: return "Volvo FH16 750 XXL 6x4";
        default: return "Unknown Vehicle";
    }
}

const char* GetVehicleCategory(VehicleID id) {
    switch (id) {
        case VehicleID::LORRY_RIGID_BOX:
        case VehicleID::LORRY_RIGID_FLATBED:
        case VehicleID::LORRY_RIGID_TIPPER:
        case VehicleID::LORRY_RIGID_CEMENT_MIXER:
        case VehicleID::LORRY_RIGID_TANKER:
        case VehicleID::LORRY_UD_QUESTER_TIPPER:
        case VehicleID::LORRY_MAN_TGX_BOX:
        case VehicleID::LORRY_SCANIA_P320_CURTAINSIDER:
            return "Rigid Lorries";

        case VehicleID::TRACTOR_CAB_OVER:
        case VehicleID::TRACTOR_CONVENTIONAL:
        case VehicleID::TRACTOR_EURO_CAB:
        case VehicleID::TRACTOR_AMERICAN_CAB:
        case VehicleID::TRACTOR_VOLVO_FH_ELECTRIC:
        case VehicleID::TRACTOR_MERCEDES_EACTROS_600:
        case VehicleID::TRACTOR_MERCEDES_GENH2:
        case VehicleID::TRACTOR_SCANIA_45R_ELECTRIC:
        case VehicleID::TRACTOR_SCANIA_R560_SUPER:
        case VehicleID::TRACTOR_SCANIA_S770_LONGLINE:
        case VehicleID::TRACTOR_SCANIA_40R_XT_ELECTRIC:
        case VehicleID::TRACTOR_FORD_FMAX_500:
        case VehicleID::TRACTOR_IVECO_SEWAY_ELECTRIC:
        case VehicleID::TRACTOR_VW_CONSTELLATION_25390:
        case VehicleID::TRACTOR_VW_METEOR_28460:
        case VehicleID::TRACTOR_MAN_TGX_18510:
        case VehicleID::TRACTOR_VOLVO_FH16_800:
        case VehicleID::TRACTOR_VOLVO_FH16_780:
        case VehicleID::TRACTOR_VOLVO_FH16_750_XXL:
            return "Articulated Tractors";

        case VehicleID::BUS_CITY_STANDARD:
        case VehicleID::BUS_CITY_DOUBLE_DECKER:
        case VehicleID::BUS_CITY_ARTICULATED_BENDY:
        case VehicleID::BUS_CITY_LOW_FLOOR:
            return "City Buses";

        case VehicleID::MATATU_14_SEATER:
        case VehicleID::SPRINTER_VAN:
        case VehicleID::SHUTTLE_BUS:
        case VehicleID::SCHOOL_VAN:
            return "Minibuses & Matatus";

        default:
            return "Intercity Coaches";
    }
}

} // namespace sphere::vehicles
