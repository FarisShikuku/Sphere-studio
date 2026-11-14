#pragma once
#include "../../library/include/TrailerDef.h"
#include <vector>

namespace sphere::trailers::hopper {
struct BulkHopperVariantConfig {
    TrailerID id;
    const char* name;
    float emptyWeightKg;
    float maxPayloadKg;
    Vec3 dims;
    bool isPneumatic;
    Vec4 baseColor;
    Vec4 accentColor;
    const char* labelText;
};
class BulkHopperVariants {
public:
    static const std::vector<BulkHopperVariantConfig>& GetAll();
    static const BulkHopperVariantConfig* FindVariant(TrailerID id);
    static TrailerDef CreateTrailerDef(const BulkHopperVariantConfig& cfg);
};
} // namespace sphere::trailers::hopper
