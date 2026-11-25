#pragma once
// ============================================================================
// GrassBladeGen.h - Parametric Bezier Grass Blade Mesh Synthesizer in C++17
// ============================================================================

#include "GrassTypes.h"
#include <vector>

namespace vegetation {

struct GrassBladeMeshSegment {
    std::vector<VegVertex_C> vertices;
    std::vector<uint32_t> indices;
};

class GrassBladeGen {
public:
    static GrassBladeMeshSegment buildBladeGeometry(
        const GrassBlade_C& blade,
        int verticalSegments = 4,
        float windDeflectionRad = 0.0f
    );
};

} // namespace vegetation
