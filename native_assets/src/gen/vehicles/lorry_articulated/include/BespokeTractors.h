#pragma once
#include "Mesh.h"
#include "VehicleDef.h"
#include <memory>

namespace sphere::vehicles::trucks {

// Dedicated real-world articulated tractor procedural 3D generators (each in its own .cpp file)
std::shared_ptr<Mesh> GenerateVolvoFhElectricMesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateMercedesEActros600Mesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateMercedesGenH2Mesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateScania45RElectricMesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateScaniaR560SuperMesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateScaniaS770LonglineMesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateScania40RXtElectricMesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateFordFMax500Mesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateIvecoSEwayElectricMesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateVwConstellation25390Mesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateVwMeteor28460Mesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateManTgx18510Mesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateVolvoFh16800Mesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateVolvoFh16780Mesh(const VehicleDef& def);
std::shared_ptr<Mesh> GenerateVolvoFh16750XxlMesh(const VehicleDef& def);

} // namespace sphere::vehicles::trucks
