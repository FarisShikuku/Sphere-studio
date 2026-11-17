#pragma once

#include "Mesh.h"
#include "IVehicle.h"
#include <memory>

namespace sphere::vehicles::coach {

// Bespoke procedural 3D mesh generator for the 12m Luxury Coach (COACH_LUXURY_VIP_4X2),
// featuring a double-height swept panoramic upper visor dome with 3 blue clearance markers,
// smiling chrome bumper mustache & swept almond multi-projector headlamps, white swan-neck
// aero mirrors, swept silver Z-boomerang cab trim, semicircular 4x2 wheel arches, and
// 3 full-height pantograph-lifting underfloor luggage boots with recessed interior holds.
std::shared_ptr<Mesh> GenerateLuxuryCoachMesh(const VehicleDef& def);

} // namespace sphere::vehicles::coach
