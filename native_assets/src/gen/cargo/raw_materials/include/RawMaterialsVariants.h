#pragma once
#include "../../library/include/CargoDef.h"
#include <vector>

namespace sphere::cargo {

enum class RawMaterialForm : uint32_t {
    CYLINDRICAL_TIMBER_LOG = 0,
    MILLED_RECTANGULAR_LUMBER,
    DEFORMED_STEEL_REBAR_BUNDLE,
    BULK_COAL_PILE,
    AGGREGATE_GRAVEL_MOUND,
    QUARTER_SPLIT_WEDGE_FIREWOOD,
    HALF_SPLIT_BIRCH_LOGS,
    CROSS_STACKED_CORDWOOD_RICK,
    CHAMFERED_RAILWAY_SLEEPER,
    CONSTRICTED_KINDLING_BUNDLE,
    FLARED_BUTCHER_STUMP_BLOCK
};

struct RawVariantConfig {
    CargoID id;
    const char* variantCode;
    const char* displayName;
    float nominalWeightKg;
    float lengthMeters;
    float widthMeters;
    float heightMeters;
    RawMaterialForm form;
    Vec4 primaryColor;
    Vec4 secondaryColor;
    float roughness;
    float metallic;
    float barkOrGritRoughness;
};

class RawMaterialsVariants {
public:
    static const std::vector<RawVariantConfig>& GetAllVariants();
    static const RawVariantConfig* FindVariant(CargoID id);
    static CargoDef CreateCargoDef(const RawVariantConfig& cfg);
};

} // namespace sphere::cargo
