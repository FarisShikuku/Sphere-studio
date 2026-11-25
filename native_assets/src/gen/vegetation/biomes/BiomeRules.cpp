// ============================================================================
// BiomeRules.cpp - Whittaker Ecological Matrix Implementation
// ============================================================================

#include "BiomeRules.h"
#include <algorithm>
#include <cmath>

extern "C" {

BiomeWeights_C veg_classify_whittaker_biome(float temperatureCelsius, float annualPrecipitationCm) {
    BiomeWeights_C w = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, BIOME_TEMPERATE_DECIDUOUS};

    // Whittaker diagram classification:
    // Temperature: -15C to +30C
    // Precipitation: 0 to 450 cm/year

    if (temperatureCelsius < -5.0f) {
        w.tundra = 1.0f;
        w.dominantBiome = BIOME_ARCTIC_TUNDRA;
    } else if (temperatureCelsius < 8.0f) {
        if (annualPrecipitationCm < 40.0f) {
            w.tundra = 0.6f;
            w.forest = 0.4f;
            w.dominantBiome = BIOME_ARCTIC_TUNDRA;
        } else {
            w.forest = 1.0f;
            w.dominantBiome = BIOME_BOREAL_FOREST;
        }
    } else if (temperatureCelsius < 20.0f) {
        if (annualPrecipitationCm < 35.0f) {
            w.desert = 0.8f;
            w.temperate = 0.2f;
            w.dominantBiome = BIOME_ARID_DESERT;
        } else if (annualPrecipitationCm < 150.0f) {
            w.temperate = 1.0f;
            w.dominantBiome = BIOME_TEMPERATE_DECIDUOUS;
        } else {
            w.forest = 0.7f;
            w.temperate = 0.3f;
            w.dominantBiome = BIOME_BOREAL_FOREST;
        }
    } else { // High temperature > 20C
        if (annualPrecipitationCm < 30.0f) {
            w.desert = 1.0f;
            w.dominantBiome = BIOME_ARID_DESERT;
        } else if (annualPrecipitationCm < 120.0f) {
            w.savanna = 0.8f;
            w.desert = 0.2f;
            w.dominantBiome = BIOME_TROPICAL_SAVANNA;
        } else {
            w.temperate = 0.5f;
            w.savanna = 0.5f;
            w.dominantBiome = BIOME_TEMPERATE_DECIDUOUS;
        }
    }

    return w;
}

} // extern "C"
