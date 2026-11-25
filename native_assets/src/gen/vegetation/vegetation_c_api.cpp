// ============================================================================
// vegetation_c_api.cpp - C API & Native Software Frame Rasterizer
// ============================================================================

#include "vegetation_c_api.h"
#include "VegetationSystem.h"
#include <algorithm>
#include <cmath>

extern "C" {

VegSystemHandle veg_system_create(uint64_t seed) {
    auto* sys = new vegetation::VegetationSystem();
    sys->initialize(seed);
    return static_cast<VegSystemHandle>(sys);
}

void veg_system_destroy(VegSystemHandle handle) {
    if (handle) {
        delete static_cast<vegetation::VegetationSystem*>(handle);
    }
}

void veg_system_update(VegSystemHandle handle, float deltaTimeSec) {
    if (handle) {
        static_cast<vegetation::VegetationSystem*>(handle)->update(deltaTimeSec);
    }
}

uint32_t veg_system_scatter_instances(
    VegSystemHandle handle,
    uint32_t category,
    float areaWidthMeters,
    float areaLengthMeters,
    float densityPerSqMeter,
    VegVec3_C* outPositions,
    float* outScales,
    uint32_t maxCapacity
) {
    if (!handle || !outPositions || !outScales || maxCapacity == 0) return 0;
    auto* sys = static_cast<vegetation::VegetationSystem*>(handle);

    std::vector<vegetation::VegetationInstance> instances;
    sys->scatterInstances(
        static_cast<vegetation::VegetationCategory>(category),
        areaWidthMeters,
        areaLengthMeters,
        densityPerSqMeter,
        instances
    );

    uint32_t count = std::min(maxCapacity, static_cast<uint32_t>(instances.size()));
    for (uint32_t i = 0; i < count; ++i) {
        outPositions[i] = {instances[i].posX, instances[i].posY, instances[i].posZ};
        outScales[i] = instances[i].scaleUniform;
    }
    return count;
}

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
) {
    if (!outArgbPixels || width <= 0 || height <= 0) return;

    // Clear background sky gradient with atmospheric haze
    for (int y = 0; y < height; ++y) {
        float ny = static_cast<float>(y) / height;
        // Sky gradient: Deep blue to soft horizon fog
        int r = static_cast<int>(vegetation::lerp(135.0f, 210.0f, ny));
        int g = static_cast<int>(vegetation::lerp(185.0f, 230.0f, ny));
        int b = static_cast<int>(vegetation::lerp(235.0f, 245.0f, ny));
        int32_t argb = (0xFF << 24) | (r << 16) | (g << 8) | b;

        for (int x = 0; x < width; ++x) {
            outArgbPixels[y * width + x] = argb;
        }
    }

    // Render ground plane slab
    int groundHorizonY = height * 5 / 8;
    for (int y = groundHorizonY; y < height; ++y) {
        float groundT = static_cast<float>(y - groundHorizonY) / (height - groundHorizonY);
        int gr = static_cast<int>(vegetation::lerp(55.0f, 35.0f, groundT));
        int gg = static_cast<int>(vegetation::lerp(95.0f, 65.0f, groundT));
        int gb = static_cast<int>(vegetation::lerp(45.0f, 25.0f, groundT));
        int32_t groundColor = (0xFF << 24) | (gr << 16) | (gg << 8) | gb;

        for (int x = 0; x < width; ++x) {
            outArgbPixels[y * width + x] = groundColor;
        }
    }

    if (outStats) {
        outStats[0] = static_cast<float>(width * height);
        outStats[1] = animTimeSec;
        outStats[2] = static_cast<float>(category);
    }
}

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
) {
    if (!outVertices || !outVertexCount || !outIndices || !outIndexCount) return false;

    vegetation::TreeSpeciesConfig cfg = vegetation::getPredefinedTreeSpecies(static_cast<vegetation::TreeSpeciesId>(speciesId));
    if (heightMeters > 0.0f) cfg.treeHeightMeters = heightMeters;
    if (trunkRadiusMeters > 0.0f) cfg.trunkRadiusMeters = trunkRadiusMeters;
    if (recursionDepth > 0) cfg.maxRecursionDepth = static_cast<int>(recursionDepth);
    if (branchAngleMeanDeg > 0.0f) cfg.branchAngleMeanDeg = branchAngleMeanDeg;
    if (branchAngleDevDeg >= 0.0f) cfg.branchAngleDeviationDeg = branchAngleDevDeg;
    if (leafClusterSizeMeters > 0.0f) cfg.leafClusterSizeMeters = leafClusterSizeMeters;
    if (windFlexibility >= 0.0f) cfg.windFlexibility = windFlexibility;

    vegetation::LSystemTree tree(cfg);
    vegetation::TreeMeshData mesh = tree.generateTreeMesh(seed);

    uint32_t vCount = std::min(maxVertices, static_cast<uint32_t>(mesh.vertices.size()));
    for (uint32_t i = 0; i < vCount; ++i) {
        const auto& v = mesh.vertices[i];
        outVertices[i].position = {v.position.x, v.position.y, v.position.z};
        outVertices[i].normal = {v.normal.x, v.normal.y, v.normal.z};
        outVertices[i].uvU = v.uvU;
        outVertices[i].uvV = v.uvV;
        outVertices[i].color = {v.r, v.g, v.b, v.a};
        outVertices[i].windWeight = v.windWeight;
        outVertices[i].isLeaf = v.isLeaf;
    }
    *outVertexCount = vCount;

    uint32_t iCount = std::min(maxIndices, static_cast<uint32_t>(mesh.indices.size()));
    for (uint32_t i = 0; i < iCount; ++i) {
        outIndices[i] = mesh.indices[i];
    }
    *outIndexCount = iCount;

    return true;
}

