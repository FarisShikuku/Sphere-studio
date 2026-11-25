// ============================================================================
// DebrisGen.cpp - Organic Detritus Scatter Implementation
// ============================================================================

#include "DebrisGen.h"

namespace vegetation {

std::vector<DebrisItem> DebrisGen::scatterDebrisPatch(
    float minX, float maxX,
    float minZ, float maxZ,
    const DebrisScatterConfig& cfg,
    uint64_t seed
) {
    FastRng rng(seed);
    std::vector<DebrisItem> items;

    float area = (maxX - minX) * (maxZ - minZ);
    int count = static_cast<int>(area * cfg.densityPerSqMeter);
    items.reserve(count);

    for (int i = 0; i < count; ++i) {
        DebrisItem item;
        item.position.x = rng.nextRange(minX, maxX);
        item.position.y = 0.02f;
        item.position.z = rng.nextRange(minZ, maxZ);
        item.yawRad = rng.nextRange(0.0f, 6.2831853f);
        item.scale = rng.nextRange(cfg.minScale, cfg.maxScale);
        item.type = cfg.type;

        float wetDarken = 1.0f - cfg.humidityWetness * 0.35f;
        item.tint = {
            0.42f * wetDarken * rng.nextRange(0.85f, 1.15f),
            0.30f * wetDarken * rng.nextRange(0.85f, 1.15f),
            0.18f * wetDarken * rng.nextRange(0.85f, 1.15f),
            1.0f
        };

        items.push_back(item);
    }

    return items;
}

} // namespace vegetation
