#pragma once
#include "../../library/include/TrailerDef.h"
#include <vector>

namespace sphere::trailers::dolly {
struct DollyVariantConfig {
    TrailerID id;
    const char* name;
    float emptyWeightKg;
    float maxPayloadKg;
    Vec3 dims;
    uint32_t axleCount;
    Vec4 baseColor;
    const char* labelText;
};
class DollyVariants {
public:
    static const std::vector<DollyVariantConfig>& GetAll();
    static const DollyVariantConfig* FindVariant(TrailerID id);
    static TrailerDef CreateTrailerDef(const DollyVariantConfig& cfg);
};
} // namespace sphere::trailers::dolly
