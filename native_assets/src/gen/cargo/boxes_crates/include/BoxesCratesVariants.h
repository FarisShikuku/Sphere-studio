#pragma once
#include "../../library/include/CargoDef.h"
#include <vector>

namespace sphere::cargo {

enum class BoxConstruction : uint32_t {
    CORRUGATED_CARDBOARD = 0,
    SLATTED_TIMBER_WOOD,
    VENTED_MOLDED_PLASTIC,
    WOODEN_EXCHANGE_PALLET,
    REINFORCED_MILK_CRATE
};

struct BoxVariantConfig {
    CargoID id;
    const char* variantCode;
    const char* displayName;
    float nominalWeightKg;
    float lengthMeters;
    float widthMeters;
    float heightMeters;
    BoxConstruction construction;
    Vec4 primaryColor;
    Vec4 secondaryColor;
    const char* labelText;
    float roughness;
    float slatWidthMeters;
    float corrugationPitch;
};

class BoxesCratesVariants {
public:
    static const std::vector<BoxVariantConfig>& GetAllVariants();
    static const BoxVariantConfig* FindVariant(CargoID id);
    static CargoDef CreateCargoDef(const BoxVariantConfig& cfg);
};

} // namespace sphere::cargo
