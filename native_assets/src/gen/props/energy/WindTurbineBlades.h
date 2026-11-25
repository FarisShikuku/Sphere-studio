#pragma once

#include "../PropMeshHelper.h"
#include "EnergyParams.h"

namespace procedural::props {

class WindTurbineBlades {
public:
    static void generate(GeneratedMeshPayload& payload, const WindTurbineParams& params);
};

} // namespace procedural::props
