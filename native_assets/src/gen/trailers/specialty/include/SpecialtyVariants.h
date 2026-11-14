#pragma once
#include "../../library/include/TrailerDef.h"
#include <vector>

namespace sphere::trailers::specialty {
struct SpecialtyVariantConfig {
    TrailerID id;
    const char* name;
    float emptyWeightKg;
    float maxPayloadKg;
    Vec3 dims;
    uint32_t axleCount;
    Vec4 baseColor;
    Vec4 accentColor;
    const char* labelText;
};
class SpecialtyVariants {
public:
    static const std::vector<SpecialtyVariantConfig>& GetAll();
    static const SpecialtyVariantConfig* FindVariant(TrailerID id);
    static TrailerDef CreateTrailerDef(const SpecialtyVariantConfig& cfg);
};
} // namespace sphere::trailers::specialty
