#pragma once

#include "../PropMeshHelper.h"
#include "EnergyParams.h"

namespace procedural::props {

class WindTurbineTower {
public:
    static void generate(GeneratedMeshPayload& payload, const WindTurbineParams& params);
};

} // namespace procedural::props
