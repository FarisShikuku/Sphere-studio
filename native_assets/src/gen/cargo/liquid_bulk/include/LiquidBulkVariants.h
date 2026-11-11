#pragma once
#include "../../library/include/CargoDef.h"
#include <vector>

namespace sphere::cargo {

enum class LiquidVesselType : uint32_t {
    BULK_ROAD_TANKER = 0,
    STEEL_DRUM_200L,
    PLASTIC_JERRICAN,
    DAIRY_MILK_CHURN,
    PRESSURIZED_GAS_CYLINDER
};

struct LiquidVariantConfig {
    CargoID id;
    const char* variantCode;
    const char* displayName;
    float nominalWeightKg;
    float capacityLiters;
    float diameterMeters;
    float heightMeters;
    LiquidVesselType vesselType;
    Vec4 primaryColor;
    Vec4 hazardDecalColor;
    const char* hazchemCode;
    float metallic;
    float roughness;
    uint32_t ribCount;
};

class LiquidBulkVariants {
public:
    static const std::vector<LiquidVariantConfig>& GetAllVariants();
    static const LiquidVariantConfig* FindVariant(CargoID id);
    static CargoDef CreateCargoDef(const LiquidVariantConfig& cfg);
};

} // namespace sphere::cargo
