#pragma once
#include "../../library/include/TrailerDef.h"
#include <vector>

namespace sphere::trailers::logging {
struct LoggingVariantConfig {
    TrailerID id;
    const char* name;
    float emptyWeightKg;
    float maxPayloadKg;
    Vec3 dims;
    uint32_t bunkCount;
    bool hasHydraulicCrane;
    Vec4 baseColor;
    const char* labelText;
};
class LoggingVariants {
public:
    static const std::vector<LoggingVariantConfig>& GetAll();
    static const LoggingVariantConfig* FindVariant(TrailerID id);
    static TrailerDef CreateTrailerDef(const LoggingVariantConfig& cfg);
};
} // namespace sphere::trailers::logging
