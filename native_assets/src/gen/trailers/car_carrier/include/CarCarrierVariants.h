#pragma once
#include "../../library/include/TrailerDef.h"
#include <vector>

namespace sphere::trailers::carcarrier {
struct CarCarrierVariantConfig {
    TrailerID id;
    const char* name;
    float emptyWeightKg;
    float maxPayloadKg;
    Vec3 dims;
    bool doubleDeck;
    bool enclosed;
    Vec4 baseColor;
    const char* labelText;
};
class CarCarrierVariants {
public:
    static const std::vector<CarCarrierVariantConfig>& GetAll();
    static const CarCarrierVariantConfig* FindVariant(TrailerID id);
    static TrailerDef CreateTrailerDef(const CarCarrierVariantConfig& cfg);
};
} // namespace sphere::trailers::carcarrier
