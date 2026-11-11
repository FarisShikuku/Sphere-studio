#pragma once
#include "../../library/include/CargoDef.h"
#include <vector>

namespace sphere::cargo {

enum class SpecialtyCategory : uint32_t {
    HEAVY_CEMENT_BAG = 0,
    LONG_STEEL_PIPE,
    MASONRY_BRICK_PALLET,
    COMMERCIAL_TYRE_STACK
};

struct SpecialtyVariantConfig {
    CargoID id;
    const char* variantCode;
    const char* displayName;
    float nominalWeightKg;
    float lengthMeters;
    float widthMeters;
    float heightMeters;
    SpecialtyCategory category;
    Vec4 primaryColor;
    Vec4 secondaryColor;
    float roughness;
    float metallic;
    uint32_t unitCount; // e.g. 500 bricks, 4 tyres, 6 pipes
};

class SpecialtyIndustrialVariants {
public:
    static const std::vector<SpecialtyVariantConfig>& GetAllVariants();
    static const SpecialtyVariantConfig* FindVariant(CargoID id);
    static CargoDef CreateCargoDef(const SpecialtyVariantConfig& cfg);
};

} // namespace sphere::cargo
