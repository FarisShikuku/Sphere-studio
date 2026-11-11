#pragma once
#include "../../library/include/CargoDef.h"
#include <vector>

namespace sphere::cargo {

enum class PerishableType : uint32_t {
    HARVEST_PRODUCE_CRATE = 0,
    VENTILATED_ROOT_BAG,
    ICED_SEAFOOD_CRATE,
    INSULATED_MEAT_BOX
};

struct PerishableVariantConfig {
    CargoID id;
    const char* variantCode;
    const char* displayName;
    float nominalWeightKg;
    float lengthMeters;
    float widthMeters;
    float heightMeters;
    PerishableType type;
    Vec4 primaryColor;
    Vec4 secondaryColor;
    float roughness;
    float wetnessSpecular;
    float storageTempCelsius;
};

class PerishableGoodsVariants {
public:
    static const std::vector<PerishableVariantConfig>& GetAllVariants();
    static const PerishableVariantConfig* FindVariant(CargoID id);
    static CargoDef CreateCargoDef(const PerishableVariantConfig& cfg);
};

} // namespace sphere::cargo
