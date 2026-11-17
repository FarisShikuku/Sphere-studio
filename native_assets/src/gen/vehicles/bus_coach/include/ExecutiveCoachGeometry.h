#pragma once

#include "Mesh.h"
#include "IVehicle.h"
#include <memory>

namespace sphere::vehicles::coach {

// Bespoke procedural 3D mesh generator for the Executive Coach (COACH_EXECUTIVE_I6),
// featuring a sharp V-prow wedge fascia, sculpted tapered top-front aero roof dome,
// curved gooseneck mirrors, and J-curved Irizar-style side window/spear geometry.
std::shared_ptr<Mesh> GenerateExecutiveCoachMesh(const VehicleDef& def);

} // namespace sphere::vehicles::coach
