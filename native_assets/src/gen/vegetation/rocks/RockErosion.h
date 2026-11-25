#pragma once
// ============================================================================
// RockErosion.h - Thermal & Hydraulic Weathering Simulation in C++17
// ============================================================================

#include "../common/VegetationMath.h"
#include <vector>

namespace vegetation {

struct RockMeshData;

class RockErosion {
public:
    static void simulateHydraulicTalus(RockMeshData& mesh, float erosionFactor);
};

} // namespace vegetation
