#pragma once
// ============================================================================
// RockGenerator.h - Procedural Geodesic Noise Mesh Modeler in C++17
// ============================================================================

#include "RockParams.h"
#include "../common/VegetationMath.h"
#include <vector>

namespace vegetation {

struct RockMeshData {
    std::vector<VegVertex_C> vertices;
    std::vector<uint32_t> indices;
};

class RockGenerator {
public:
    static RockMeshData generateRockMesh(const RockSynthesisParams& params, uint64_t seed);
};

} // namespace vegetation
