#pragma once

#include "LampPostGen.h"
#include "TrafficLightGen.h"
#include "BenchGen.h"
#include "TrashBinGen.h"

namespace procedural::props {

struct StreetPropParams {
    LampPostParams lampPost;
    TrafficLightParams trafficLight;
    BenchParams bench;
    TrashBinParams trashBin;
};

} // namespace procedural::props
