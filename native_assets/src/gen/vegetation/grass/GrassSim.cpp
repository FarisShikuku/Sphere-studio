// ============================================================================
// GrassSim.cpp - Damped Spring Aerodynamic Deflection Physics
// ============================================================================

#include "GrassSim.h"
#include <cmath>

namespace vegetation {

float GrassSim::evaluateBladeDeflection(
    const GrassBlade_C& blade,
    const WindField& wind,
    float globalTimeSec
) {
    Vec3 windVec = wind.sampleWindVelocity(
        blade.basePosition.x,
        blade.basePosition.y,
        blade.basePosition.z,
        globalTimeSec
    );

    float windSpeed = windVec.length();
    // Angular deflection modeled via aerodynamic drag vs elastic blade stiffness
    float bladeStiffness = 4.5f / (blade.heightMeters * blade.heightMeters + 0.01f);
    float drag = 0.5f * 1.225f * (windSpeed * windSpeed) * (blade.widthMeters * blade.heightMeters);
    float targetAngle = std::atan2(drag, bladeStiffness * 0.1f);

    // Dynamic wave oscillation
    float flutter = std::sin(globalTimeSec * 4.2f + blade.basePosition.x * 3.0f + blade.basePosition.z * 2.5f) * 0.12f;

    return targetAngle * blade.windBendFactor + flutter;
}

} // namespace vegetation
