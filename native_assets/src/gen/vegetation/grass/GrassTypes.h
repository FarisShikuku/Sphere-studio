#pragma once
// ============================================================================
// GrassTypes.h - Grass Blade, Clump & Aerodynamic Simulation Types in C++17 / C
// ============================================================================

#include <cstdint>
#include "../common/VegetationMath.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum GrassSpecies_C {
    GRASS_FESCUE = 0,
    GRASS_KENTUCKY_BLUEGRASS = 1,
    GRASS_PERENNIAL_RYE = 2,
    GRASS_SAVANNA_THATCH = 3,
    GRASS_ALPINE_MEADOW = 4
} GrassSpecies_C;

typedef struct GrassBlade_C {
    VegVec3_C basePosition;
    float heightMeters;
    float widthMeters;
    float curvature;
    float headingRad;
    float tiltRad;
    float windBendFactor;
} GrassBlade_C;

typedef struct GrassFieldTile_C {
    uint32_t bladeCount;
    GrassBlade_C* blades;
    float areaWidthMeters;
    float areaLengthMeters;
} GrassFieldTile_C;

#ifdef __cplusplus
} // extern "C"
#endif
