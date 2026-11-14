#pragma once
#include "../../library/include/TrailerDef.h"
#include <vector>

namespace sphere::trailers::chassis {
struct ChassisVariantConfig {
    TrailerID id;
    const char* name;
    float emptyWeightKg;
    float maxPayloadKg;
    Vec3 dims;
    uint32_t axleCount;
    bool gooseneck;
    bool tipping;
    Vec4 baseColor;
    const char* labelText;
};
class ChassisVariants {
public:
    static const std::vector<ChassisVariantConfig>& GetAll();
    static const ChassisVariantConfig* FindVariant(TrailerID id);
    static TrailerDef CreateTrailerDef(const ChassisVariantConfig& cfg);
};
} // namespace sphere::trailers::chassis
