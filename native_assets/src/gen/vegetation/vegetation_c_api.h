#pragma once
// ============================================================================
// vegetation_c_api.h - Pure Standard C Interface (extern "C")
// Enables foreign engines, JNI bridges, and C programs to invoke 100% of the
// botanical synthesis and rendering logic directly in C.
// ============================================================================

#include <stdint.h>
#include <stdbool.h>
#include "common/VegetationMath.h"
#include "grass/GrassTypes.h"
#include "biomes/BiomeRules.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* VegSystemHandle;

// Lifecycle
VegSystemHandle veg_system_create(uint64_t seed);
void veg_system_destroy(VegSystemHandle handle);
void veg_system_update(VegSystemHandle handle, float deltaTimeSec);

// Scattering query
uint32_t veg_system_scatter_instances(
    VegSystemHandle handle,
    uint32_t category,
    float areaWidthMeters,
    float areaLengthMeters,
    float densityPerSqMeter,
    VegVec3_C* outPositions,
    float* outScales,
    uint32_t maxCapacity
);

// High-speed C software rasterizer preview generating 32-bit ARGB pixels
void veg_render_software_preview(
    uint32_t category,
    uint32_t archetypeIndex,
    float animTimeSec,
    float camAzimuthDeg,
    float camElevationDeg,
    float camDistMeters,
    int width,
    int height,
    int32_t* outArgbPixels,
    float* outStats
);

// Stojanovic (2016) Vulkan C/C++ Engine Tree Mesh Vertex
typedef struct VegTreeMeshVertex_C {
    VegVec3_C position;
    VegVec3_C normal;
    float uvU;
    float uvV;
    VegColor_C color;
    float windWeight;
    uint32_t isLeaf;
} VegTreeMeshVertex_C;

// Pure C procedural tree synthesis using Stojanovic (2016) stochastic L-System & da Vinci pipe model
bool veg_stojanovic_synthesize_tree_mesh(
    uint32_t speciesId,
    uint64_t seed,
    float heightMeters,
    float trunkRadiusMeters,
    uint32_t recursionDepth,
    float branchAngleMeanDeg,
    float branchAngleDevDeg,
    float leafClusterSizeMeters,
    float windFlexibility,
    VegTreeMeshVertex_C* outVertices,
    uint32_t maxVertices,
    uint32_t* outVertexCount,
    uint32_t* outIndices,
    uint32_t maxIndices,
    uint32_t* outIndexCount
);

// Procedural 7-octave fractal Perlin noise bark synthesis with Sobel normal map
void veg_stojanovic_generate_bark_texture(
    int width,
    int height,
    float hurstParam,
    uint32_t* outRgbaPixels,
    uint32_t* outNormalMapPixels
);

#ifdef __cplusplus
} // extern "C"
#endif

