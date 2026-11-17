#pragma once
#include "Mesh.h"
#include "VehicleDef.h"
#include <memory>

namespace sphere::vehicles::trucks {

// Dedicated real-world rigid lorry procedural 3D generators (each in its own .cpp file)
std::shared_ptr<Mesh> GenerateUdQuesterTipperMesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateManTgxBoxLorryMesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateScaniaP320CurtainsiderMesh(const VehicleDef& def);

} // namespace sphere::vehicles::trucks
