#pragma once
// ============================================================================
// GrassSim.h - Aerodynamic Grass Wind Deflection Simulation in C++17
// ============================================================================

#include "GrassTypes.h"
#include "../wind/WindField.h"

namespace vegetation {

class GrassSim {
public:
    static float evaluateBladeDeflection(
        const GrassBlade_C& blade,
        const WindField& wind,
        float globalTimeSec
    );
};

} // namespace vegetation
