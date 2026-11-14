#pragma once
#include "../../library/include/TrailerDef.h"
#include <vector>

namespace sphere::trailers::boxvan {
struct BoxVanVariantConfig {
    TrailerID id;
    const char* name;
    float emptyWeightKg;
    float maxPayloadKg;
    Vec3 dims;
    bool hasReeferUnit;
    bool hasSideSkirts;
    Vec4 baseColor;
    Vec4 accentColor;
    const char* labelText;
};
class BoxVanVariants {
public:
    static const std::vector<BoxVanVariantConfig>& GetAll();
    static const BoxVanVariantConfig* FindVariant(TrailerID id);
    static TrailerDef CreateTrailerDef(const BoxVanVariantConfig& cfg);
};
} // namespace sphere::trailers::boxvan
