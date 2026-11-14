#pragma once
#include "../../library/include/TrailerDef.h"
#include <vector>

namespace sphere::trailers::flatbed {

struct FlatbedVariantConfig {
    TrailerID id;
    const char* name;
    float emptyWeightKg;
    float maxPayloadKg;
    Vec3 dims;
    float deckHeightM;
    uint32_t axleCount;
    bool hasCraneOrDrop;
    Vec4 baseColor;
    Vec4 accentColor;
    const char* labelText;
};

class FlatbedVariants {
public:
    static const std::vector<FlatbedVariantConfig>& GetAll();
    static const FlatbedVariantConfig* FindVariant(TrailerID id);
    static TrailerDef CreateTrailerDef(const FlatbedVariantConfig& cfg);
};

} // namespace sphere::trailers::flatbed
