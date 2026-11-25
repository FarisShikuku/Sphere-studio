#pragma once

#include "FenceGen.h"
#include "HayBaleGen.h"
#include "WaterTroughGen.h"
#include "GrainSiloGen.h"
#include "FarmWindmillGen.h"
#include "FarmWagonGen.h"

namespace procedural::props {

struct RuralPropParams {
    FenceParams fence;
    HayBaleParams hayBale;
    WaterTroughParams waterTrough;
    GrainSiloParams grainSilo;
    FarmWindmillParams farmWindmill;
    FarmWagonParams farmWagon;
};

} // namespace procedural::props
