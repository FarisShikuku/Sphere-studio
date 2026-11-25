#pragma once

#include <cstdint>

namespace procedural::props {

struct EnergyTextureSpec {
    uint32_t pvCellResolution = 1024;
    float siliconReflectance = 0.08f;
    float busbarMetallic = 0.95f;
    float glassRoughness = 0.02f;
    float turbineCoatingRoughness = 0.45f;
    float transformerPaintRoughness = 0.55f;
};

} // namespace procedural::props
