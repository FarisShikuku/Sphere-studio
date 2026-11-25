#pragma once
#include <cstdint>

namespace vegetation {
struct BiomeMeshSpec {
    uint32_t maxInstancesPerChunk{8192};
    float chunkSizeMeters{128.0f};
    float lodMorphDistanceMeters{40.0f};
};
}
