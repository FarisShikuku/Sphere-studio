#pragma once
#include "../../library/include/TrailerDef.h"
#include <vector>

namespace sphere::trailers::tanker {

struct TankerVariantConfig {
    TrailerID id;
    const char* name;
    float emptyWeightKg;
    float maxPayloadKg;
    Vec3 dims;
    uint32_t compartments;
    bool pressurizedDome;
    Vec4 baseColor;
    Vec4 accentColor;
    float roughness;
    float metallic;
    const char* labelText;
};

class TankerVariants {
public:
    static const std::vector<TankerVariantConfig>& GetAll();
    static const TankerVariantConfig* FindVariant(TrailerID id);
    static TrailerDef CreateTrailerDef(const TankerVariantConfig& cfg);
};

} // namespace sphere::trailers::tanker
