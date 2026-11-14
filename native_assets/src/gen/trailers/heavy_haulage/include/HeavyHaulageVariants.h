#pragma once
#include "../../library/include/TrailerDef.h"
#include <vector>

namespace sphere::trailers::heavy {
struct HeavyVariantConfig {
    TrailerID id;
    const char* name;
    float emptyWeightKg;
    float maxPayloadKg;
    Vec3 dims;
    float deckHeightM;
    uint32_t axleCount;
    Vec4 baseColor;
    const char* labelText;
};
class HeavyHaulageVariants {
public:
    static const std::vector<HeavyVariantConfig>& GetAll();
    static const HeavyVariantConfig* FindVariant(TrailerID id);
    static TrailerDef CreateTrailerDef(const HeavyVariantConfig& cfg);
};
} // namespace sphere::trailers::heavy
