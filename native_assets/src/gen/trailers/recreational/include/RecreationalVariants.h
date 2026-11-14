#pragma once
#include "../../library/include/TrailerDef.h"
#include <vector>

namespace sphere::trailers::recreational {
struct RecreationalVariantConfig {
    TrailerID id;
    const char* name;
    float emptyWeightKg;
    float maxPayloadKg;
    Vec3 dims;
    uint32_t axleCount;
    Vec4 baseColor;
    const char* labelText;
};
class RecreationalVariants {
public:
    static const std::vector<RecreationalVariantConfig>& GetAll();
    static const RecreationalVariantConfig* FindVariant(TrailerID id);
    static TrailerDef CreateTrailerDef(const RecreationalVariantConfig& cfg);
};
} // namespace sphere::trailers::recreational
