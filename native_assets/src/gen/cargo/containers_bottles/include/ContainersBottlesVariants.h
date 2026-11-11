#pragma once
#include "../../library/include/CargoDef.h"
#include <vector>

namespace sphere::cargo {

enum class BottleSubstrate : uint32_t {
    AMBER_BOROSILICATE_GLASS = 0,
    CLEAR_POLYETHYLENE_TEREPHTHALATE,
    TINTED_GREEN_GLASS,
    CLEAR_SODA_LIME_GLASS_JAR,
    ELECTROLYTIC_TINPLATE_STEEL_CAN
};

struct BottleVariantConfig {
    CargoID id;
    const char* variantCode;
    const char* displayName;
    float nominalWeightKg;
    float volumeLiters;
    float baseDiameterMeters;
    float totalHeightMeters;
    BottleSubstrate substrate;
    Vec4 liquidColor;
    Vec4 labelColor;
    const char* brandText;
    float opacityBias;
    float roughness;
    float metallic;
    float neckRatio;
};

class ContainersBottlesVariants {
public:
    static const std::vector<BottleVariantConfig>& GetAllVariants();
    static const BottleVariantConfig* FindVariant(CargoID id);
    static CargoDef CreateCargoDef(const BottleVariantConfig& cfg);
};

} // namespace sphere::cargo