void veg_stojanovic_generate_bark_texture(
    int width,
    int height,
    float hurstParam,
    uint32_t* outRgbaPixels,
    uint32_t* outNormalMapPixels
) {
    if (width <= 0 || height <= 0) return;
    float H = std::clamp(hurstParam, 0.4f, 0.9f);
    std::vector<float> heightmap(width * height, 0.0f);

    for (int y = 0; y < height; ++y) {
        float v = static_cast<float>(y) / height;
        for (int x = 0; x < width; ++x) {
            float u = static_cast<float>(x) / width;
            float noiseSum = 0.0f;
            float amp = 1.0f;
            float freq = 2.0f;

            for (int oct = 0; oct < 7; ++oct) {
                float n = std::sin(u * freq * 6.28318f) * std::cos(v * freq * 12.56637f);
                noiseSum += n * amp;
                amp *= H;
                freq *= 2.0f;
            }

            float kMod = noiseSum * 0.5f;
            float sinMod = std::sin(u * 25.0f + kMod * 2.0f) + std::sin(v * 45.0f + kMod);
            float hVal = std::clamp(0.5f + sinMod * 0.25f, 0.0f, 1.0f);
            heightmap[y * width + x] = hVal;

            if (outRgbaPixels) {
                int r = static_cast<int>(std::clamp(hVal * 110.0f + 25.0f, 0.0f, 255.0f));
                int g = static_cast<int>(std::clamp(hVal * 75.0f + 18.0f, 0.0f, 255.0f));
                int b = static_cast<int>(std::clamp(hVal * 50.0f + 12.0f, 0.0f, 255.0f));
                outRgbaPixels[y * width + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }

    if (outNormalMapPixels) {
        for (int y = 0; y < height; ++y) {
            int ym1 = (y - 1 + height) % height;
            int yp1 = (y + 1) % height;
            for (int x = 0; x < width; ++x) {
                int xm1 = (x - 1 + width) % width;
                int xp1 = (x + 1) % width;

                float tl = heightmap[ym1 * width + xm1];
                float tr = heightmap[ym1 * width + xp1];
                float l  = heightmap[y * width + xm1];
                float r  = heightmap[y * width + xp1];
                float bl = heightmap[yp1 * width + xm1];
                float br = heightmap[yp1 * width + xp1];

                float gx = (tr + 2.0f * r + br) - (tl + 2.0f * l + bl);
                float gy = (bl + 2.0f * heightmap[yp1 * width + x] + br) - (tl + 2.0f * heightmap[ym1 * width + x] + tr);

                float nz = 1.0f;
                float len = std::sqrt(gx * gx + gy * gy + nz * nz);
                float nx = -gx / len;
                float ny = -gy / len;
                nz = nz / len;

                int nr = static_cast<int>((nx * 0.5f + 0.5f) * 255.0f);
                int ng = static_cast<int>((ny * 0.5f + 0.5f) * 255.0f);
                int nb = static_cast<int>((nz * 0.5f + 0.5f) * 255.0f);
                outNormalMapPixels[y * width + x] = (0xFF << 24) | (nr << 16) | (ng << 8) | nb;
            }
        }
    }
}

} // extern "C"

