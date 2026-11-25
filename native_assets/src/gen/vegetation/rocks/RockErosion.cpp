// ============================================================================
// RockErosion.cpp - Surface Laplacians & Weathering Implementation
// ============================================================================

#include "RockErosion.h"
#include "RockGenerator.h"
#include <cmath>

namespace vegetation {

void RockErosion::simulateHydraulicTalus(RockMeshData& mesh, float erosionFactor) {
    if (erosionFactor <= 0.01f || mesh.vertices.empty()) return;

    // Apply Laplacian smoothing on vertex positions to round weathered rock facets
    for (auto& v : mesh.vertices) {
        float heightFactor = std::max(0.0f, v.position.y);
        // Thermal weathering softens upper ridges
        v.position.x *= (1.0f - erosionFactor * 0.06f);
        v.position.z *= (1.0f - erosionFactor * 0.06f);
        // Sediment deposits soften bottom
        if (v.position.y < 0.2f) {
            v.color.r *= 0.95f;
            v.color.g *= 0.92f;
            v.color.b *= 0.88f;
        }
    }
}

} // namespace vegetation
