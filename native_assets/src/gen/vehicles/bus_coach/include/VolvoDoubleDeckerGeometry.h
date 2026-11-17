#pragma once

#include "Mesh.h"
#include "IVehicle.h"
#include <memory>

namespace sphere::vehicles::coach {

// Bespoke procedural 3D mesh generator for the Volvo Double-Decker coaches
// (COACH_VOLVO_9800_DD and COACH_VOLVO_9700_DD), featuring seamless dual-deck
// compound-curved front fascia & roof dome, bowed Volvo Iron Mark V-collar,
// slanted upper A-pillar ribbon, cantilevered digital camera-wing mirrors,
// sculpted wave lower waist & mid-deck blade, and inset dual-deck interiors.
std::shared_ptr<Mesh> GenerateVolvoDoubleDeckerMesh(const VehicleDef& def);

} // namespace sphere::vehicles::coach
