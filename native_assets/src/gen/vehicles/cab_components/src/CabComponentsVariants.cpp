#include "CabComponentsVariants.h"

namespace sphere::vehicles::cab {

CabPartSpec GetCabPartSpec(CabPartID id) {
    switch (id) {
        case CabPartID::DOOR_LEFT:    return {id, "Heavy Lorry Door (Left)", 1.05f, 1.38f, 0.08f};
        case CabPartID::DOOR_RIGHT:   return {id, "Heavy Lorry Door (Right)", 1.05f, 1.38f, 0.08f};
        case CabPartID::WINDSHIELD:   return {id, "Laminated Panoramic Windshield", 2.18f, 0.88f, 0.03f};
        case CabPartID::GRILLE_HEAVY: return {id, "Slatted Chrome Heavy Grille", 1.72f, 0.72f, 0.06f};
        case CabPartID::MIRROR_TRUCK: return {id, "Dual-Arm Heated Wing Mirrors", 0.34f, 0.56f, 0.06f};
        default:                      return {id, "Unknown Cab Part", 1.0f, 1.0f, 0.05f};
    }
}

} // namespace sphere::vehicles::cab
