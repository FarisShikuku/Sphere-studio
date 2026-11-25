#pragma once
// ============================================================================
// ForestCanopyGenerator.h
// Native C++17 PlantGL Asymmetric Crown Hull 3D Mesher
// Part of the Unified Vegetation Subsystem (native-assets/src/vegetation/forest/)
//
// Conforms to Pradal & Boudon, "Asymmetric Crown Hulls: A Simple Geometric
// Model of the Tree Crown", UMR AMAP / INRIA.
// ============================================================================

#include <vector>
#include <cstdint>
#include "../common/VegetationMath.h"

namespace vegetation {

struct CrownControlParams {
    float rxPos{3.6f};
    float rxNeg{3.2f};
    float ryPos{3.5f};
    float ryNeg{3.1f};
    float hzXPos{4.2f};
    float hzXNeg{4.5f};
    float hzYPos{4.0f};
    float hzYNeg{4.6f};
    float offsetX{0.15f};
    float offsetY{-0.10f};
    float crownHeight{10.5f};
    float shapeTop{1.85f};
    float shapeBot{1.45f};
    float trunkHeight{4.2f};
    int leafCount{320};
};

struct CrownMeshVertex {
    float px, py, pz;
    float nx, ny, nz;
    float u, v;
};

struct CrownMesh3D {
    std::vector<CrownMeshVertex> vertices;
    std::vector<uint32_t> indices;
};

struct LeafPlatelet3D {
    float posX, posY, posZ;
    float normX, normY, normZ;
    float scale;
    float shadeFactor;
};

class ForestCanopyGenerator {
public:
    static CrownMesh3D generateWireframeHull(const CrownControlParams& params, int slices = 16, int stacks = 12);
    static std::vector<LeafPlatelet3D> scatterVolumetricLeaves(const CrownControlParams& params, uint32_t seed = 42);
    static float evaluateCrownRadius(const CrownControlParams& params, float thetaRad, float heightFrac);
};

} // namespace vegetation
