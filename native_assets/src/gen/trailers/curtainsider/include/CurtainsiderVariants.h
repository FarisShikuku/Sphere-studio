#pragma once
#include "../../library/include/TrailerDef.h"
#include <vector>

namespace sphere::trailers::curtainsider {
struct CurtainsiderVariantConfig {
    TrailerID id;
    const char* name;
    float emptyWeightKg;
    float maxPayloadKg;
    Vec3 dims;
    float curtainOpacity;
    Vec4 curtainColor;
    Vec4 stripeColor;
    const char* labelText;
};
class CurtainsiderVariants {
public:
    static const std::vector<CurtainsiderVariantConfig>& GetAll();
    static const CurtainsiderVariantConfig* FindVariant(TrailerID id);
    static TrailerDef CreateTrailerDef(const CurtainsiderVariantConfig& cfg);
};
} // namespace sphere::trailers::curtainsider
