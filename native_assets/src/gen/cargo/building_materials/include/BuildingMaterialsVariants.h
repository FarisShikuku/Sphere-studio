#pragma once
#include "../../library/include/CargoDef.h"
#include <vector>

namespace sphere::cargo {

enum class BuildingMaterialForm : uint32_t {
    STRUCTURAL_IBEAM = 0,
    GLULAM_TIMBER_BEAM,
    WASHED_SAND_HEAP,
    SAND_FIBC_TOTE,
    FLOAT_GLASS_RACK,
    HOLLOW_CMU_PALLET,
    PRECAST_HOLLOW_SLAB,
    GYPSUM_DRYWALL_STACK,
    CORRUGATED_STEEL_ROOF,
    TERRACOTTA_TILE_BUNDLE
};

struct BuildingMaterialVariantConfig {
    CargoID id;
    const char* variantCode;
    const char* displayName;
    float nominalWeightKg;
    float lengthMeters;
    float widthMeters;
    float heightMeters;
    BuildingMaterialForm form;
    Vec4 primaryColor;
    Vec4 secondaryColor;
    float roughness;
    float metallic;
    float opacityBias;
    bool isBulkGranular;
};

class BuildingMaterialsVariants {
public:
    static const std::vector<BuildingMaterialVariantConfig>& GetAllVariants();
    static const BuildingMaterialVariantConfig* FindVariant(CargoID id);
    static CargoDef CreateCargoDef(const BuildingMaterialVariantConfig& cfg);
};

} // namespace sphere::cargo
