#pragma once
#include <cstdint>

namespace vegetation {
struct WindMeshSpec {
    uint32_t streamlineSegmentCount{64};
    float streamlineParticleLifespanSec{4.5f};
    float vortexCoreRadius{3.5f};
};
}
