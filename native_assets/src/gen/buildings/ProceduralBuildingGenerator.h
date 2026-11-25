#pragma once

#include "../IGenerator.h"
#include "BuildingTypes.h"
#include "BuildingComponents.h"
#include <memory>
#include <vector>

namespace procedural::buildings {

class ProceduralBuildingGenerator : public IGenerator {
public:
    ProceduralBuildingGenerator();
    ~ProceduralBuildingGenerator() override = default;

    const char* getName() const override { return "ProceduralBuildingGenerator"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Buildings; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const BuildingParameters& params);
    const BuildingParameters& getParameters() const { return params_; }
    BuildingStats getStats() const { return stats_; }

private:
    float getFloorScale(int32_t floor, bool isTopEdge = false) const;
    void generateSquareOrRectangularMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateOvalOrSemiOvalMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCylinderMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTriangleSemiCircleMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTriangularFlatironMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateLShapedMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generatePolygonalPrismMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices, int32_t sides);
    void generateSteppedZigguratMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    // 3 Star Designs
    void generateStarTriPointMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateStarFourPointMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateStarFivePointMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    // General Architectural Designs
    void generateTwistedHelixMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTorusRingMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCantileverOverhangMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateConicalPyramidMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    // 8 Complex Architectural Building Shapes
    void generateUShapedCourtyardMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateHShapedTwinWingMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCurvedWaveRibbonMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateDiagridExoskeletonMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateMobiusArchPortalMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateSteppedSailMarineMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateInterlockingJengaMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateFacetedCrystallineGemMesh(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

    BuildingParameters params_;
    BuildingStats stats_;
};

} // namespace procedural::buildings
