#pragma once
// ============================================================================
// BiomeRules.h - Whittaker Ecological Biome Classification in C++17 / C
// ============================================================================

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum BiomeKind_C {
    BIOME_TEMPERATE_DECIDUOUS = 0,
    BIOME_ARID_DESERT = 1,
    BIOME_BOREAL_FOREST = 2,
    BIOME_ARCTIC_TUNDRA = 3,
    BIOME_TROPICAL_SAVANNA = 4
} BiomeKind_C;

typedef struct BiomeWeights_C {
    float temperate;
    float desert;
    float forest;
    float tundra;
    float savanna;
    BiomeKind_C dominantBiome;
} BiomeWeights_C;

// Pure C Whittaker biome evaluation function
BiomeWeights_C veg_classify_whittaker_biome(float temperatureCelsius, float annualPrecipitationCm);

#ifdef __cplusplus
} // extern "C"
#endif

namespace vegetation {

class BiomeRules {
public:
    static BiomeWeights_C evaluate(float temperatureCelsius, float annualPrecipitationCm) {
        return veg_classify_whittaker_biome(temperatureCelsius, annualPrecipitationCm);
    }
};

} // namespace vegetation
