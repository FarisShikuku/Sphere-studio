#pragma once
#include "../../library/include/CargoDef.h"
#include <vector>

namespace sphere::cargo {

enum class BagMaterialType : uint32_t {
    WOVEN_POLYPROPYLENE = 0,
    MULTIWALL_KRAFT_PAPER,
    LAMINATED_PLASTIC,
    ROUGH_JUTE_HESSIAN,
    FOIL_LINED_KRAFT
};

struct BagVariantConfig {
    CargoID id;
    const char* variantCode;
    const char* displayName;
    float nominalWeightKg;
    float lengthMeters;
    float widthMeters;
    float heightMeters;
    BagMaterialType materialType;
    Vec4 primaryColor;
    Vec4 labelColor;
    const char* brandText;
    float weaveRoughness;
    float wrinkleAmplitude;
};

class BaggedGoodsVariants {
public:
    static const std::vector<BagVariantConfig>& GetAllVariants();
    static const BagVariantConfig* FindVariant(CargoID id);
    static CargoDef CreateCargoDef(const BagVariantConfig& cfg);
};

} // namespace sphere::cargo
