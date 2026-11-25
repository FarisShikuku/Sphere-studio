#include "ProceduralBuildingGenerator.h"
#include <cmath>
#include <algorithm>

namespace procedural::buildings {

namespace {
constexpr float kPi = 3.14159265358979323846f;

static bool shouldUseUnifiedContourRoof(RoofType rt) {
    switch (rt) {
        case RoofType::FlatParapet:
        case RoofType::GreenRoofTerrace:
        case RoofType::Mansard:
        case RoofType::Dome:
        case RoofType::PyramidalApex:
        case RoofType::ShedMonopitch:
        case RoofType::GeodesicSkylightCrown:
        case RoofType::HyperbolicParaboloidShell:
        case RoofType::ConicalSpireLantern:
        case RoofType::DiagridSailCanopy:
        case RoofType::TieredSteppedZigguratCrown:
        case RoofType::PagodaCurvedEaves:
            return true;
        default:
            return false;
    }
}

static void appendFlatCapFanXZ(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    const std::vector<Vec2>& contourXZ,
    const Vec2& centerXZ,
    float y,
    uint32_t materialId
) {
    if (contourXZ.size() < 3) return;
    Vec3 c{centerXZ.x, y, centerXZ.y};
    for (size_t i = 0; i < contourXZ.size(); ++i) {
        size_t next = (i + 1) % contourXZ.size();
        Vec3 a{contourXZ[i].x, y, contourXZ[i].y};
        Vec3 b{contourXZ[next].x, y, contourXZ[next].y};
        Vec3 crossP = (a - c).cross(b - c);
        if (crossP.y < 0.0f) std::swap(a, b);
        uint32_t baseIdx = static_cast<uint32_t>(outVerts.size());
        outVerts.push_back({c, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}, materialId});
        outVerts.push_back({a, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, materialId});
        outVerts.push_back({b, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, materialId});
        outIndices.push_back(baseIdx);
        outIndices.push_back(baseIdx + 1);
        outIndices.push_back(baseIdx + 2);
    }
}

static void appendPerimeterBayWithAperture(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    const Vec3& bayStart,
    const Vec3& bayEnd,
    float floorBottomY,
    float floorTopY,
    const Vec3& normal,
    const Vec3& tangent,
    uint32_t wallMat,
    bool hasWindow,
    float windowSillHeight,
    float windowWidth,
    float windowHeight,
    WindowType windowType,
    GlassType glassType,
    bool hasShutters = false,
    bool hasLouvres = false,
    WallMaterialType wallType = WallMaterialType::ConcreteBrutalist,
    bool isEntranceBay = false,
    DoorType doorType = DoorType::None,
    float doorWidth = 2.4f,
    float doorHeight = 2.4f,
    bool hasCanopy = false
) {
    // 1. If wall material is glass / curtain wall system, render full curtain wall bay without punched windows
    if (BuildingComponents::isCurtainWallMaterial(wallType)) {
        BuildingComponents::generateCurtainWallBay(
            outVerts, outIndices,
            bayStart, bayEnd, normal, tangent,
            floorBottomY, floorTopY,
            wallType,
            isEntranceBay, doorType, doorWidth, doorHeight,
            hasCanopy, glassType
        );
        return;
    }

    // 2. Entrance door bay for masonry/solid walls
    if (isEntranceBay && doorType != DoorType::None) {
        float bayWidth = (bayEnd - bayStart).length();
        float dw = std::min(bayWidth * 0.85f, doorWidth);
        float dh = std::min(floorTopY - floorBottomY - 0.3f, doorHeight);
        Vec3 bayCenter = (bayStart + bayEnd) * 0.5f;

        // Wall to left of door
        Vec3 wL0 = bayStart + Vec3{0.0f, floorBottomY, 0.0f};
        Vec3 wL1 = bayCenter - tangent * (dw * 0.5f) + Vec3{0.0f, floorBottomY, 0.0f};
        Vec3 wL2 = wL1 + Vec3{0.0f, floorTopY - floorBottomY, 0.0f};
        Vec3 wL3 = wL0 + Vec3{0.0f, floorTopY - floorBottomY, 0.0f};
        BuildingComponents::appendQuad(outVerts, outIndices, wL0, wL1, wL2, wL3, normal, wallMat);

        // Wall to right of door
        Vec3 wR0 = bayCenter + tangent * (dw * 0.5f) + Vec3{0.0f, floorBottomY, 0.0f};
        Vec3 wR1 = bayEnd + Vec3{0.0f, floorBottomY, 0.0f};
        Vec3 wR2 = wR1 + Vec3{0.0f, floorTopY - floorBottomY, 0.0f};
        Vec3 wR3 = wR0 + Vec3{0.0f, floorTopY - floorBottomY, 0.0f};
        BuildingComponents::appendQuad(outVerts, outIndices, wR0, wR1, wR2, wR3, normal, wallMat);

        // Wall lintel above door
        Vec3 lint0 = wL1 + Vec3{0.0f, dh, 0.0f};
        Vec3 lint1 = wR0 + Vec3{0.0f, dh, 0.0f};
        Vec3 lint2 = lint1 + Vec3{0.0f, floorTopY - floorBottomY - dh, 0.0f};
        Vec3 lint3 = lint0 + Vec3{0.0f, floorTopY - floorBottomY - dh, 0.0f};
        BuildingComponents::appendQuad(outVerts, outIndices, lint0, lint1, lint2, lint3, normal, wallMat);

        Vec3 doorGround = bayCenter + Vec3{0.0f, floorBottomY, 0.0f};
        BuildingComponents::generateDoor(outVerts, outIndices,
            doorGround, dw, dh, normal, tangent, doorType, hasCanopy);
        return;
    }

    if (!hasWindow) {
        Vec3 p0 = bayStart + Vec3{0.0f, floorBottomY, 0.0f};
        Vec3 p1 = bayEnd   + Vec3{0.0f, floorBottomY, 0.0f};
        Vec3 p2 = bayEnd   + Vec3{0.0f, floorTopY, 0.0f};
        Vec3 p3 = bayStart + Vec3{0.0f, floorTopY, 0.0f};
        BuildingComponents::appendQuad(outVerts, outIndices, p0, p1, p2, p3, normal, wallMat);
        return;
    }

    float bayWidth = (bayEnd - bayStart).length();
    float ww = std::min(bayWidth * 0.72f, windowWidth);
    float wh = std::min(floorTopY - floorBottomY - 0.9f, windowHeight);
    float sillTopY = floorBottomY + windowSillHeight;
    float winTopY = sillTopY + wh;
    Vec3 bayCenter = (bayStart + bayEnd) * 0.5f;

    // 1. Sill apron quad
    Vec3 wBot0 = bayStart + Vec3{0.0f, floorBottomY, 0.0f};
    Vec3 wBot1 = bayEnd   + Vec3{0.0f, floorBottomY, 0.0f};
    Vec3 wBot2 = bayEnd   + Vec3{0.0f, sillTopY, 0.0f};
    Vec3 wBot3 = bayStart + Vec3{0.0f, sillTopY, 0.0f};
    BuildingComponents::appendQuad(outVerts, outIndices, wBot0, wBot1, wBot2, wBot3, normal, wallMat);

    // 2. Header lintel quad
    Vec3 wTop0 = bayStart + Vec3{0.0f, winTopY, 0.0f};
    Vec3 wTop1 = bayEnd   + Vec3{0.0f, winTopY, 0.0f};
    Vec3 wTop2 = bayEnd   + Vec3{0.0f, floorTopY, 0.0f};
    Vec3 wTop3 = bayStart + Vec3{0.0f, floorTopY, 0.0f};
    BuildingComponents::appendQuad(outVerts, outIndices, wTop0, wTop1, wTop2, wTop3, normal, wallMat);

    // 3. Left jamb quad
    Vec3 jambL0 = bayStart + Vec3{0.0f, sillTopY, 0.0f};
    Vec3 jambL1 = bayCenter - tangent * (ww * 0.5f) + Vec3{0.0f, sillTopY, 0.0f};
    Vec3 jambL2 = bayCenter - tangent * (ww * 0.5f) + Vec3{0.0f, winTopY, 0.0f};
    Vec3 jambL3 = bayStart + Vec3{0.0f, winTopY, 0.0f};
    BuildingComponents::appendQuad(outVerts, outIndices, jambL0, jambL1, jambL2, jambL3, normal, wallMat);

    // 4. Right jamb quad
    Vec3 jambR0 = bayCenter + tangent * (ww * 0.5f) + Vec3{0.0f, sillTopY, 0.0f};
    Vec3 jambR1 = bayEnd   + Vec3{0.0f, sillTopY, 0.0f};
    Vec3 jambR2 = bayEnd   + Vec3{0.0f, winTopY, 0.0f};
    Vec3 jambR3 = bayCenter + tangent * (ww * 0.5f) + Vec3{0.0f, winTopY, 0.0f};
    BuildingComponents::appendQuad(outVerts, outIndices, jambR0, jambR1, jambR2, jambR3, normal, wallMat);

    // 5. Generate Window Component with outward relief so it is never buried
    Vec3 windowCenter = bayCenter + normal * 0.04f + Vec3{0.0f, (sillTopY + winTopY) * 0.5f, 0.0f};
    BuildingComponents::generateWindow(outVerts, outIndices,
        windowCenter, ww, wh, normal, tangent, windowType, glassType);

    if (hasShutters) {
        BuildingComponents::generateWindowShutters(outVerts, outIndices,
            windowCenter, ww, wh, normal, tangent);
    }
    if (hasLouvres) {
        BuildingComponents::generateSunLouvres(outVerts, outIndices,
            windowCenter + Vec3{0.0f, wh * 0.5f, 0.0f}, ww, 0.45f, normal, tangent);
    }
}

static void generateRoofOffset(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    BuildingVolumeShape shape,
    float baseWidth,
    float baseDepth,
    float baseRadius,
    float baseTopY,
    RoofType type,
    float roofHeight,
    float overhang,
    bool showClutter,
    float offsetX,
    float offsetZ
) {
    size_t startVert = outVerts.size();
    BuildingComponents::generateRoof(
        outVerts, outIndices,
        shape, baseWidth, baseDepth, baseRadius,
        baseTopY, type, roofHeight, overhang, showClutter
    );
    if (std::abs(offsetX) > 1e-4f || std::abs(offsetZ) > 1e-4f) {
        for (size_t i = startVert; i < outVerts.size(); ++i) {
            outVerts[i].position.x += offsetX;
            outVerts[i].position.z += offsetZ;
        }
    }
}
}

ProceduralBuildingGenerator::ProceduralBuildingGenerator() {
    configure(BuildingParameters{});
}

bool ProceduralBuildingGenerator::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void ProceduralBuildingGenerator::update(float /*timeSeconds*/, float /*dt*/) {
    // Static buildings: no per-frame dynamic physics updates required
}

void ProceduralBuildingGenerator::configure(const BuildingParameters& params) {
    params_ = params;
    // Normalize dimensions according to shape
    if (params_.shape == BuildingVolumeShape::Square) {
        params_.depth = params_.width;
    } else if (params_.shape == BuildingVolumeShape::Cube) {
        float cubeDim = params_.floorCount * params_.floorHeight;
        params_.width = cubeDim;
        params_.depth = cubeDim;
    } else if (params_.shape == BuildingVolumeShape::Cylinder) {
        params_.depth = params_.width;
    } else if (params_.shape == BuildingVolumeShape::Octagon || params_.shape == BuildingVolumeShape::Hexagon) {
        params_.depth = params_.width;
    }
}

GeneratedMeshPayload ProceduralBuildingGenerator::generateMesh(const GeneratorContext& /*ctx*/) {
    std::vector<BuildingVertex> vertices;
    std::vector<uint32_t> indices;

    switch (params_.shape) {
        case BuildingVolumeShape::Oval:
        case BuildingVolumeShape::SemiOval:
            generateOvalOrSemiOvalMesh(vertices, indices);
            break;
        case BuildingVolumeShape::Cylinder:
            generateCylinderMesh(vertices, indices);
            break;
        case BuildingVolumeShape::TriangleSemiCircleHybrid:
            generateTriangleSemiCircleMesh(vertices, indices);
            break;
        case BuildingVolumeShape::TriangularFlatiron:
            generateTriangularFlatironMesh(vertices, indices);
            break;
        case BuildingVolumeShape::LShaped:
            generateLShapedMesh(vertices, indices);
            break;
        case BuildingVolumeShape::Octagon:
            generatePolygonalPrismMesh(vertices, indices, 8);
            break;
        case BuildingVolumeShape::Hexagon:
            generatePolygonalPrismMesh(vertices, indices, 6);
            break;
        case BuildingVolumeShape::SteppedZiggurat:
            generateSteppedZigguratMesh(vertices, indices);
            break;
        case BuildingVolumeShape::StarTriPointY:
            generateStarTriPointMesh(vertices, indices);
            break;
        case BuildingVolumeShape::StarFourPointCross:
            generateStarFourPointMesh(vertices, indices);
            break;
        case BuildingVolumeShape::StarFivePointApex:
            generateStarFivePointMesh(vertices, indices);
            break;
        case BuildingVolumeShape::TwistedHelix:
            generateTwistedHelixMesh(vertices, indices);
            break;
        case BuildingVolumeShape::TorusRingPavilion:
            generateTorusRingMesh(vertices, indices);
            break;
        case BuildingVolumeShape::CantileverOverhang:
            generateCantileverOverhangMesh(vertices, indices);
            break;
        case BuildingVolumeShape::ConicalPyramid:
            generateConicalPyramidMesh(vertices, indices);
            break;
        case BuildingVolumeShape::UShapedCourtyard:
            generateUShapedCourtyardMesh(vertices, indices);
            break;
        case BuildingVolumeShape::HShapedTwinWing:
            generateHShapedTwinWingMesh(vertices, indices);
            break;
        case BuildingVolumeShape::CurvedWaveRibbon:
            generateCurvedWaveRibbonMesh(vertices, indices);
            break;
        case BuildingVolumeShape::DiagridExoskeletonTower:
            generateDiagridExoskeletonMesh(vertices, indices);
            break;
        case BuildingVolumeShape::MobiusArchPortal:
            generateMobiusArchPortalMesh(vertices, indices);
            break;
        case BuildingVolumeShape::SteppedSailMarineTower:
            generateSteppedSailMarineMesh(vertices, indices);
            break;
        case BuildingVolumeShape::InterlockingJengaCantilevers:
            generateInterlockingJengaMesh(vertices, indices);
            break;
        case BuildingVolumeShape::FacetedCrystallineGem:
            generateFacetedCrystallineGemMesh(vertices, indices);
            break;
        default:
            generateSquareOrRectangularMesh(vertices, indices);
            break;
    }

    // Convert to GeneratedMeshPayload
    GeneratedMeshPayload payload;
    payload.vertexStride = 9 * sizeof(float); // pos(3), normal(3), uv(2), matId(1)
    payload.vertexCount = static_cast<uint32_t>(vertices.size());
    payload.indexCount = static_cast<uint32_t>(indices.size());
    payload.vertexBuffer.reserve(vertices.size() * 9);
    payload.indexBuffer = indices;

    for (const auto& v : vertices) {
        payload.vertexBuffer.push_back(v.position.x);
        payload.vertexBuffer.push_back(v.position.y);
        payload.vertexBuffer.push_back(v.position.z);
        payload.vertexBuffer.push_back(v.normal.x);
        payload.vertexBuffer.push_back(v.normal.y);
        payload.vertexBuffer.push_back(v.normal.z);
        payload.vertexBuffer.push_back(v.uv.x);
        payload.vertexBuffer.push_back(v.uv.y);
        payload.vertexBuffer.push_back(static_cast<float>(v.materialId));
    }

    // Compute stats
    stats_.floorCount = params_.floorCount;
    stats_.totalHeightMeters = params_.getTotalHeight();
    if (params_.shape == BuildingVolumeShape::Oval) {
        stats_.footprintAreaSqM = kPi * (params_.width * 0.5f) * (params_.depth * 0.5f);
    } else if (params_.shape == BuildingVolumeShape::SemiOval) {
        stats_.footprintAreaSqM = 0.5f * kPi * (params_.width * 0.5f) * (params_.depth * 0.5f);
    } else if (params_.shape == BuildingVolumeShape::Cylinder) {
        float r = params_.width * 0.5f;
        stats_.footprintAreaSqM = kPi * r * r;
    } else if (params_.shape == BuildingVolumeShape::TriangleSemiCircleHybrid) {
        float hw = params_.width * 0.5f;
        float triArea = 0.5f * params_.width * (params_.depth * 0.55f);
        float semiArea = 0.5f * kPi * hw * (params_.depth * 0.45f);
        stats_.footprintAreaSqM = triArea + semiArea;
    } else if (params_.shape == BuildingVolumeShape::TriangularFlatiron) {
        stats_.footprintAreaSqM = 0.5f * params_.width * params_.depth;
    } else if (params_.shape == BuildingVolumeShape::LShaped) {
        stats_.footprintAreaSqM = params_.width * params_.depth * 0.65f;
    } else if (params_.shape == BuildingVolumeShape::Octagon) {
        float r = params_.width * 0.5f;
        stats_.footprintAreaSqM = 2.0f * (1.0f + std::sqrt(2.0f)) * r * r * 0.5f;
    } else if (params_.shape == BuildingVolumeShape::Hexagon) {
        float r = params_.width * 0.5f;
        stats_.footprintAreaSqM = (3.0f * std::sqrt(3.0f) * 0.5f) * r * r;
    } else if (params_.shape == BuildingVolumeShape::UShapedCourtyard) {
        stats_.footprintAreaSqM = params_.width * params_.depth * 0.64f;
    } else if (params_.shape == BuildingVolumeShape::HShapedTwinWing) {
        stats_.footprintAreaSqM = params_.width * params_.depth * 0.68f;
    } else if (params_.shape == BuildingVolumeShape::MobiusArchPortal) {
        stats_.footprintAreaSqM = params_.width * params_.depth * 0.56f;
    } else if (params_.shape == BuildingVolumeShape::SteppedSailMarineTower) {
        stats_.footprintAreaSqM = params_.width * params_.depth * 0.62f;
    } else {
        stats_.footprintAreaSqM = params_.width * params_.depth;
    }
    stats_.totalGrossFloorAreaSqM = stats_.footprintAreaSqM * params_.floorCount;
    stats_.totalWindowCount = params_.windowsPerFacade * 4 * params_.floorCount;
    stats_.vertexCount = static_cast<int32_t>(vertices.size());
    stats_.triangleCount = static_cast<int32_t>(indices.size() / 3);

    return payload;
}

float ProceduralBuildingGenerator::getFloorScale(int32_t floor, bool isTopEdge) const {
    if (params_.floorCount <= 1) return 1.0f;
    float t = (static_cast<float>(floor) + (isTopEdge ? 1.0f : 0.0f)) / static_cast<float>(params_.floorCount);
    t = std::max(0.0f, std::min(1.0f, t));

    switch (params_.stepMode) {
        case BuildingStepMode::ContinuousTaper: {
            float topRatio = std::max(0.15f, std::min(0.98f, params_.topTaperRatio));
            return 1.0f - (1.0f - topRatio) * t;
        }
        case BuildingStepMode::ConcaveCurvedTaper: {
            float topRatio = std::max(0.15f, std::min(0.98f, params_.topTaperRatio));
            float curvedT = std::sqrt(t);
            return 1.0f - (1.0f - topRatio) * curvedT;
        }
        case BuildingStepMode::SteppedBlocks: {
            int32_t tiers = std::max(2, std::min(params_.floorCount, params_.stepTiers));
            int32_t tierIndex = (floor * tiers) / params_.floorCount;
            if (tierIndex >= tiers) tierIndex = tiers - 1;
            float topRatio = std::max(0.18f, std::min(0.95f, params_.topTaperRatio));
            float rawFrac = (tiers > 1) ? (static_cast<float>(tierIndex) / static_cast<float>(tiers - 1)) : 0.0f;
            float curveExp = 0.5f + std::max(0.0f, std::min(0.5f, params_.stepShrinkFactor)) * 3.0f;
            float tierFrac = std::pow(rawFrac, curveExp);
            return 1.0f - (1.0f - topRatio) * tierFrac;
        }
        case BuildingStepMode::ExpandingCantilever: {
            int32_t tiers = std::max(2, std::min(params_.floorCount, params_.stepTiers));
            int32_t tierIndex = (floor * tiers) / params_.floorCount;
            if (tierIndex >= tiers) tierIndex = tiers - 1;
            float baseRatio = std::max(0.35f, std::min(0.85f, params_.topTaperRatio));
            float rawFrac = (tiers > 1) ? (static_cast<float>(tierIndex) / static_cast<float>(tiers - 1)) : 0.0f;
            float curveExp = 0.5f + std::max(0.0f, std::min(0.5f, params_.stepShrinkFactor)) * 3.0f;
            float tierFrac = std::pow(rawFrac, curveExp);
            return baseRatio + (1.0f - baseRatio) * tierFrac;
        }
        case BuildingStepMode::UniformStraight:
        default:
            return 1.0f;
    }
}

void ProceduralBuildingGenerator::generateSquareOrRectangularMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float totalBuildingHeight = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t glassMat = BuildingComponents::getGlassMaterial(params_.glassType);
    uint32_t deckMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);

    // 1. Foundation Plinth (Exterior vertical faces only - scaled to base floor)
    float baseScale = getFloorScale(0);
    float hw0 = params_.width * 0.5f * baseScale;
    float hd0 = params_.depth * 0.5f * baseScale;
    BuildingComponents::appendQuad(outVerts, outIndices,
        {-hw0 - 0.4f, -0.4f, hd0 + 0.4f}, {hw0 + 0.4f, -0.4f, hd0 + 0.4f},
        {hw0 + 0.4f, 0.0f, hd0 + 0.4f}, {-hw0 - 0.4f, 0.0f, hd0 + 0.4f},
        {0.0f, 0.0f, 1.0f}, foundMat);
    BuildingComponents::appendQuad(outVerts, outIndices,
        {hw0 + 0.4f, -0.4f, -hd0 - 0.4f}, {-hw0 - 0.4f, -0.4f, -hd0 - 0.4f},
        {-hw0 - 0.4f, 0.0f, -hd0 - 0.4f}, {hw0 + 0.4f, 0.0f, -hd0 - 0.4f},
        {0.0f, 0.0f, -1.0f}, foundMat);
    BuildingComponents::appendQuad(outVerts, outIndices,
        {-hw0 - 0.4f, -0.4f, -hd0 - 0.4f}, {-hw0 - 0.4f, -0.4f, hd0 + 0.4f},
        {-hw0 - 0.4f, 0.0f, hd0 + 0.4f}, {-hw0 - 0.4f, 0.0f, -hd0 - 0.4f},
        {-1.0f, 0.0f, 0.0f}, foundMat);
    BuildingComponents::appendQuad(outVerts, outIndices,
        {hw0 + 0.4f, -0.4f, hd0 + 0.4f}, {hw0 + 0.4f, -0.4f, -hd0 - 0.4f},
        {hw0 + 0.4f, 0.0f, -hd0 - 0.4f}, {hw0 + 0.4f, 0.0f, hd0 + 0.4f},
        {1.0f, 0.0f, 0.0f}, foundMat);

    // 2. Floors, Facades, Stepped Blocks & Corner Geometry
    struct FacadeDef {
        Vec3 start;
        Vec3 end;
        Vec3 normal;
        Vec3 tangent;
        float length;
        bool isFront;
    };

    for (int32_t floor = 0; floor < params_.floorCount; ++floor) {
        float floorBottomY = floor * params_.floorHeight;
        float floorTopY = (floor + 1) * params_.floorHeight;
        float scale = getFloorScale(floor);
        float hw = params_.width * 0.5f * scale;
        float hd = params_.depth * 0.5f * scale;

        // Setback Terrace Ledge (when previous lower block is wider than current upper block)
        if (floor > 0) {
            float prevScale = getFloorScale(floor - 1);
            if (prevScale > scale + 0.005f) {
                float prevHW = params_.width * 0.5f * prevScale;
                float prevHD = params_.depth * 0.5f * prevScale;

                // Horizontal setback terrace deck surface at floorBottomY
                BuildingComponents::appendQuad(outVerts, outIndices,
                    {-prevHW, floorBottomY, prevHD}, {prevHW, floorBottomY, prevHD},
                    {hw, floorBottomY, hd}, {-hw, floorBottomY, hd},
                    {0.0f, 1.0f, 0.0f}, deckMat);
                BuildingComponents::appendQuad(outVerts, outIndices,
                    {prevHW, floorBottomY, -prevHD}, {-prevHW, floorBottomY, -prevHD},
                    {-hw, floorBottomY, -hd}, {hw, floorBottomY, -hd},
                    {0.0f, 1.0f, 0.0f}, deckMat);
                BuildingComponents::appendQuad(outVerts, outIndices,
                    {prevHW, floorBottomY, prevHD}, {prevHW, floorBottomY, -prevHD},
                    {hw, floorBottomY, -hd}, {hw, floorBottomY, hd},
                    {0.0f, 1.0f, 0.0f}, deckMat);
                BuildingComponents::appendQuad(outVerts, outIndices,
                    {-prevHW, floorBottomY, -prevHD}, {-prevHW, floorBottomY, prevHD},
                    {-hw, floorBottomY, hd}, {-hw, floorBottomY, -hd},
                    {0.0f, 1.0f, 0.0f}, deckMat);

                // Safety terrace railing / parapet along outer perimeter
                if (params_.stepTerraceRailing) {
                    float railH = 0.9f;
                    uint32_t railMat = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-prevHW, floorBottomY, prevHD}, {prevHW, floorBottomY, prevHD},
                        {prevHW, floorBottomY + railH, prevHD}, {-prevHW, floorBottomY + railH, prevHD},
                        {0.0f, 0.0f, 1.0f}, railMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {prevHW, floorBottomY, -prevHD}, {-prevHW, floorBottomY, -prevHD},
                        {-prevHW, floorBottomY + railH, -prevHD}, {prevHW, floorBottomY + railH, -prevHD},
                        {0.0f, 0.0f, -1.0f}, railMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {prevHW, floorBottomY, prevHD}, {prevHW, floorBottomY, -prevHD},
                        {prevHW, floorBottomY + railH, -prevHD}, {prevHW, floorBottomY + railH, prevHD},
                        {1.0f, 0.0f, 0.0f}, railMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-prevHW, floorBottomY, -prevHD}, {-prevHW, floorBottomY, prevHD},
                        {-prevHW, floorBottomY + railH, prevHD}, {-prevHW, floorBottomY + railH, -prevHD},
                        {-1.0f, 0.0f, 0.0f}, railMat);
                }
            } else if (scale > prevScale + 0.005f) {
                // Underside soffit for cantilevered wider upper block
                float prevHW = params_.width * 0.5f * prevScale;
                float prevHD = params_.depth * 0.5f * prevScale;
                BuildingComponents::appendQuad(outVerts, outIndices,
                    {-prevHW, floorBottomY, prevHD}, {prevHW, floorBottomY, prevHD},
                    {hw, floorBottomY, hd}, {-hw, floorBottomY, hd},
                    {0.0f, -1.0f, 0.0f}, foundMat);
                BuildingComponents::appendQuad(outVerts, outIndices,
                    {prevHW, floorBottomY, -prevHD}, {-prevHW, floorBottomY, -prevHD},
                    {-hw, floorBottomY, -hd}, {hw, floorBottomY, -hd},
                    {0.0f, -1.0f, 0.0f}, foundMat);
                BuildingComponents::appendQuad(outVerts, outIndices,
                    {prevHW, floorBottomY, prevHD}, {prevHW, floorBottomY, -prevHD},
                    {hw, floorBottomY, -hd}, {hw, floorBottomY, hd},
                    {0.0f, -1.0f, 0.0f}, foundMat);
                BuildingComponents::appendQuad(outVerts, outIndices,
                    {-prevHW, floorBottomY, -prevHD}, {-prevHW, floorBottomY, prevHD},
                    {-hw, floorBottomY, hd}, {-hw, floorBottomY, -hd},
                    {0.0f, -1.0f, 0.0f}, foundMat);
            }
        }

        // Calculate corner size for current floor
        float maxCorner = std::min(hw, hd) * 0.40f;
        float c = (params_.cornerType == BuildingCornerType::Sharp90) ? 0.0f :
                  std::min(params_.cornerSize * scale, maxCorner);
        if (c < 0.1f) c = 0.0f;

        // Floor slab dividing line / trim trimmed to corner span
        if (floor > 0) {
            float bandH = params_.slabThickness;
            float b0 = floorBottomY - bandH * 0.5f;
            float b1 = floorBottomY + bandH * 0.5f;
            float spanTrimX = std::max(0.0f, hw - c);
            float spanTrimZ = std::max(0.0f, hd - c);
            float pad = (c > 0.05f) ? 0.0f : 0.05f;
            BuildingComponents::appendQuad(outVerts, outIndices,
                {-spanTrimX - pad, b0, hd + 0.05f}, {spanTrimX + pad, b0, hd + 0.05f},
                {spanTrimX + pad, b1, hd + 0.05f}, {-spanTrimX - pad, b1, hd + 0.05f},
                {0.0f, 0.0f, 1.0f}, foundMat);
            BuildingComponents::appendQuad(outVerts, outIndices,
                {spanTrimX + pad, b0, -hd - 0.05f}, {-spanTrimX - pad, b0, -hd - 0.05f},
                {-spanTrimX - pad, b1, -hd - 0.05f}, {spanTrimX + pad, b1, -hd - 0.05f},
                {0.0f, 0.0f, -1.0f}, foundMat);
            BuildingComponents::appendQuad(outVerts, outIndices,
                {-hw - 0.05f, b0, -spanTrimZ - pad}, {-hw - 0.05f, b0, spanTrimZ + pad},
                {-hw - 0.05f, b1, spanTrimZ + pad}, {-hw - 0.05f, b1, -spanTrimZ - pad},
                {-1.0f, 0.0f, 0.0f}, foundMat);
            BuildingComponents::appendQuad(outVerts, outIndices,
                {hw + 0.05f, b0, spanTrimZ + pad}, {hw + 0.05f, b0, -spanTrimZ - pad},
                {hw + 0.05f, b1, -spanTrimZ - pad}, {hw + 0.05f, b1, spanTrimZ + pad},
                {1.0f, 0.0f, 0.0f}, foundMat);
        }

        // Define 4 main facade spans between the corner profiles
        std::vector<FacadeDef> facades = {
            // Front (+Z)
            {{-hw + c, 0.0f, hd}, {hw - c, 0.0f, hd}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, (hw - c) * 2.0f, true},
            // Right (+X)
            {{hw, 0.0f, hd - c}, {hw, 0.0f, -hd + c}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, (hd - c) * 2.0f, false},
            // Back (-Z)
            {{hw - c, 0.0f, -hd}, {-hw + c, 0.0f, -hd}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, (hw - c) * 2.0f, false},
            // Left (-X)
            {{-hw, 0.0f, -hd + c}, {-hw, 0.0f, hd - c}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, (hd - c) * 2.0f, false}
        };

        // Generate each of the 4 facades
        for (const auto& facade : facades) {
            int32_t numBays = std::max(1, params_.windowsPerFacade);
            float bayWidth = facade.length / static_cast<float>(numBays);

            for (int32_t bay = 0; bay < numBays; ++bay) {
                float bayStartFrac = static_cast<float>(bay) / numBays;
                float bayEndFrac = static_cast<float>(bay + 1) / numBays;
                Vec3 bayStart = facade.start + facade.tangent * (bayStartFrac * facade.length);
                Vec3 bayEnd = facade.start + facade.tangent * (bayEndFrac * facade.length);
                Vec3 bayCenter = (bayStart + bayEnd) * 0.5f;

                bool isEntranceBay = (floor == 0 && facade.isFront && bay == (numBays / 2));

                if (BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
                    BuildingComponents::generateCurtainWallBay(outVerts, outIndices,
                        bayStart, bayEnd, facade.normal, facade.tangent,
                        floorBottomY, floorTopY, params_.wallMaterial,
                        isEntranceBay, params_.doorType, params_.doorWidth, params_.doorHeight,
                        params_.hasEntranceCanopy, params_.glassType);
                } else if (isEntranceBay && params_.doorType != DoorType::None) {
                    float dw = params_.doorWidth;
                    float dh = params_.doorHeight;

                    // Wall to left of door
                    Vec3 wallL0 = bayStart + Vec3{0.0f, floorBottomY, 0.0f};
                    Vec3 wallL1 = bayCenter - facade.tangent * (dw * 0.5f) + Vec3{0.0f, floorBottomY, 0.0f};
                    Vec3 wallL2 = wallL1 + Vec3{0.0f, params_.floorHeight, 0.0f};
                    Vec3 wallL3 = wallL0 + Vec3{0.0f, params_.floorHeight, 0.0f};
                    BuildingComponents::appendQuad(outVerts, outIndices, wallL0, wallL1, wallL2, wallL3, facade.normal, wallMat);

                    // Wall to right of door
                    Vec3 wallR0 = bayCenter + facade.tangent * (dw * 0.5f) + Vec3{0.0f, floorBottomY, 0.0f};
                    Vec3 wallR1 = bayEnd + Vec3{0.0f, floorBottomY, 0.0f};
                    Vec3 wallR2 = wallR1 + Vec3{0.0f, params_.floorHeight, 0.0f};
                    Vec3 wallR3 = wallR0 + Vec3{0.0f, params_.floorHeight, 0.0f};
                    BuildingComponents::appendQuad(outVerts, outIndices, wallR0, wallR1, wallR2, wallR3, facade.normal, wallMat);

                    // Wall lintel above door
                    Vec3 lint0 = wallL1 + Vec3{0.0f, dh, 0.0f};
                    Vec3 lint1 = wallR0 + Vec3{0.0f, dh, 0.0f};
                    Vec3 lint2 = lint1 + Vec3{0.0f, params_.floorHeight - dh, 0.0f};
                    Vec3 lint3 = lint0 + Vec3{0.0f, params_.floorHeight - dh, 0.0f};
                    BuildingComponents::appendQuad(outVerts, outIndices, lint0, lint1, lint2, lint3, facade.normal, wallMat);

                    // Generate Door Component
                    Vec3 doorGround = bayCenter + Vec3{0.0f, floorBottomY, 0.0f};
                    BuildingComponents::generateDoor(outVerts, outIndices,
                        doorGround, dw, dh, facade.normal, facade.tangent,
                        params_.doorType, params_.hasEntranceCanopy);

                } else {
                    float ww = std::min(bayWidth * 0.75f, params_.windowWidth);
                    float wh = std::min(params_.floorHeight - 1.0f, params_.windowHeight);
                    float sillH = params_.windowSillHeight;

                    float sillBottomY = floorBottomY;
                    float sillTopY = floorBottomY + sillH;
                    float winTopY = sillTopY + wh;

                    // Wall under window (sill apron)
                    Vec3 wBot0 = bayStart + Vec3{0.0f, sillBottomY, 0.0f};
                    Vec3 wBot1 = bayEnd + Vec3{0.0f, sillBottomY, 0.0f};
                    Vec3 wBot2 = bayEnd + Vec3{0.0f, sillTopY, 0.0f};
                    Vec3 wBot3 = bayStart + Vec3{0.0f, sillTopY, 0.0f};
                    BuildingComponents::appendQuad(outVerts, outIndices, wBot0, wBot1, wBot2, wBot3, facade.normal, wallMat);

                    // Wall above window (header lintel)
                    Vec3 wTop0 = bayStart + Vec3{0.0f, winTopY, 0.0f};
                    Vec3 wTop1 = bayEnd + Vec3{0.0f, winTopY, 0.0f};
                    Vec3 wTop2 = bayEnd + Vec3{0.0f, floorTopY, 0.0f};
                    Vec3 wTop3 = bayStart + Vec3{0.0f, floorTopY, 0.0f};
                    BuildingComponents::appendQuad(outVerts, outIndices, wTop0, wTop1, wTop2, wTop3, facade.normal, wallMat);

                    // Wall jambs (left and right of window)
                    Vec3 jambL0 = bayStart + Vec3{0.0f, sillTopY, 0.0f};
                    Vec3 jambL1 = bayCenter - facade.tangent * (ww * 0.5f) + Vec3{0.0f, sillTopY, 0.0f};
                    Vec3 jambL2 = bayCenter - facade.tangent * (ww * 0.5f) + Vec3{0.0f, winTopY, 0.0f};
                    Vec3 jambL3 = bayStart + Vec3{0.0f, winTopY, 0.0f};
                    BuildingComponents::appendQuad(outVerts, outIndices, jambL0, jambL1, jambL2, jambL3, facade.normal, wallMat);

                    Vec3 jambR0 = bayCenter + facade.tangent * (ww * 0.5f) + Vec3{0.0f, sillTopY, 0.0f};
                    Vec3 jambR1 = bayEnd + Vec3{0.0f, sillTopY, 0.0f};
                    Vec3 jambR2 = bayEnd + Vec3{0.0f, winTopY, 0.0f};
                    Vec3 jambR3 = bayCenter + facade.tangent * (ww * 0.5f) + Vec3{0.0f, winTopY, 0.0f};
                    BuildingComponents::appendQuad(outVerts, outIndices, jambR0, jambR1, jambR2, jambR3, facade.normal, wallMat);

                    // Generate Window Component
                    Vec3 windowCenter = bayCenter + Vec3{0.0f, (sillTopY + winTopY) * 0.5f, 0.0f};
                    BuildingComponents::generateWindow(outVerts, outIndices,
                        windowCenter, ww, wh, facade.normal, facade.tangent,
                        params_.windowType, params_.glassType);

                    if (params_.hasWindowShutters) {
                        BuildingComponents::generateWindowShutters(outVerts, outIndices,
                            windowCenter, ww, wh, facade.normal, facade.tangent);
                    }
                    if (params_.hasSunLouvres) {
                        BuildingComponents::generateSunLouvres(outVerts, outIndices,
                            windowCenter + Vec3{0.0f, wh * 0.5f, 0.0f}, ww, 0.45f, facade.normal, facade.tangent);
                    }
                    if (floor > 0 && params_.hasBalconies && bay % 2 == 0) {
                        BuildingComponents::generateBalcony(outVerts, outIndices,
                            bayCenter + Vec3{0.0f, floorBottomY, 0.0f},
                            ww + 0.4f, params_.balconyDepth, facade.normal, facade.tangent);
                    }
                }
            }
        }

        // =====================================================================
        // Generate Architectural Corner Features
        // =====================================================================
        if (c > 0.05f) {
            uint32_t cornerMat = BuildingComponents::isCurtainWallMaterial(params_.wallMaterial) ? glassMat : wallMat;
            uint32_t trimMat = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);

            switch (params_.cornerType) {
                case BuildingCornerType::Chamfer45: {
                    // 4 diagonal beveled corner facets
                    // Front-Right
                    Vec3 nFR = Vec3{1.0f, 0.0f, 1.0f}.normalized();
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c, floorBottomY, hd}, {hw, floorBottomY, hd - c},
                        {hw, floorTopY, hd - c}, {hw - c, floorTopY, hd},
                        nFR, cornerMat);
                    // Back-Right
                    Vec3 nBR = Vec3{1.0f, 0.0f, -1.0f}.normalized();
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw, floorBottomY, -hd + c}, {hw - c, floorBottomY, -hd},
                        {hw - c, floorTopY, -hd}, {hw, floorTopY, -hd + c},
                        nBR, cornerMat);
                    // Back-Left
                    Vec3 nBL = Vec3{-1.0f, 0.0f, -1.0f}.normalized();
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c, floorBottomY, -hd}, {-hw, floorBottomY, -hd + c},
                        {-hw, floorTopY, -hd + c}, {-hw + c, floorTopY, -hd},
                        nBL, cornerMat);
                    // Front-Left
                    Vec3 nFL = Vec3{-1.0f, 0.0f, 1.0f}.normalized();
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw, floorBottomY, hd - c}, {-hw + c, floorBottomY, hd},
                        {-hw + c, floorTopY, hd}, {-hw, floorTopY, hd - c},
                        nFL, cornerMat);
                    break;
                }
                case BuildingCornerType::RoundedFillet: {
                    // Smooth radial quarter-circle arcs (4 radial segments per corner)
                    int segs = 4;
                    struct ArcCorner {
                        float cx, cz;
                        float startAngle;
                    };
                    ArcCorner corners[4] = {
                        {hw - c, hd - c, 0.0f},
                        {hw - c, -hd + c, -kPi * 0.5f},
                        {-hw + c, -hd + c, -kPi},
                        {-hw + c, hd - c, kPi * 0.5f}
                    };
                    for (int k = 0; k < 4; ++k) {
                        for (int s = 0; s < segs; ++s) {
                            float a0 = corners[k].startAngle + (static_cast<float>(s) / segs) * (kPi * 0.5f);
                            float a1 = corners[k].startAngle + (static_cast<float>(s + 1) / segs) * (kPi * 0.5f);
                            Vec3 p0{corners[k].cx + c * std::cos(a0), floorBottomY, corners[k].cz + c * std::sin(a0)};
                            Vec3 p1{corners[k].cx + c * std::cos(a1), floorBottomY, corners[k].cz + c * std::sin(a1)};
                            Vec3 p2{corners[k].cx + c * std::cos(a1), floorTopY, corners[k].cz + c * std::sin(a1)};
                            Vec3 p3{corners[k].cx + c * std::cos(a0), floorTopY, corners[k].cz + c * std::sin(a0)};
                            Vec3 midNorm = Vec3{std::cos((a0 + a1) * 0.5f), 0.0f, std::sin((a0 + a1) * 0.5f)}.normalized();
                            BuildingComponents::appendQuad(outVerts, outIndices, p0, p1, p2, p3, midNorm, cornerMat);
                        }
                    }
                    break;
                }
                case BuildingCornerType::InsetNotch: {
                    // Stepped-in L-notch with corner terrace deck and recessed corner glass
                    // Front-Right Notch
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c, floorBottomY, hd}, {hw - c, floorBottomY, hd - c},
                        {hw - c, floorTopY, hd - c}, {hw - c, floorTopY, hd},
                        {-1.0f, 0.0f, 0.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c, floorBottomY, hd - c}, {hw, floorBottomY, hd - c},
                        {hw, floorTopY, hd - c}, {hw - c, floorTopY, hd - c},
                        {0.0f, 0.0f, 1.0f}, cornerMat);
                    // Back-Right Notch
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw, floorBottomY, -hd + c}, {hw - c, floorBottomY, -hd + c},
                        {hw - c, floorTopY, -hd + c}, {hw, floorTopY, -hd + c},
                        {0.0f, 0.0f, -1.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c, floorBottomY, -hd + c}, {hw - c, floorBottomY, -hd},
                        {hw - c, floorTopY, -hd}, {hw - c, floorTopY, -hd + c},
                        {1.0f, 0.0f, 0.0f}, cornerMat);
                    // Back-Left Notch
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c, floorBottomY, -hd}, {-hw + c, floorBottomY, -hd + c},
                        {-hw + c, floorTopY, -hd + c}, {-hw + c, floorTopY, -hd},
                        {1.0f, 0.0f, 0.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c, floorBottomY, -hd + c}, {-hw, floorBottomY, -hd + c},
                        {-hw, floorTopY, -hd + c}, {-hw + c, floorTopY, -hd + c},
                        {0.0f, 0.0f, -1.0f}, cornerMat);
                    // Front-Left Notch
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw, floorBottomY, hd - c}, {-hw + c, floorBottomY, hd - c},
                        {-hw + c, floorTopY, hd - c}, {-hw, floorTopY, hd - c},
                        {0.0f, 0.0f, 1.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c, floorBottomY, hd - c}, {-hw + c, floorBottomY, hd},
                        {-hw + c, floorTopY, hd}, {-hw + c, floorTopY, hd - c},
                        {-1.0f, 0.0f, 0.0f}, cornerMat);
                    break;
                }
                case BuildingCornerType::SteppedDoubleNotch: {
                    // Two-step Art Deco stepped setback per corner
                    float c2 = c * 0.5f;
                    // FR Corner
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c, floorBottomY, hd}, {hw - c, floorBottomY, hd - c2},
                        {hw - c, floorTopY, hd - c2}, {hw - c, floorTopY, hd},
                        {-1.0f, 0.0f, 0.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c, floorBottomY, hd - c2}, {hw - c2, floorBottomY, hd - c2},
                        {hw - c2, floorTopY, hd - c2}, {hw - c, floorTopY, hd - c2},
                        {0.0f, 0.0f, 1.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c2, floorBottomY, hd - c2}, {hw - c2, floorBottomY, hd - c},
                        {hw - c2, floorTopY, hd - c}, {hw - c2, floorTopY, hd - c2},
                        {-1.0f, 0.0f, 0.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c2, floorBottomY, hd - c}, {hw, floorBottomY, hd - c},
                        {hw, floorTopY, hd - c}, {hw - c2, floorTopY, hd - c},
                        {0.0f, 0.0f, 1.0f}, cornerMat);

                    // BR Corner
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw, floorBottomY, -hd + c}, {hw - c2, floorBottomY, -hd + c},
                        {hw - c2, floorTopY, -hd + c}, {hw, floorTopY, -hd + c},
                        {0.0f, 0.0f, -1.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c2, floorBottomY, -hd + c}, {hw - c2, floorBottomY, -hd + c2},
                        {hw - c2, floorTopY, -hd + c2}, {hw - c2, floorTopY, -hd + c},
                        {1.0f, 0.0f, 0.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c2, floorBottomY, -hd + c2}, {hw - c, floorBottomY, -hd + c2},
                        {hw - c, floorTopY, -hd + c2}, {hw - c2, floorTopY, -hd + c2},
                        {0.0f, 0.0f, -1.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c, floorBottomY, -hd + c2}, {hw - c, floorBottomY, -hd},
                        {hw - c, floorTopY, -hd}, {hw - c, floorTopY, -hd + c2},
                        {1.0f, 0.0f, 0.0f}, cornerMat);

                    // BL Corner
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c, floorBottomY, -hd}, {-hw + c, floorBottomY, -hd + c2},
                        {-hw + c, floorTopY, -hd + c2}, {-hw + c, floorTopY, -hd},
                        {1.0f, 0.0f, 0.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c, floorBottomY, -hd + c2}, {-hw + c2, floorBottomY, -hd + c2},
                        {-hw + c2, floorTopY, -hd + c2}, {-hw + c, floorTopY, -hd + c2},
                        {0.0f, 0.0f, -1.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c2, floorBottomY, -hd + c2}, {-hw + c2, floorBottomY, -hd + c},
                        {-hw + c2, floorTopY, -hd + c}, {-hw + c2, floorTopY, -hd + c2},
                        {1.0f, 0.0f, 0.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c2, floorBottomY, -hd + c}, {-hw, floorBottomY, -hd + c},
                        {-hw, floorTopY, -hd + c}, {-hw + c2, floorTopY, -hd + c},
                        {0.0f, 0.0f, -1.0f}, cornerMat);

                    // FL Corner
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw, floorBottomY, hd - c}, {-hw + c2, floorBottomY, hd - c},
                        {-hw + c2, floorTopY, hd - c}, {-hw, floorTopY, hd - c},
                        {0.0f, 0.0f, 1.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c2, floorBottomY, hd - c}, {-hw + c2, floorBottomY, hd - c2},
                        {-hw + c2, floorTopY, hd - c2}, {-hw + c2, floorTopY, hd - c},
                        {-1.0f, 0.0f, 0.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c2, floorBottomY, hd - c2}, {-hw + c, floorBottomY, hd - c2},
                        {-hw + c, floorTopY, hd - c2}, {-hw + c2, floorTopY, hd - c2},
                        {0.0f, 0.0f, 1.0f}, cornerMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c, floorBottomY, hd - c2}, {-hw + c, floorBottomY, hd},
                        {-hw + c, floorTopY, hd}, {-hw + c, floorTopY, hd - c2},
                        {-1.0f, 0.0f, 0.0f}, cornerMat);
                    break;
                }
                case BuildingCornerType::CornerColumn: {
                    // Structural cylindrical pillar at each corner with recessed corner glass
                    float colR = c * 0.35f;
                    float colOffX = hw - c * 0.45f;
                    float colOffZ = hd - c * 0.45f;
                    Vec3 colLocs[4] = {
                        {colOffX, 0.0f, colOffZ},
                        {colOffX, 0.0f, -colOffZ},
                        {-colOffX, 0.0f, -colOffZ},
                        {-colOffX, 0.0f, colOffZ}
                    };
                    for (int k = 0; k < 4; ++k) {
                        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                            {colLocs[k].x, floorBottomY, colLocs[k].z},
                            {colLocs[k].x, floorTopY, colLocs[k].z},
                            colR, colR, 12, trimMat, false, false);
                    }
                    // Recessed corner glass behind the column
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c, floorBottomY, hd}, {hw, floorBottomY, hd - c},
                        {hw, floorTopY, hd - c}, {hw - c, floorTopY, hd},
                        Vec3{1.0f, 0.0f, 1.0f}.normalized(), glassMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw, floorBottomY, -hd + c}, {hw - c, floorBottomY, -hd},
                        {hw - c, floorTopY, -hd}, {hw, floorTopY, -hd + c},
                        Vec3{1.0f, 0.0f, -1.0f}.normalized(), glassMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c, floorBottomY, -hd}, {-hw, floorBottomY, -hd + c},
                        {-hw, floorTopY, -hd + c}, {-hw + c, floorTopY, -hd},
                        Vec3{-1.0f, 0.0f, -1.0f}.normalized(), glassMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw, floorBottomY, hd - c}, {-hw + c, floorBottomY, hd},
                        {-hw + c, floorTopY, hd}, {-hw, floorTopY, hd - c},
                        Vec3{-1.0f, 0.0f, 1.0f}.normalized(), glassMat);
                    break;
                }
                case BuildingCornerType::GlazedSlotReveal: {
                    // Recessed vertical glass slot reveal between facade walls
                    float slotDepth = 0.35f;
                    // FR Slot
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c, floorBottomY, hd}, {hw - c, floorBottomY, hd - slotDepth},
                        {hw - c, floorTopY, hd - slotDepth}, {hw - c, floorTopY, hd},
                        {-1.0f, 0.0f, 0.0f}, wallMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - slotDepth, floorBottomY, hd - c}, {hw, floorBottomY, hd - c},
                        {hw, floorTopY, hd - c}, {hw - slotDepth, floorTopY, hd - c},
                        {0.0f, 0.0f, 1.0f}, wallMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c, floorBottomY, hd - slotDepth}, {hw - slotDepth, floorBottomY, hd - c},
                        {hw - slotDepth, floorTopY, hd - c}, {hw - c, floorTopY, hd - slotDepth},
                        Vec3{1.0f, 0.0f, 1.0f}.normalized(), glassMat);

                    // BR Slot
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw, floorBottomY, -hd + c}, {hw - slotDepth, floorBottomY, -hd + c},
                        {hw - slotDepth, floorTopY, -hd + c}, {hw, floorTopY, -hd + c},
                        {0.0f, 0.0f, -1.0f}, wallMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - c, floorBottomY, -hd + slotDepth}, {hw - c, floorBottomY, -hd},
                        {hw - c, floorTopY, -hd}, {hw - c, floorTopY, -hd + slotDepth},
                        {1.0f, 0.0f, 0.0f}, wallMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {hw - slotDepth, floorBottomY, -hd + c}, {hw - c, floorBottomY, -hd + slotDepth},
                        {hw - c, floorTopY, -hd + slotDepth}, {hw - slotDepth, floorTopY, -hd + c},
                        Vec3{1.0f, 0.0f, -1.0f}.normalized(), glassMat);

                    // BL Slot
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c, floorBottomY, -hd}, {-hw + c, floorBottomY, -hd + slotDepth},
                        {-hw + c, floorTopY, -hd + slotDepth}, {-hw + c, floorTopY, -hd},
                        {1.0f, 0.0f, 0.0f}, wallMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + slotDepth, floorBottomY, -hd + c}, {-hw, floorBottomY, -hd + c},
                        {-hw, floorTopY, -hd + c}, {-hw + slotDepth, floorTopY, -hd + c},
                        {0.0f, 0.0f, -1.0f}, wallMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c, floorBottomY, -hd + slotDepth}, {-hw + slotDepth, floorBottomY, -hd + c},
                        {-hw + slotDepth, floorTopY, -hd + c}, {-hw + c, floorTopY, -hd + slotDepth},
                        Vec3{-1.0f, 0.0f, -1.0f}.normalized(), glassMat);

                    // FL Slot
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw, floorBottomY, hd - c}, {-hw + slotDepth, floorBottomY, hd - c},
                        {-hw + slotDepth, floorTopY, hd - c}, {-hw, floorTopY, hd - c},
                        {0.0f, 0.0f, 1.0f}, wallMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + c, floorBottomY, hd - slotDepth}, {-hw + c, floorBottomY, hd},
                        {-hw + c, floorTopY, hd}, {-hw + c, floorTopY, hd - slotDepth},
                        {-1.0f, 0.0f, 0.0f}, wallMat);
                    BuildingComponents::appendQuad(outVerts, outIndices,
                        {-hw + slotDepth, floorBottomY, hd - c}, {-hw + c, floorBottomY, hd - slotDepth},
                        {-hw + c, floorTopY, hd - slotDepth}, {-hw + slotDepth, floorTopY, hd - c},
                        Vec3{-1.0f, 0.0f, 1.0f}.normalized(), glassMat);
                    break;
                }
                default:
                    break;
            }
        }
    }

    // Ground colonnade pillars
    if (params_.hasGroundPillars) {
        float scale0 = getFloorScale(0);
        float hwG = params_.width * 0.5f * scale0;
        float hdG = params_.depth * 0.5f * scale0;
        BuildingComponents::generateGroundPillars(outVerts, outIndices,
            {-hwG, 0.0f, hdG}, {hwG, 0.0f, hdG}, params_.floorHeight, params_.pillarCount, {0.0f, 0.0f, 1.0f}, wallMat);
        BuildingComponents::generateGroundPillars(outVerts, outIndices,
            {hwG, 0.0f, -hdG}, {-hwG, 0.0f, -hdG}, params_.floorHeight, params_.pillarCount, {0.0f, 0.0f, -1.0f}, wallMat);
    }

    // Cornice band & exact top-floor contour (matching sculpted corners & top floor wall perimeter)
    float topScale = getFloorScale(params_.floorCount - 1, false);
    float topW = params_.width * topScale;
    float topD = params_.depth * topScale;
    float hwTop = topW * 0.5f;
    float hdTop = topD * 0.5f;
    float maxCornerTop = std::min(hwTop, hdTop) * 0.40f;
    float cTop = (params_.cornerType == BuildingCornerType::Sharp90) ? 0.0f :
                 std::min(params_.cornerSize * topScale, maxCornerTop);
    if (cTop < 0.1f) cTop = 0.0f;

    std::vector<Vec2> topContour;
    if (cTop > 0.05f) {
        switch (params_.cornerType) {
            case BuildingCornerType::Chamfer45:
            case BuildingCornerType::CornerColumn: {
                topContour = {
                    {-hwTop + cTop, hdTop}, {hwTop - cTop, hdTop},
                    {hwTop, hdTop - cTop}, {hwTop, -hdTop + cTop},
                    {hwTop - cTop, -hdTop}, {-hwTop + cTop, -hdTop},
                    {-hwTop, -hdTop + cTop}, {-hwTop, hdTop - cTop}
                };
                break;
            }
            case BuildingCornerType::RoundedFillet: {
                const int segs = 4;
                struct ArcCorner { float cx, cz, startAngle; };
                ArcCorner arcCorners[4] = {
                    {hwTop - cTop, hdTop - cTop, kPi * 0.5f},
                    {hwTop - cTop, -hdTop + cTop, 0.0f},
                    {-hwTop + cTop, -hdTop + cTop, -kPi * 0.5f},
                    {-hwTop + cTop, hdTop - cTop, -kPi}
                };
                for (int k = 0; k < 4; ++k) {
                    for (int s = 0; s <= segs; ++s) {
                        float a = arcCorners[k].startAngle - (static_cast<float>(s) / segs) * (kPi * 0.5f);
                        topContour.push_back(Vec2{
                            arcCorners[k].cx + cTop * std::cos(a),
                            arcCorners[k].cz + cTop * std::sin(a)
                        });
                    }
                }
                break;
            }
            case BuildingCornerType::InsetNotch: {
                topContour = {
                    {-hwTop + cTop, hdTop}, {hwTop - cTop, hdTop},
                    {hwTop - cTop, hdTop - cTop}, {hwTop, hdTop - cTop},
                    {hwTop, -hdTop + cTop}, {hwTop - cTop, -hdTop + cTop},
                    {hwTop - cTop, -hdTop}, {-hwTop + cTop, -hdTop},
                    {-hwTop + cTop, -hdTop + cTop}, {-hwTop, -hdTop + cTop},
                    {-hwTop, hdTop - cTop}, {-hwTop + cTop, hdTop - cTop}
                };
                break;
            }
            case BuildingCornerType::SteppedDoubleNotch: {
                float c2 = cTop * 0.5f;
                topContour = {
                    {-hwTop + cTop, hdTop}, {hwTop - cTop, hdTop},
                    {hwTop - cTop, hdTop - c2}, {hwTop - c2, hdTop - c2},
                    {hwTop - c2, hdTop - cTop}, {hwTop, hdTop - cTop},
                    {hwTop, -hdTop + cTop}, {hwTop - c2, -hdTop + cTop},
                    {hwTop - c2, -hdTop + c2}, {hwTop - cTop, -hdTop + c2},
                    {hwTop - cTop, -hdTop}, {-hwTop + cTop, -hdTop},
                    {-hwTop + cTop, -hdTop + c2}, {-hwTop + c2, -hdTop + c2},
                    {-hwTop + c2, -hdTop + cTop}, {-hwTop, -hdTop + cTop},
                    {-hwTop, hdTop - cTop}, {-hwTop + c2, hdTop - cTop},
                    {-hwTop + c2, hdTop - c2}, {-hwTop + cTop, hdTop - c2}
                };
                break;
            }
            case BuildingCornerType::GlazedSlotReveal: {
                float sd = 0.35f;
                topContour = {
                    {-hwTop + cTop, hdTop}, {hwTop - cTop, hdTop},
                    {hwTop - cTop, hdTop - sd}, {hwTop - sd, hdTop - cTop},
                    {hwTop, hdTop - cTop}, {hwTop, -hdTop + cTop},
                    {hwTop - sd, -hdTop + cTop}, {hwTop - cTop, -hdTop + sd},
                    {hwTop - cTop, -hdTop}, {-hwTop + cTop, -hdTop},
                    {-hwTop + cTop, -hdTop + sd}, {-hwTop + sd, -hdTop + cTop},
                    {-hwTop, -hdTop + cTop}, {-hwTop, hdTop - cTop},
                    {-hwTop + sd, hdTop - cTop}, {-hwTop + cTop, hdTop - sd}
                };
                break;
            }
            default:
                topContour = {{-hwTop, hdTop}, {hwTop, hdTop}, {hwTop, -hdTop}, {-hwTop, -hdTop}};
                break;
        }
    } else {
        topContour = {{-hwTop, hdTop}, {hwTop, hdTop}, {hwTop, -hdTop}, {-hwTop, -hdTop}};
    }

    // Seal flat top ceiling slab matching the exact perimeter
    appendFlatCapFanXZ(outVerts, outIndices, topContour, {0.0f, 0.0f}, totalBuildingHeight, deckMat);

    if (params_.hasCornice && params_.cornerType == BuildingCornerType::Sharp90) {
        BuildingComponents::generateCornice(outVerts, outIndices, topW, topD, totalBuildingHeight, 0.45f);
    }

    // 3. Adaptive Conforming Rooftop Generation
    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        topContour,
        totalBuildingHeight,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang,
        params_.showRooftopClutter,
        wallMat
    );
}

void ProceduralBuildingGenerator::generateOvalOrSemiOvalMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float hw = params_.width * 0.5f;
    float hd = params_.depth * 0.5f;
    float totalBuildingHeight = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);

    int32_t numSegments = (params_.shape == BuildingVolumeShape::SemiOval) ? 96 : 128;
    std::vector<Vec3> perimeterPoints;
    std::vector<Vec3> normals;
    std::vector<Vec3> tangents;

    if (params_.shape == BuildingVolumeShape::SemiOval) {
        // Half ellipse curved in front (+Z), flat back wall in rear (-Z)
        int curvedSegs = 64;
        for (int i = 0; i <= curvedSegs; ++i) {
            float angle = (static_cast<float>(i) / curvedSegs) * kPi; // 0 to PI
            float x = hw * std::cos(angle);
            float z = hd * std::sin(angle);
            perimeterPoints.push_back({x, 0.0f, z});
            Vec3 norm = Vec3{x / (hw * hw), 0.0f, z / (hd * hd)}.normalized();
            normals.push_back(norm);
            tangents.push_back(Vec3{0.0f, 1.0f, 0.0f}.cross(norm).normalized());
        }
        // Flat back wall closing line
        perimeterPoints.push_back({-hw, 0.0f, 0.0f});
        normals.push_back({0.0f, 0.0f, -1.0f});
        tangents.push_back({1.0f, 0.0f, 0.0f});
    } else {
        // Full oval (ellipse)
        for (int i = 0; i < numSegments; ++i) {
            float angle = (static_cast<float>(i) / numSegments) * (kPi * 2.0f);
            float x = hw * std::cos(angle);
            float z = hd * std::sin(angle);
            perimeterPoints.push_back({x, 0.0f, z});
            Vec3 norm = Vec3{x / (hw * hw), 0.0f, z / (hd * hd)}.normalized();
            normals.push_back(norm);
            tangents.push_back(Vec3{0.0f, 1.0f, 0.0f}.cross(norm).normalized());
        }
    }

    // Foundation
    for (size_t i = 0; i < perimeterPoints.size(); ++i) {
        size_t next = (i + 1) % perimeterPoints.size();
        Vec3 p0 = perimeterPoints[next] + Vec3{0.0f, -0.4f, 0.0f};
        Vec3 p1 = perimeterPoints[i] + Vec3{0.0f, -0.4f, 0.0f};
        Vec3 p2 = perimeterPoints[i];
        Vec3 p3 = perimeterPoints[next];
        BuildingComponents::appendQuad(outVerts, outIndices, p0, p1, p2, p3, normals[i], foundMat);
    }

    // Floors & Windows along curved perimeter
    size_t count = perimeterPoints.size();
    for (int32_t floor = 0; floor < params_.floorCount; ++floor) {
        float floorBottomY = floor * params_.floorHeight;
        float floorTopY = (floor + 1) * params_.floorHeight;

        for (size_t i = 0; i < count; ++i) {
            size_t next = (i + 1) % count;
            bool isEntranceBay = (floor == 0 && (i == count / 4 || i == count / 4 - 1));
            bool hasWindow = !isEntranceBay && (i % 2 == 0);
            appendPerimeterBayWithAperture(
                outVerts, outIndices,
                perimeterPoints[next], perimeterPoints[i],
                floorBottomY, floorTopY,
                normals[i], tangents[i],
                wallMat, hasWindow,
                params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                params_.windowType, params_.glassType,
                params_.hasWindowShutters, params_.hasSunLouvres,
                params_.wallMaterial, isEntranceBay, params_.doorType, params_.doorWidth, params_.doorHeight, params_.hasEntranceCanopy
            );
        }
    }

    // Ground floor entrance door at apex (+Z center) for solid masonry facades
    if (!BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
        Vec3 doorCenter{0.0f, 0.0f, hd};
        BuildingComponents::generateDoor(outVerts, outIndices,
            doorCenter, params_.doorWidth, params_.doorHeight,
            {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},
            params_.doorType, params_.hasEntranceCanopy);
    }

    // Sealed top roof slab & adaptive contour roof matching exact oval / semi-oval footprint
    std::vector<Vec2> roofContour;
    roofContour.reserve(perimeterPoints.size());
    for (const auto& pt : perimeterPoints) {
        roofContour.push_back(Vec2{pt.x, pt.z});
    }
    Vec2 deckCenterXZ = (params_.shape == BuildingVolumeShape::SemiOval) ? Vec2{0.0f, hd * 0.4f} : Vec2{0.0f, 0.0f};
    appendFlatCapFanXZ(outVerts, outIndices, roofContour, deckCenterXZ, totalBuildingHeight,
        static_cast<uint32_t>(MaterialSlot::RoofSurface));

    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        roofContour,
        totalBuildingHeight,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang,
        params_.showRooftopClutter,
        wallMat
    );
}

void ProceduralBuildingGenerator::generateCylinderMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float radius = params_.width * 0.5f;
    float totalBuildingHeight = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);

    int32_t segments = 128;
    std::vector<Vec3> ring;
    std::vector<Vec3> normals;
    std::vector<Vec3> tangents;

    for (int i = 0; i < segments; ++i) {
        float angle = (static_cast<float>(i) / segments) * (2.0f * kPi);
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        ring.push_back({x, 0.0f, z});
        Vec3 norm{std::cos(angle), 0.0f, std::sin(angle)};
        normals.push_back(norm);
        tangents.push_back(Vec3{0.0f, 1.0f, 0.0f}.cross(norm).normalized());
    }

    // Foundation circular plinth
    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        Vec3 p0 = ring[next] * 1.04f + Vec3{0.0f, -0.5f, 0.0f};
        Vec3 p1 = ring[i] * 1.04f + Vec3{0.0f, -0.5f, 0.0f};
        Vec3 p2 = ring[i] * 1.04f;
        Vec3 p3 = ring[next] * 1.04f;
        BuildingComponents::appendQuad(outVerts, outIndices, p0, p1, p2, p3, normals[i], foundMat);
    }

    // Cylindrical floors with windows and floor dividing bands
    for (int32_t floor = 0; floor < params_.floorCount; ++floor) {
        float floorBottomY = floor * params_.floorHeight;
        float floorTopY = (floor + 1) * params_.floorHeight;

        // Architectural floor dividing slab band (only for solid walls)
        if (floor > 0 && !BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
            float bandH = 0.22f;
            for (int i = 0; i < segments; ++i) {
                int next = (i + 1) % segments;
                Vec3 b0 = ring[next] * 1.02f + Vec3{0.0f, floorBottomY - bandH * 0.5f, 0.0f};
                Vec3 b1 = ring[i] * 1.02f + Vec3{0.0f, floorBottomY - bandH * 0.5f, 0.0f};
                Vec3 b2 = ring[i] * 1.02f + Vec3{0.0f, floorBottomY + bandH * 0.5f, 0.0f};
                Vec3 b3 = ring[next] * 1.02f + Vec3{0.0f, floorBottomY + bandH * 0.5f, 0.0f};
                BuildingComponents::appendQuad(outVerts, outIndices, b0, b1, b2, b3, normals[i], foundMat);
            }
        }

        for (int i = 0; i < segments; ++i) {
            int next = (i + 1) % segments;
            bool isEntranceBay = (floor == 0 && (i == segments / 4 || i == (segments / 4 - 1)));
            bool hasWindow = !isEntranceBay && (i % 2 == 0);
            appendPerimeterBayWithAperture(
                outVerts, outIndices,
                ring[next], ring[i],
                floorBottomY, floorTopY,
                normals[i], tangents[i],
                wallMat, hasWindow,
                params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                params_.windowType, params_.glassType,
                params_.hasWindowShutters, params_.hasSunLouvres,
                params_.wallMaterial, isEntranceBay, params_.doorType, params_.doorWidth, params_.doorHeight, params_.hasEntranceCanopy
            );
        }
    }

    // Ground Floor Entrance Door facing Front (+Z) for solid walls
    if (!BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
        Vec3 doorCenter{0.0f, 0.0f, radius};
        BuildingComponents::generateDoor(outVerts, outIndices,
            doorCenter, params_.doorWidth, params_.doorHeight,
            {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},
            params_.doorType, params_.hasEntranceCanopy);
    }

    // Watertight Circular Roof Deck + Adaptive Conforming Roof
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    std::vector<Vec2> cylContour;
    cylContour.reserve(ring.size());
    for (const auto& pt : ring) {
        cylContour.push_back(Vec2{pt.x, pt.z});
    }
    appendFlatCapFanXZ(outVerts, outIndices, cylContour, {0.0f, 0.0f}, totalBuildingHeight, roofMat);

    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        cylContour,
        totalBuildingHeight,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang,
        params_.showRooftopClutter,
        wallMat
    );
}

void ProceduralBuildingGenerator::generateTriangleSemiCircleMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float hw = params_.width * 0.5f;
    float hd = params_.depth * 0.5f;
    float totalBuildingHeight = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);

    // Base CSA combines a front triangular wedge with a rear semi-circular rotunda:
    // Front apex at (0, 0, hd)
    // Left shoulder at (-hw, 0, 0)
    // Right shoulder at (hw, 0, 0)
    // Rear semi-circle from (hw, 0, 0) through (0, 0, -hd) to (-hw, 0, 0)
    std::vector<Vec3> perimeter;
    std::vector<Vec3> normals;
    std::vector<Vec3> tangents;

    // 1. Triangular front right edge: Apex (0, 0, hd) to Right shoulder (hw, 0, 0)
    Vec3 apex{0.0f, 0.0f, hd};
    Vec3 rightShoulder{hw, 0.0f, 0.0f};
    Vec3 leftShoulder{-hw, 0.0f, 0.0f};

    perimeter.push_back(apex);
    Vec3 normTR = Vec3{hd, 0.0f, hw}.normalized();
    normals.push_back(normTR);
    tangents.push_back((rightShoulder - apex).normalized());

    perimeter.push_back(rightShoulder);
    normals.push_back(normTR);
    tangents.push_back((rightShoulder - apex).normalized());

    // 2. Rear Semi-Circle arc from right shoulder to left shoulder (64 smooth segments toward -Z)
    int arcSegments = 64;
    for (int i = 1; i < arcSegments; ++i) {
        float th = -static_cast<float>(i) / arcSegments * kPi;
        float x = hw * std::cos(th);
        float z = hd * std::sin(th);
        perimeter.push_back({x, 0.0f, z});
        Vec3 norm{x / (hw * hw), 0.0f, z / (hd * hd)};
        norm = norm.normalized();
        normals.push_back(norm);
        tangents.push_back(Vec3{0.0f, 1.0f, 0.0f}.cross(norm).normalized());
    }

    // Left shoulder
    perimeter.push_back(leftShoulder);
    Vec3 normTL = Vec3{-hd, 0.0f, hw}.normalized();
    normals.push_back(normTL);
    tangents.push_back((apex - leftShoulder).normalized());

    size_t count = perimeter.size();

    // Foundation
    for (size_t i = 0; i < count; ++i) {
        size_t next = (i + 1) % count;
        Vec3 p0 = perimeter[next] + Vec3{0.0f, -0.4f, 0.0f};
        Vec3 p1 = perimeter[i] + Vec3{0.0f, -0.4f, 0.0f};
        Vec3 p2 = perimeter[i];
        Vec3 p3 = perimeter[next];
        BuildingComponents::appendQuad(outVerts, outIndices, p0, p1, p2, p3, normals[i], foundMat);
    }

    // Floors and Facades
    for (int32_t floor = 0; floor < params_.floorCount; ++floor) {
        float floorBottomY = floor * params_.floorHeight;
        float floorTopY = (floor + 1) * params_.floorHeight;

        for (size_t i = 0; i < count; ++i) {
            size_t next = (i + 1) % count;
            bool isEntranceBay = (floor == 0 && i == 0);
            bool hasWindow = !isEntranceBay && (i % 2 == 0);
            appendPerimeterBayWithAperture(
                outVerts, outIndices,
                perimeter[next], perimeter[i],
                floorBottomY, floorTopY,
                normals[i], tangents[i],
                wallMat, hasWindow,
                params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                params_.windowType, params_.glassType,
                params_.hasWindowShutters, params_.hasSunLouvres,
                params_.wallMaterial, isEntranceBay, params_.doorType, params_.doorWidth, params_.doorHeight, params_.hasEntranceCanopy
            );
        }
    }

    // Grand Entrance Door at the front prow apex for solid walls
    if (!BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
        BuildingComponents::generateDoor(outVerts, outIndices,
            apex, params_.doorWidth, params_.doorHeight,
            {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},
            params_.doorType, params_.hasEntranceCanopy);
    }

    // Hybrid Roof Deck + Adaptive Conforming Roof
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    std::vector<Vec2> hybridContour;
    hybridContour.reserve(count);
    for (const auto& pt : perimeter) {
        hybridContour.push_back(Vec2{pt.x, pt.z});
    }
    appendFlatCapFanXZ(outVerts, outIndices, hybridContour, {0.0f, 0.0f}, totalBuildingHeight, roofMat);

    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        hybridContour,
        totalBuildingHeight,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang,
        params_.showRooftopClutter,
        wallMat
    );
}

void ProceduralBuildingGenerator::generateTriangularFlatironMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float hw = params_.width * 0.5f;
    float hd = params_.depth * 0.5f;
    float totalBuildingHeight = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);

    // 3 Corners: Apex at (0, 0, hd), Left Rear (-hw, 0, -hd), Right Rear (hw, 0, -hd)
    Vec3 pApex{0.0f, 0.0f, hd};
    Vec3 pRearR{hw, 0.0f, -hd};
    Vec3 pRearL{-hw, 0.0f, -hd};

    struct FacadeLine {
        Vec3 start;
        Vec3 end;
        Vec3 norm;
        Vec3 tan;
        float len;
        bool hasDoor;
    };

    Vec3 nRight = Vec3{2.0f * hd, 0.0f, hw}.normalized();
    Vec3 nRear{0.0f, 0.0f, -1.0f};
    Vec3 nLeft = Vec3{-2.0f * hd, 0.0f, hw}.normalized();

    std::vector<FacadeLine> facades = {
        {pApex, pRearR, nRight, (pRearR - pApex).normalized(), (pRearR - pApex).length(), false},
        {pRearR, pRearL, nRear, (pRearL - pRearR).normalized(), (pRearL - pRearR).length(), false},
        {pRearL, pApex, nLeft, (pApex - pRearL).normalized(), (pApex - pRearL).length(), false}
    };

    // Foundation
    for (const auto& f : facades) {
        BuildingComponents::appendQuad(outVerts, outIndices,
            f.start + Vec3{0.0f, -0.4f, 0.0f},
            f.end + Vec3{0.0f, -0.4f, 0.0f},
            f.end, f.start,
            f.norm, foundMat);
    }

    // Floors & Windows
    for (int32_t floor = 0; floor < params_.floorCount; ++floor) {
        float floorBottomY = floor * params_.floorHeight;
        float floorTopY = (floor + 1) * params_.floorHeight;

        for (const auto& f : facades) {
            int32_t numBays = std::max(2, params_.windowsPerFacade);
            float bayW = f.len / numBays;

            for (int32_t bay = 0; bay < numBays; ++bay) {
                Vec3 b0 = f.start + f.tan * (bay * bayW);
                Vec3 b1 = f.start + f.tan * ((bay + 1) * bayW);
                bool isEntranceBay = (floor == 0 && &f == &facades[0] && bay == 0);

                appendPerimeterBayWithAperture(
                    outVerts, outIndices,
                    b0, b1,
                    floorBottomY, floorTopY,
                    f.norm, f.tan,
                    wallMat, !isEntranceBay,
                    params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                    params_.windowType, params_.glassType,
                    params_.hasWindowShutters, params_.hasSunLouvres,
                    params_.wallMaterial, isEntranceBay, params_.doorType, params_.doorWidth, params_.doorHeight, params_.hasEntranceCanopy
                );
            }
        }
    }

    // Flatiron prow entrance for solid walls
    if (!BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
        BuildingComponents::generateDoor(outVerts, outIndices,
            pApex, params_.doorWidth, params_.doorHeight,
            {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},
            params_.doorType, params_.hasEntranceCanopy);
    }

    // Triangular Roof Deck + Adaptive Conforming Roof
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    Vec3 r0 = pApex + Vec3{0.0f, totalBuildingHeight, 0.0f};
    Vec3 r1 = pRearR + Vec3{0.0f, totalBuildingHeight, 0.0f};
    Vec3 r2 = pRearL + Vec3{0.0f, totalBuildingHeight, 0.0f};
    uint32_t baseIdx = static_cast<uint32_t>(outVerts.size());
    outVerts.push_back({r0, {0.0f, 1.0f, 0.0f}, {0.5f, 1.0f}, roofMat});
    outVerts.push_back({r1, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, roofMat});
    outVerts.push_back({r2, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, roofMat});
    outIndices.push_back(baseIdx);
    outIndices.push_back(baseIdx + 1);
    outIndices.push_back(baseIdx + 2);

    std::vector<Vec2> triContour = {{pApex.x, pApex.z}, {pRearR.x, pRearR.z}, {pRearL.x, pRearL.z}};
    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        triContour,
        totalBuildingHeight,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang,
        params_.showRooftopClutter,
        wallMat
    );
}

void ProceduralBuildingGenerator::generateLShapedMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float hw = params_.width * 0.5f;
    float hd = params_.depth * 0.5f;
    float totalBuildingHeight = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);

    // 6-Corner L-Shape (Counter-Clockwise Winding for Outward Face Normals):
    // P0: (-hw, -hd)  [Outer corner, West/South]
    // P1: (-hw, hd)   [Wing 2 end, North/West]
    // P2: (0, hd)     [Wing 2 inner cut]
    // P3: (0, 0)      [Inner courtyard re-entrant corner]
    // P4: (hw, 0)     [Wing 1 inner cut]
    // P5: (hw, -hd)   [Wing 1 end, East/South]
    std::vector<Vec3> corners = {
        {-hw, 0.0f, -hd},
        {-hw, 0.0f, hd},
        {0.0f, 0.0f, hd},
        {0.0f, 0.0f, 0.0f},
        {hw, 0.0f, 0.0f},
        {hw, 0.0f, -hd}
    };

    size_t count = corners.size();
    for (size_t i = 0; i < count; ++i) {
        size_t next = (i + 1) % count;
        Vec3 edge = corners[next] - corners[i];
        Vec3 tan = edge.normalized();
        Vec3 norm = tan.cross(Vec3{0, 1, 0}).normalized();

        // Foundation
        BuildingComponents::appendQuad(outVerts, outIndices,
            corners[i] + Vec3{0.0f, -0.4f, 0.0f},
            corners[next] + Vec3{0.0f, -0.4f, 0.0f},
            corners[next], corners[i],
            norm, foundMat);

        // Floors & Windows with aperture cutting
        float segLen = edge.length();
        int32_t numBays = std::max(1, static_cast<int32_t>(segLen / 3.8f));
        float bayW = segLen / numBays;

        for (int32_t floor = 0; floor < params_.floorCount; ++floor) {
            float floorBottomY = floor * params_.floorHeight;
            float floorTopY = (floor + 1) * params_.floorHeight;

            for (int32_t bay = 0; bay < numBays; ++bay) {
                Vec3 b0 = corners[i] + tan * (bay * bayW);
                Vec3 b1 = corners[i] + tan * ((bay + 1) * bayW);
                bool isEntranceBay = (floor == 0 && i == 3 && bay == 0);
                bool hasWindow = !isEntranceBay && (segLen > 2.5f);
                appendPerimeterBayWithAperture(
                    outVerts, outIndices,
                    b0, b1,
                    floorBottomY, floorTopY,
                    norm, tan,
                    wallMat, hasWindow,
                    params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                    params_.windowType, params_.glassType,
                    params_.hasWindowShutters, params_.hasSunLouvres,
                    params_.wallMaterial, isEntranceBay, params_.doorType, params_.doorWidth, params_.doorHeight, params_.hasEntranceCanopy
                );
            }
        }
    }

    // Courtyard entrance door for solid walls
    if (!BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
        Vec3 doorPos = corners[3] + Vec3{-1.0f, 0.0f, 1.0f};
        BuildingComponents::generateDoor(outVerts, outIndices,
            doorPos, params_.doorWidth, params_.doorHeight,
            {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},
            params_.doorType, params_.hasEntranceCanopy);
    }

    // L-Shaped Roof Deck + Unified Adaptive Contour Roof
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    // Section 1: (-hw, -hd) to (hw, 0)
    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw, totalBuildingHeight - 0.1f, -hd},
        {hw, totalBuildingHeight, 0.0f},
        roofMat);
    // Section 2: (-hw, 0) to (0, hd)
    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw, totalBuildingHeight - 0.1f, 0.0f},
        {0.0f, totalBuildingHeight, hd},
        roofMat);

    std::vector<Vec2> lContour;
    lContour.reserve(corners.size());
    for (const auto& pt : corners) lContour.push_back(Vec2{pt.x, pt.z});
    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        lContour,
        totalBuildingHeight,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang,
        params_.showRooftopClutter,
        wallMat
    );
}

void ProceduralBuildingGenerator::generatePolygonalPrismMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    int32_t sides
) {
    float radius = params_.width * 0.5f;
    float totalBuildingHeight = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);

    std::vector<Vec3> ring;
    std::vector<Vec3> normals;
    std::vector<Vec3> tangents;

    for (int i = 0; i < sides; ++i) {
        float angle = (static_cast<float>(i) / sides) * (2.0f * kPi);
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        ring.push_back({x, 0.0f, z});
    }

    for (int i = 0; i < sides; ++i) {
        int next = (i + 1) % sides;
        Vec3 edge = ring[next] - ring[i];
        Vec3 norm = Vec3{edge.z, 0.0f, -edge.x}.normalized();
        normals.push_back(norm);
        tangents.push_back((ring[i] - ring[next]).normalized());
    }

    // Foundation
    for (int i = 0; i < sides; ++i) {
        int next = (i + 1) % sides;
        BuildingComponents::appendQuad(outVerts, outIndices,
            ring[next] + Vec3{0.0f, -0.4f, 0.0f},
            ring[i] + Vec3{0.0f, -0.4f, 0.0f},
            ring[i], ring[next],
            normals[i], foundMat);
    }

    // Front Entrance Door on side facing positive Z
    int frontSide = 0;
    float maxZ = -1000.0f;
    for (int i = 0; i < sides; ++i) {
        int next = (i + 1) % sides;
        float avgZ = (ring[i].z + ring[next].z) * 0.5f;
        if (avgZ > maxZ) {
            maxZ = avgZ;
            frontSide = i;
        }
    }

    // Facades & Windows with aperture cutting
    for (int32_t floor = 0; floor < params_.floorCount; ++floor) {
        float floorBottomY = floor * params_.floorHeight;
        float floorTopY = (floor + 1) * params_.floorHeight;

        for (int i = 0; i < sides; ++i) {
            int next = (i + 1) % sides;
            bool isEntranceBay = (floor == 0 && i == frontSide);
            bool hasWindow = !isEntranceBay;
            appendPerimeterBayWithAperture(
                outVerts, outIndices,
                ring[next], ring[i],
                floorBottomY, floorTopY,
                normals[i], tangents[i],
                wallMat, hasWindow,
                params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                params_.windowType, params_.glassType,
                params_.hasWindowShutters, params_.hasSunLouvres,
                params_.wallMaterial, isEntranceBay, params_.doorType, params_.doorWidth, params_.doorHeight, params_.hasEntranceCanopy
            );
        }
    }

    // Entrance door for solid walls
    if (!BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
        int fNext = (frontSide + 1) % sides;
        Vec3 doorCenter = (ring[frontSide] + ring[fNext]) * 0.5f;
        BuildingComponents::generateDoor(outVerts, outIndices,
            doorCenter, params_.doorWidth, params_.doorHeight,
            normals[frontSide], tangents[frontSide],
            params_.doorType, params_.hasEntranceCanopy);
    }

    // Polygonal Roof Deck + Adaptive Conforming Roof
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    std::vector<Vec2> polyContour;
    polyContour.reserve(ring.size());
    for (const auto& pt : ring) polyContour.push_back(Vec2{pt.x, pt.z});
    appendFlatCapFanXZ(outVerts, outIndices, polyContour, {0.0f, 0.0f}, totalBuildingHeight, roofMat);

    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        polyContour,
        totalBuildingHeight,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang,
        params_.showRooftopClutter,
        wallMat
    );
}

void ProceduralBuildingGenerator::generateSteppedZigguratMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);

    // 3 Tier Setbacks:
    // Tier 1 (Base): floors 0 to f1, scale 1.0
    // Tier 2 (Mid): floors f1 to f2, scale 0.75
    // Tier 3 (Tower Top): floors f2 to total, scale 0.50
    int32_t fTotal = params_.floorCount;
    int32_t f1 = std::max(1, fTotal * 4 / 10);
    int32_t f2 = std::max(f1 + 1, fTotal * 7 / 10);

    struct TierDef {
        int32_t startFloor;
        int32_t endFloor;
        float scale;
    };

    std::vector<TierDef> tiers = {
        {0, f1, 1.0f},
        {f1, f2, 0.75f},
        {f2, fTotal, 0.50f}
    };

    // Foundation
    float hw0 = params_.width * 0.5f;
    float hd0 = params_.depth * 0.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw0 - 0.4f, -0.4f, -hd0 - 0.4f},
        {hw0 + 0.4f, 0.0f, hd0 + 0.4f},
        foundMat);

    for (const auto& tier : tiers) {
        float hw = params_.width * 0.5f * tier.scale;
        float hd = params_.depth * 0.5f * tier.scale;
        float tierStartY = tier.startFloor * params_.floorHeight;
        float tierEndY = tier.endFloor * params_.floorHeight;

        // 4 Facades for this tier
        struct Facade {
            Vec3 start;
            Vec3 end;
            Vec3 norm;
            Vec3 tan;
            float len;
            bool isFront;
        };

        std::vector<Facade> tFacades = {
            {{-hw, tierStartY, hd}, {hw, tierStartY, hd}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, hw * 2.0f, true},
            {{hw, tierStartY, hd}, {hw, tierStartY, -hd}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, hd * 2.0f, false},
            {{hw, tierStartY, -hd}, {-hw, tierStartY, -hd}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, hw * 2.0f, false},
            {{-hw, tierStartY, -hd}, {-hw, tierStartY, hd}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, hd * 2.0f, false}
        };

        for (int32_t floor = tier.startFloor; floor < tier.endFloor; ++floor) {
            float floorBottomY = floor * params_.floorHeight;
            float floorTopY = (floor + 1) * params_.floorHeight;

            for (const auto& f : tFacades) {
                int32_t numBays = std::max(2, static_cast<int32_t>(params_.windowsPerFacade * tier.scale));
                float bayW = f.len / numBays;

                for (int32_t bay = 0; bay < numBays; ++bay) {
                    Vec3 b0 = f.start + f.tan * (bay * bayW);
                    Vec3 b1 = f.start + f.tan * ((bay + 1) * bayW);

                    bool isDoor = (floor == 0 && f.isFront && bay == (numBays / 2));
                    appendPerimeterBayWithAperture(
                        outVerts, outIndices,
                        b0, b1,
                        floorBottomY, floorTopY,
                        f.norm, f.tan,
                        wallMat, !isDoor,
                        params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                        params_.windowType, params_.glassType,
                        params_.hasWindowShutters, params_.hasSunLouvres,
                        params_.wallMaterial, isDoor, params_.doorType, params_.doorWidth, params_.doorHeight, params_.hasEntranceCanopy
                    );
                }
            }
        }

        // Setback roof terrace flat surface
        BuildingComponents::appendBox(outVerts, outIndices,
            {-hw, tierEndY - 0.1f, -hd},
            {hw, tierEndY, hd},
            roofMat);

        if (tier.endFloor < fTotal) {
            // Open perimeter balustrade border on setback terrace (never solid box!)
            float rH = 0.72f;
            float rT = 0.14f;
            BuildingComponents::appendBox(outVerts, outIndices, {-hw, tierEndY, hd - rT}, {hw, tierEndY + rH, hd}, wallMat);
            BuildingComponents::appendBox(outVerts, outIndices, {-hw, tierEndY, -hd}, {hw, tierEndY + rH, -hd + rT}, wallMat);
            BuildingComponents::appendBox(outVerts, outIndices, {-hw, tierEndY, -hd + rT}, {-hw + rT, tierEndY + rH, hd - rT}, wallMat);
            BuildingComponents::appendBox(outVerts, outIndices, {hw - rT, tierEndY, -hd + rT}, {hw, tierEndY + rH, hd - rT}, wallMat);
        } else {
            // Crowning roof fitted structurally onto uppermost ziggurat tier
            std::vector<Vec2> topContour = {{-hw, hd}, {hw, hd}, {hw, -hd}, {-hw, -hd}};
            BuildingComponents::generateRoofForContour(
                outVerts, outIndices,
                topContour,
                tierEndY,
                params_.roofType,
                params_.roofHeight,
                params_.roofOverhang,
                params_.showRooftopClutter,
                wallMat
            );
        }
    }

    // Ground Floor Entrance Door on Tier 1 for solid walls
    if (!BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
        Vec3 doorCenter{0.0f, 0.0f, hd0};
        BuildingComponents::generateDoor(outVerts, outIndices,
            doorCenter, params_.doorWidth, params_.doorHeight,
            {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},
            params_.doorType, params_.hasEntranceCanopy);
    }
}

// =========================================================================
// 1. STAR DESIGN: 3-Point Star / Tri-Star Y-Wing Skyscraper
// =========================================================================
void ProceduralBuildingGenerator::generateStarTriPointMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float rOut = params_.width * 0.5f;
    float rIn = rOut * 0.38f;
    float totalH = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);

    // Compute 6 perimeter polygon vertices
    std::vector<Vec2> starPoly(6);
    for (int i = 0; i < 6; ++i) {
        float ang = (i * 60.0f + 90.0f) * (kPi / 180.0f);
        float r = (i % 2 == 0) ? rOut : rIn;
        starPoly[i] = Vec2{std::cos(ang) * r, std::sin(ang) * r};
    }

    // Foundation (Outward Normal, Counter-Clockwise Winding)
    for (size_t i = 0; i < starPoly.size(); ++i) {
        size_t next = (i + 1) % starPoly.size();
        Vec2 p0 = starPoly[next] * 1.05f;
        Vec2 p1 = starPoly[i] * 1.05f;
        Vec3 d{p1.x - p0.x, 0.0f, p1.y - p0.y};
        Vec3 norm = d.normalized().cross(Vec3{0, 1, 0}).normalized();
        BuildingComponents::appendQuad(outVerts, outIndices,
            Vec3{p0.x, -0.4f, p0.y}, Vec3{p1.x, -0.4f, p1.y},
            Vec3{p1.x, 0.0f, p1.y}, Vec3{p0.x, 0.0f, p0.y},
            norm, foundMat);
    }

    // Floors and Facades (Solid Opaque Walls with Outward-Facing Normals)
    for (int f = 0; f < params_.floorCount; ++f) {
        float fBot = f * params_.floorHeight;
        float fTop = fBot + params_.floorHeight;

        for (size_t i = 0; i < starPoly.size(); ++i) {
            size_t next = (i + 1) % starPoly.size();
            Vec3 b0{starPoly[next].x, 0.0f, starPoly[next].y};
            Vec3 b1{starPoly[i].x, 0.0f, starPoly[i].y};
            Vec3 d = b1 - b0;
            float len = d.length();
            if (len < 0.1f) continue;
            Vec3 tang = d * (1.0f / len);
            Vec3 norm = tang.cross(Vec3{0, 1, 0}).normalized();

            int32_t numBays = std::max(1, (params_.windowsPerFacade + 1) / 2);
            float bayW = len / numBays;

            for (int32_t bay = 0; bay < numBays; ++bay) {
                Vec3 bay0 = b0 + tang * (bay * bayW);
                Vec3 bay1 = b0 + tang * ((bay + 1) * bayW);
                bool isDoor = (f == 0 && i == 3 && bay == numBays / 2);
                appendPerimeterBayWithAperture(outVerts, outIndices,
                    bay0, bay1, fBot, fTop, norm, tang,
                    wallMat, !isDoor,
                    params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                    params_.windowType, params_.glassType,
                    params_.hasWindowShutters, params_.hasSunLouvres,
                    params_.wallMaterial, isDoor, params_.doorType, params_.doorWidth, params_.doorHeight, params_.hasEntranceCanopy);
            }

            if (!BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
                // Floor slab belt course only on solid masonry facades
                BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                    b0 + norm * 0.04f + Vec3{0, fBot, 0}, b1 + norm * 0.04f + Vec3{0, fBot, 0}, 0.06f, 0.06f, 8, frameMat, false, false);
            }
        }
    }

    // Roof Cap Polygon Fan + Conforming Star Roof
    appendFlatCapFanXZ(outVerts, outIndices, starPoly, {0.0f, 0.0f}, totalH, roofMat);
    if (params_.roofType == RoofType::NoneFlat) {
        Vec3 roofCenter{0.0f, totalH, 0.0f};
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            roofCenter, roofCenter + Vec3{0.0f, totalH * 0.22f, 0.0f},
            0.18f, 0.02f, 12, frameMat, true, true);
    } else {
        BuildingComponents::generateRoofForContour(
            outVerts, outIndices,
            starPoly,
            totalH,
            params_.roofType,
            params_.roofHeight,
            params_.roofOverhang,
            params_.showRooftopClutter,
            wallMat
        );
    }
}

// =========================================================================
// 2. STAR DESIGN: 4-Point Star / Cruciform Cross Tower
// =========================================================================
void ProceduralBuildingGenerator::generateStarFourPointMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float rOut = params_.width * 0.5f;
    float rIn = rOut * 0.44f;
    float totalH = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);

    std::vector<Vec2> crossPoly(8);
    for (int i = 0; i < 8; ++i) {
        float ang = (i * 45.0f) * (kPi / 180.0f);
        float r = (i % 2 == 0) ? rOut : rIn;
        crossPoly[i] = Vec2{std::cos(ang) * r, std::sin(ang) * r};
    }

    // Foundation (Outward Normal, Counter-Clockwise Winding)
    for (size_t i = 0; i < crossPoly.size(); ++i) {
        size_t next = (i + 1) % crossPoly.size();
        Vec2 p0 = crossPoly[next] * 1.05f;
        Vec2 p1 = crossPoly[i] * 1.05f;
        Vec3 d{p1.x - p0.x, 0.0f, p1.y - p0.y};
        Vec3 norm = d.normalized().cross(Vec3{0, 1, 0}).normalized();
        BuildingComponents::appendQuad(outVerts, outIndices,
            Vec3{p0.x, -0.4f, p0.y}, Vec3{p1.x, -0.4f, p1.y},
            Vec3{p1.x, 0.0f, p1.y}, Vec3{p0.x, 0.0f, p0.y},
            norm, foundMat);
    }

    // Facades per floor (Solid Opaque Walls with Outward-Facing Normals)
    for (int f = 0; f < params_.floorCount; ++f) {
        float fBot = f * params_.floorHeight;
        float fTop = fBot + params_.floorHeight;

        for (size_t i = 0; i < crossPoly.size(); ++i) {
            size_t next = (i + 1) % crossPoly.size();
            Vec3 b0{crossPoly[next].x, 0.0f, crossPoly[next].y};
            Vec3 b1{crossPoly[i].x, 0.0f, crossPoly[i].y};
            Vec3 d = b1 - b0;
            float len = d.length();
            if (len < 0.1f) continue;
            Vec3 tang = d * (1.0f / len);
            Vec3 norm = tang.cross(Vec3{0, 1, 0}).normalized();

            int32_t numBays = std::max(1, (params_.windowsPerFacade + 1) / 2);
            float bayW = len / numBays;

            for (int32_t bay = 0; bay < numBays; ++bay) {
                Vec3 bay0 = b0 + tang * (bay * bayW);
                Vec3 bay1 = b0 + tang * ((bay + 1) * bayW);
                bool isDoor = (f == 0 && i == 0 && bay == numBays / 2);
                appendPerimeterBayWithAperture(outVerts, outIndices,
                    bay0, bay1, fBot, fTop, norm, tang,
                    wallMat, !isDoor,
                    params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                    params_.windowType, params_.glassType,
                    params_.hasWindowShutters, params_.hasSunLouvres,
                    params_.wallMaterial, isDoor, params_.doorType, params_.doorWidth, params_.doorHeight, params_.hasEntranceCanopy);
            }

            if (!BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
                BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                    b0 + norm * 0.04f + Vec3{0, fBot, 0}, b1 + norm * 0.04f + Vec3{0, fBot, 0}, 0.055f, 0.055f, 8, frameMat, false, false);
            }
        }
    }

    // Roof Deck + Conforming Cruciform Star Roof
    appendFlatCapFanXZ(outVerts, outIndices, crossPoly, {0.0f, 0.0f}, totalH, roofMat);
    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        crossPoly,
        totalH,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang,
        params_.showRooftopClutter,
        wallMat
    );
}

// =========================================================================
// 3. STAR DESIGN: 5-Point Star / Pentagram Complex
// =========================================================================
void ProceduralBuildingGenerator::generateStarFivePointMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float rOut = params_.width * 0.5f;
    float rIn = rOut * 0.42f;
    float totalH = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);

    std::vector<Vec2> star5(10);
    for (int i = 0; i < 10; ++i) {
        float ang = (i * 36.0f + 90.0f) * (kPi / 180.0f);
        float r = (i % 2 == 0) ? rOut : rIn;
        star5[i] = Vec2{std::cos(ang) * r, std::sin(ang) * r};
    }

    // Foundation (Outward Normal, Counter-Clockwise Winding)
    for (size_t i = 0; i < star5.size(); ++i) {
        size_t next = (i + 1) % star5.size();
        Vec2 p0 = star5[next] * 1.05f;
        Vec2 p1 = star5[i] * 1.05f;
        Vec3 d{p1.x - p0.x, 0.0f, p1.y - p0.y};
        Vec3 norm = d.normalized().cross(Vec3{0, 1, 0}).normalized();
        BuildingComponents::appendQuad(outVerts, outIndices,
            Vec3{p0.x, -0.4f, p0.y}, Vec3{p1.x, -0.4f, p1.y},
            Vec3{p1.x, 0.0f, p1.y}, Vec3{p0.x, 0.0f, p0.y},
            norm, foundMat);
    }

    // Facades (Solid Opaque Walls with Outward-Facing Normals)
    for (int f = 0; f < params_.floorCount; ++f) {
        float fBot = f * params_.floorHeight;
        float fTop = fBot + params_.floorHeight;

        for (size_t i = 0; i < star5.size(); ++i) {
            size_t next = (i + 1) % star5.size();
            Vec3 b0{star5[next].x, 0.0f, star5[next].y};
            Vec3 b1{star5[i].x, 0.0f, star5[i].y};
            Vec3 d = b1 - b0;
            float len = d.length();
            if (len < 0.1f) continue;
            Vec3 tang = d * (1.0f / len);
            Vec3 norm = tang.cross(Vec3{0, 1, 0}).normalized();

            int32_t numBays = std::max(1, (params_.windowsPerFacade + 1) / 2);
            float bayW = len / numBays;

            for (int32_t bay = 0; bay < numBays; ++bay) {
                Vec3 bay0 = b0 + tang * (bay * bayW);
                Vec3 bay1 = b0 + tang * ((bay + 1) * bayW);
                bool isDoor = (f == 0 && i == 5 && bay == numBays / 2);
                appendPerimeterBayWithAperture(outVerts, outIndices,
                    bay0, bay1, fBot, fTop, norm, tang,
                    wallMat, !isDoor,
                    params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                    params_.windowType, params_.glassType,
                    params_.hasWindowShutters, params_.hasSunLouvres,
                    params_.wallMaterial, isDoor, params_.doorType, params_.doorWidth, params_.doorHeight, params_.hasEntranceCanopy);
            }

            if (!BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
                BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                    b0 + norm * 0.04f + Vec3{0, fBot, 0}, b1 + norm * 0.04f + Vec3{0, fBot, 0}, 0.05f, 0.05f, 8, frameMat, false, false);
            }
        }
    }

    // Roof Deck + Conforming 5-Point Star Roof
    appendFlatCapFanXZ(outVerts, outIndices, star5, {0.0f, 0.0f}, totalH, roofMat);
    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        star5,
        totalH,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang,
        params_.showRooftopClutter,
        wallMat
    );
}

// =========================================================================
// 4. GENERAL DESIGN: Twisted Helix Skyscraper (Turning Torso)
// =========================================================================
void ProceduralBuildingGenerator::generateTwistedHelixMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float hw = params_.width * 0.5f;
    float hd = params_.depth * 0.5f;
    float totalH = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);

    float totalTwistDeg = 65.0f;
    float twistStepRad = (totalTwistDeg * (kPi / 180.0f)) / std::max(1, params_.floorCount);

    auto rot = [](float x, float z, float angle) -> Vec2 {
        float c = std::cos(angle), s = std::sin(angle);
        return Vec2{x * c - z * s, x * s + z * c};
    };

    // Ground foundation plinth
    Vec2 baseP[4] = {rot(-hw * 1.05f, hd * 1.05f, 0.0f), rot(hw * 1.05f, hd * 1.05f, 0.0f), rot(hw * 1.05f, -hd * 1.05f, 0.0f), rot(-hw * 1.05f, -hd * 1.05f, 0.0f)};
    for (int i = 0; i < 4; ++i) {
        int next = (i + 1) % 4;
        Vec3 pb0{baseP[i].x, -0.45f, baseP[i].y};
        Vec3 pb1{baseP[next].x, -0.45f, baseP[next].y};
        Vec3 pt1{baseP[next].x, 0.0f, baseP[next].y};
        Vec3 pt0{baseP[i].x, 0.0f, baseP[i].y};
        Vec3 pNorm = ((pb1 - pb0).normalized()).cross(Vec3{0.0f, 1.0f, 0.0f}).normalized();
        BuildingComponents::appendQuad(outVerts, outIndices, pb0, pb1, pt1, pt0, pNorm, foundMat);
    }
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{baseP[0].x, 0.0f, baseP[0].y}, Vec3{baseP[1].x, 0.0f, baseP[1].y},
        Vec3{baseP[2].x, 0.0f, baseP[2].y}, Vec3{baseP[3].x, 0.0f, baseP[3].y},
        Vec3{0.0f, 1.0f, 0.0f}, foundMat);

    int32_t baysPerFace = std::max(2, params_.windowsPerFacade);

    for (int f = 0; f < params_.floorCount; ++f) {
        float fBot = f * params_.floorHeight;
        float fTop = fBot + params_.floorHeight;
        float a0 = f * twistStepRad;
        float a1 = (f + 1) * twistStepRad;

        Vec2 bCorners[4] = {rot(-hw, hd, a0), rot(hw, hd, a0), rot(hw, -hd, a0), rot(-hw, -hd, a0)};
        Vec2 tCorners[4] = {rot(-hw, hd, a1), rot(hw, hd, a1), rot(hw, -hd, a1), rot(-hw, -hd, a1)};

        // Structural internal floor plate visible through translucent glass curtain wall
        if (f > 0 && BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
            float slabScale = 0.96f;
            BuildingComponents::appendQuad(outVerts, outIndices,
                Vec3{bCorners[0].x * slabScale, fBot + 0.04f, bCorners[0].y * slabScale},
                Vec3{bCorners[1].x * slabScale, fBot + 0.04f, bCorners[1].y * slabScale},
                Vec3{bCorners[2].x * slabScale, fBot + 0.04f, bCorners[2].y * slabScale},
                Vec3{bCorners[3].x * slabScale, fBot + 0.04f, bCorners[3].y * slabScale},
                Vec3{0.0f, 1.0f, 0.0f}, frameMat);
        }

        for (int i = 0; i < 4; ++i) {
            int next = (i + 1) % 4;
            Vec3 p0{bCorners[i].x, fBot, bCorners[i].y};
            Vec3 p1{bCorners[next].x, fBot, bCorners[next].y};
            Vec3 p2{tCorners[next].x, fTop, tCorners[next].y};
            Vec3 p3{tCorners[i].x, fTop, tCorners[i].y};

            Vec3 mid = (p0 + p1 + p2 + p3) * 0.25f;
            Vec3 outwardHint = Vec3{mid.x, 0.0f, mid.z}.normalized();

            if (BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
                BuildingComponents::generateSlantedCurtainWallBay(outVerts, outIndices,
                    p0, p1, p2, p3, outwardHint, baysPerFace, params_.wallMaterial,
                    (f == 0 && i == 0), params_.doorType, params_.doorWidth, params_.doorHeight,
                    params_.hasEntranceCanopy, params_.glassType);
            } else {
                Vec3 tang = (p1 - p0).normalized();
                Vec3 upSlope = (((p3 + p2) * 0.5f) - ((p0 + p1) * 0.5f)).normalized();
                Vec3 trueNorm = tang.cross(upSlope).normalized();
                if (trueNorm.dot(outwardHint) < 0.0f) trueNorm = -trueNorm;

                // Triangulate twisted wall quad cleanly so non-planar surfaces never clip windows
                Vec3 nA = ((p1 - p0).cross(p2 - p0)).normalized();
                if (nA.dot(outwardHint) < 0.0f) nA = -nA;
                Vec3 nB = ((p2 - p0).cross(p3 - p0)).normalized();
                if (nB.dot(outwardHint) < 0.0f) nB = -nB;

                uint32_t bIdx = static_cast<uint32_t>(outVerts.size());
                outVerts.push_back({p0, nA, {0.0f, 0.0f}, wallMat});
                outVerts.push_back({p1, nA, {1.0f, 0.0f}, wallMat});
                outVerts.push_back({p2, nA, {1.0f, 1.0f}, wallMat});
                outIndices.push_back(bIdx); outIndices.push_back(bIdx + 1); outIndices.push_back(bIdx + 2);

                bIdx = static_cast<uint32_t>(outVerts.size());
                outVerts.push_back({p0, nB, {0.0f, 0.0f}, wallMat});
                outVerts.push_back({p2, nB, {1.0f, 1.0f}, wallMat});
                outVerts.push_back({p3, nB, {0.0f, 1.0f}, wallMat});
                outIndices.push_back(bIdx); outIndices.push_back(bIdx + 1); outIndices.push_back(bIdx + 2);

                for (int b = 0; b < baysPerFace; ++b) {
                    float u = (static_cast<float>(b) + 0.5f) / static_cast<float>(baysPerFace);
                    Vec3 botPt = p0 * (1.0f - u) + p1 * u;
                    Vec3 topPt = p3 * (1.0f - u) + p2 * u;
                    Vec3 winCenter = botPt * 0.45f + topPt * 0.55f + trueNorm * 0.14f;
                    float faceLen = (p1 - p0).length();
                    float ww = std::min(params_.windowWidth, (faceLen / baysPerFace) * 0.72f);
                    float wh = std::min(params_.windowHeight, params_.floorHeight * 0.65f);
                    BuildingComponents::generateWindow(outVerts, outIndices,
                        winCenter, ww, wh, trueNorm, tang, params_.windowType, params_.glassType);
                }
            }

            // Helical corner structural column
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices, p0, p3, 0.11f, 0.11f, 10, frameMat, true, true);
        }
    }

    // Top flat deck + rotated adaptive roof matching twisted top floor orientation
    float topAngle = params_.floorCount * twistStepRad;
    Vec2 topC[4] = {rot(-hw, hd, topAngle), rot(hw, hd, topAngle), rot(hw, -hd, topAngle), rot(-hw, -hd, topAngle)};
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{topC[0].x, totalH, topC[0].y}, Vec3{topC[1].x, totalH, topC[1].y},
        Vec3{topC[2].x, totalH, topC[2].y}, Vec3{topC[3].x, totalH, topC[3].y},
        Vec3{0, 1, 0}, roofMat);

    if (params_.roofType != RoofType::NoneFlat) {
        size_t roofStart = outVerts.size();
        std::vector<Vec2> unrotRect = {{-hw, hd}, {hw, hd}, {hw, -hd}, {-hw, -hd}};
        BuildingComponents::generateRoofForContour(
            outVerts, outIndices,
            unrotRect,
            totalH,
            params_.roofType,
            params_.roofHeight,
            params_.roofOverhang,
            params_.showRooftopClutter,
            wallMat
        );
        for (size_t i = roofStart; i < outVerts.size(); ++i) {
            Vec2 rp = rot(outVerts[i].position.x, outVerts[i].position.z, topAngle);
            outVerts[i].position.x = rp.x;
            outVerts[i].position.z = rp.y;
            Vec2 rn = rot(outVerts[i].normal.x, outVerts[i].normal.z, topAngle);
            outVerts[i].normal.x = rn.x;
            outVerts[i].normal.z = rn.y;
        }
    }
}

// =========================================================================
// 5. GENERAL DESIGN: Torus Ring Pavilion (Apple Park / Center Void Ring)
// =========================================================================
void ProceduralBuildingGenerator::generateTorusRingMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float rOut = params_.width * 0.5f;
    float rIn = rOut * 0.52f;
    float totalH = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);

    const int segments = 48;
    float angleStep = 2.0f * kPi / segments;

    for (int f = 0; f < params_.floorCount; ++f) {
        float fBot = f * params_.floorHeight;
        float fTop = fBot + params_.floorHeight;

        for (int i = 0; i < segments; ++i) {
            float a0 = i * angleStep;
            float a1 = (i + 1) * angleStep;
            float c0 = std::cos(a0), s0 = std::sin(a0);
            float c1 = std::cos(a1), s1 = std::sin(a1);

            // Outer Facade
            Vec3 o0{c0 * rOut, 0.0f, s0 * rOut};
            Vec3 o1{c1 * rOut, 0.0f, s1 * rOut};
            Vec3 normOut = Vec3{(c0 + c1) * 0.5f, 0.0f, (s0 + s1) * 0.5f}.normalized();
            Vec3 tangOut = (o1 - o0).normalized();
            bool isOuterDoor = (f == 0 && i == 0);
            appendPerimeterBayWithAperture(outVerts, outIndices,
                o0, o1, fBot, fTop, normOut, tangOut,
                wallMat, !isOuterDoor,
                params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                params_.windowType, params_.glassType,
                params_.hasWindowShutters, params_.hasSunLouvres,
                params_.wallMaterial, isOuterDoor, params_.doorType, params_.doorWidth, params_.doorHeight, params_.hasEntranceCanopy);

            // Inner Atrium Facade
            Vec3 in0{c0 * rIn, 0.0f, s0 * rIn};
            Vec3 in1{c1 * rIn, 0.0f, s1 * rIn};
            Vec3 normIn = -Vec3{(c0 + c1) * 0.5f, 0.0f, (s0 + s1) * 0.5f}.normalized();
            Vec3 tangIn = (in0 - in1).normalized();
            appendPerimeterBayWithAperture(outVerts, outIndices,
                in1, in0, fBot, fTop, normIn, tangIn,
                wallMat, true,
                params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                params_.windowType, params_.glassType,
                false, false,
                params_.wallMaterial, false, params_.doorType, params_.doorWidth, params_.doorHeight, false);

            // Floor & Roof Ring Slabs
            BuildingComponents::appendQuad(outVerts, outIndices,
                in0 + Vec3{0, fTop, 0}, o0 + Vec3{0, fTop, 0},
                o1 + Vec3{0, fTop, 0}, in1 + Vec3{0, fTop, 0},
                Vec3{0, 1, 0}, (f == params_.floorCount - 1) ? roofMat : frameMat);
        }
    }

    // Courtyard Central Foundation Deck
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, -0.2f, 0.0f}, Vec3{0.0f, 0.0f, 0.0f},
        rIn * 0.98f, rIn * 0.98f, 48, foundMat, true, true);

    // Annular Conforming Roof preserving central atrium void
    BuildingComponents::generateRingRoof(
        outVerts, outIndices,
        rOut, rIn, segments, totalH,
        params_.roofType, params_.roofHeight, params_.roofOverhang,
        params_.showRooftopClutter, wallMat
    );
}

// =========================================================================
// 6. GENERAL DESIGN: Cantilever Overhang Complex (Staggered Blocks)
// =========================================================================
void ProceduralBuildingGenerator::generateCantileverOverhangMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float hw = params_.width * 0.5f;
    float hd = params_.depth * 0.5f;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);

    float shiftDist = 4.2f;

    for (int f = 0; f < params_.floorCount; ++f) {
        float fBot = f * params_.floorHeight;
        float fTop = fBot + params_.floorHeight;
        float xShift = (f % 2 == 0) ? -shiftDist * 0.5f : shiftDist * 0.5f;

        Vec3 minPt{xShift - hw, fBot, -hd};
        Vec3 maxPt{xShift + hw, fTop, hd};

        if (BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
            int32_t bays = std::max(1, params_.windowsPerFacade);
            auto emitFaceBays = [&](const Vec3& startXZ, const Vec3& endXZ, const Vec3& normal, const Vec3& tangent, bool allowDoor) {
                for (int32_t b = 0; b < bays; ++b) {
                    float u0 = static_cast<float>(b) / static_cast<float>(bays);
                    float u1 = static_cast<float>(b + 1) / static_cast<float>(bays);
                    Vec3 bStart = startXZ * (1.0f - u0) + endXZ * u0;
                    Vec3 bEnd = startXZ * (1.0f - u1) + endXZ * u1;
                    bool isDoor = allowDoor && (f == 0) && (b == bays / 2);
                    BuildingComponents::generateCurtainWallBay(
                        outVerts, outIndices, bStart, bEnd, normal, tangent, fBot, fTop,
                        params_.wallMaterial, isDoor, params_.doorType, params_.doorWidth, params_.doorHeight,
                        params_.hasEntranceCanopy, params_.glassType);
                }
            };

            emitFaceBays({xShift - hw, 0.0f, hd}, {xShift + hw, 0.0f, hd}, {0, 0, 1}, {1, 0, 0}, true);
            emitFaceBays({xShift + hw, 0.0f, -hd}, {xShift - hw, 0.0f, -hd}, {0, 0, -1}, {-1, 0, 0}, false);
            emitFaceBays({xShift - hw, 0.0f, -hd}, {xShift - hw, 0.0f, hd}, {-1, 0, 0}, {0, 0, 1}, false);
            emitFaceBays({xShift + hw, 0.0f, hd}, {xShift + hw, 0.0f, -hd}, {1, 0, 0}, {0, 0, -1}, false);

            // Slabs
            BuildingComponents::appendQuad(outVerts, outIndices, {xShift-hw, fBot, -hd}, {xShift+hw, fBot, -hd}, {xShift+hw, fBot, hd}, {xShift-hw, fBot, hd}, {0,-1,0}, frameMat);
            BuildingComponents::appendQuad(outVerts, outIndices, {xShift-hw, fTop, hd}, {xShift+hw, fTop, hd}, {xShift+hw, fTop, -hd}, {xShift-hw, fTop, -hd}, {0,1,0}, frameMat);
        } else {
            BuildingComponents::appendBox(outVerts, outIndices, minPt, maxPt, wallMat);

            // Windows on all 4 faces
            Vec3 cFront = (minPt + maxPt) * 0.5f;
            cFront.z = maxPt.z + 0.04f;
            BuildingComponents::generateWindow(outVerts, outIndices, cFront, params_.windowWidth, params_.windowHeight, {0,0,1}, {1,0,0}, params_.windowType, params_.glassType);

            Vec3 cBack = (minPt + maxPt) * 0.5f;
            cBack.z = minPt.z - 0.04f;
            BuildingComponents::generateWindow(outVerts, outIndices, cBack, params_.windowWidth, params_.windowHeight, {0,0,-1}, {-1,0,0}, params_.windowType, params_.glassType);
        }

        // Exposed Cantilever Soffits
        if (f > 0) {
            float prevShift = ((f - 1) % 2 == 0) ? -shiftDist * 0.5f : shiftDist * 0.5f;
            if (xShift > prevShift) {
                // Cantilever to the right (+X)
                BuildingComponents::appendQuad(outVerts, outIndices,
                    Vec3{prevShift + hw, fBot, -hd}, Vec3{xShift + hw, fBot, -hd},
                    Vec3{xShift + hw, fBot, hd}, Vec3{prevShift + hw, fBot, hd},
                    Vec3{0, -1, 0}, frameMat);
            } else {
                // Cantilever to the left (-X)
                BuildingComponents::appendQuad(outVerts, outIndices,
                    Vec3{xShift - hw, fBot, -hd}, Vec3{prevShift - hw, fBot, -hd},
                    Vec3{prevShift - hw, fBot, hd}, Vec3{xShift - hw, fBot, hd},
                    Vec3{0, -1, 0}, frameMat);
            }
        }
    }

    // Crowning roof fitted cleanly onto uppermost cantilevered block
    float totalH = params_.floorCount * params_.floorHeight;
    float topShift = ((params_.floorCount - 1) % 2 == 0) ? -shiftDist * 0.5f : shiftDist * 0.5f;
    std::vector<Vec2> topRect = {
        {topShift - hw, hd}, {topShift + hw, hd},
        {topShift + hw, -hd}, {topShift - hw, -hd}
    };
    appendFlatCapFanXZ(outVerts, outIndices, topRect, {topShift, 0.0f}, totalH, roofMat);
    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        topRect,
        totalH,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang,
        params_.showRooftopClutter,
        wallMat
    );
}

// =========================================================================
// 7. GENERAL DESIGN: Conical Pyramid / Obelisk
// =========================================================================
void ProceduralBuildingGenerator::generateConicalPyramidMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float rBase = params_.width * 0.5f;
    float rApex = rBase * 0.15f;
    float totalH = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);

    const int sides = 8;
    float angleStep = 2.0f * kPi / sides;

    // Foundation
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, -0.4f, 0.0f}, Vec3{0.0f, 0.0f, 0.0f},
        rBase * 1.1f, rBase * 1.05f, sides, foundMat, true, true);

    for (int f = 0; f < params_.floorCount; ++f) {
        float fBot = f * params_.floorHeight;
        float fTop = fBot + params_.floorHeight;
        float t0 = static_cast<float>(f) / params_.floorCount;
        float t1 = static_cast<float>(f + 1) / params_.floorCount;
        float r0 = rBase * (1.0f - t0) + rApex * t0;
        float r1 = rBase * (1.0f - t1) + rApex * t1;

        for (int i = 0; i < sides; ++i) {
            float a0 = i * angleStep;
            float a1 = (i + 1) * angleStep;
            float c0 = std::cos(a0), s0 = std::sin(a0);
            float c1 = std::cos(a1), s1 = std::sin(a1);

            Vec3 p0{c0 * r0, fBot, s0 * r0};
            Vec3 p1{c1 * r0, fBot, s1 * r0};
            Vec3 p2{c1 * r1, fTop, s1 * r1};
            Vec3 p3{c0 * r1, fTop, s0 * r1};

            Vec3 mid = (p0 + p1 + p2 + p3) * 0.25f;
            Vec3 outwardHint = Vec3{mid.x, 0.0f, mid.z}.normalized();
            Vec3 tang = (p1 - p0).normalized();
            Vec3 norm = ((p1 - p0).cross(p3 - p0)).normalized();
            if (norm.dot(outwardHint) < 0.0f) norm = -norm;

            if (BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
                int32_t sideBays = std::max(1, static_cast<int32_t>(std::round((p1 - p0).length() / 3.2f)));
                BuildingComponents::generateSlantedCurtainWallBay(outVerts, outIndices,
                    p0, p1, p2, p3, outwardHint, sideBays, params_.wallMaterial,
                    (f == 0 && i == 2), params_.doorType, params_.doorWidth, params_.doorHeight,
                    params_.hasEntranceCanopy, params_.glassType);
            } else {
                BuildingComponents::appendQuad(outVerts, outIndices, p0, p1, p2, p3, norm, wallMat);

                // Window in center
                Vec3 winCenter = mid + norm * 0.06f;
                BuildingComponents::generateWindow(outVerts, outIndices,
                    winCenter, params_.windowWidth * (r0 / rBase), params_.windowHeight,
                    norm, tang, params_.windowType, params_.glassType);
            }

            // Diagonal corner hip frames
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices, p0, p3, 0.045f, 0.04f, 6, frameMat, true, true);
        }
    }

    // Apex Pyramidion Cap or Structural Roof fitted onto octagonal apex ring
    if (params_.roofType == RoofType::NoneFlat || params_.roofType == RoofType::PyramidalApex) {
        Vec3 apexTip{0.0f, totalH + ((params_.roofType == RoofType::PyramidalApex) ? params_.roofHeight : params_.floorHeight * 0.8f), 0.0f};
        for (int i = 0; i < sides; ++i) {
            float a0 = i * angleStep;
            float a1 = (i + 1) * angleStep;
            Vec3 b0{std::cos(a0) * rApex, totalH, std::sin(a0) * rApex};
            Vec3 b1{std::cos(a1) * rApex, totalH, std::sin(a1) * rApex};
            Vec3 norm = ((b1 - b0).cross(apexTip - b0)).normalized();
            BuildingComponents::appendQuad(outVerts, outIndices, b0, b1, apexTip, b0, norm, frameMat);
        }
    } else {
        std::vector<Vec2> apexRing;
        apexRing.reserve(sides);
        for (int i = 0; i < sides; ++i) {
            float a0 = i * angleStep;
            apexRing.push_back(Vec2{std::cos(a0) * rApex, std::sin(a0) * rApex});
        }
        appendFlatCapFanXZ(outVerts, outIndices, apexRing, {0.0f, 0.0f}, totalH,
            static_cast<uint32_t>(MaterialSlot::RoofSurface));
        BuildingComponents::generateRoofForContour(
            outVerts, outIndices,
            apexRing,
            totalH,
            params_.roofType,
            params_.roofHeight,
            params_.roofOverhang,
            params_.showRooftopClutter,
            wallMat
        );
    }
}

// =========================================================================
// 8. COMPLEX SHAPE: U-Shaped Courtyard Wing Complex
// =========================================================================
void ProceduralBuildingGenerator::generateUShapedCourtyardMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float hw = params_.width * 0.5f;
    float hd = params_.depth * 0.5f;
    float wingW = std::max(3.2f, params_.width * 0.30f);
    float wingD = std::max(3.2f, params_.depth * 0.34f);
    float totalH = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t deckMat = static_cast<uint32_t>(MaterialSlot::CorniceStone);
    uint32_t greenMat = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);

    // 8-corner CCW U-shape perimeter (facing open plaza toward +Z)
    std::vector<Vec3> corners = {
        {-hw, 0.0f, -hd},
        {-hw, 0.0f, hd},
        {-hw + wingW, 0.0f, hd},
        {-hw + wingW, 0.0f, -hd + wingD},
        {hw - wingW, 0.0f, -hd + wingD},
        {hw - wingW, 0.0f, hd},
        {hw, 0.0f, hd},
        {hw, 0.0f, -hd}
    };

    size_t count = corners.size();
    for (size_t i = 0; i < count; ++i) {
        size_t next = (i + 1) % count;
        Vec3 edge = corners[next] - corners[i];
        float segLen = edge.length();
        if (segLen < 0.05f) continue;
        Vec3 tan = edge * (1.0f / segLen);
        Vec3 norm = tan.cross(Vec3{0.0f, 1.0f, 0.0f}).normalized();

        BuildingComponents::appendQuad(outVerts, outIndices,
            corners[i] + Vec3{0.0f, -0.4f, 0.0f},
            corners[next] + Vec3{0.0f, -0.4f, 0.0f},
            corners[next], corners[i],
            norm, foundMat);

        int32_t numBays = std::max(1, static_cast<int32_t>(std::round(segLen / 3.6f)));
        float bayW = segLen / numBays;

        for (int32_t f = 0; f < params_.floorCount; ++f) {
            float fBot = f * params_.floorHeight;
            float fTop = fBot + params_.floorHeight;
            for (int32_t bay = 0; bay < numBays; ++bay) {
                Vec3 b0 = corners[i] + tan * (bay * bayW);
                Vec3 b1 = corners[i] + tan * ((bay + 1) * bayW);
                bool isEntrance = (f == 0 && i == 4 && bay == numBays / 2);
                bool hasWin = !isEntrance && (bayW > 1.8f);
                appendPerimeterBayWithAperture(
                    outVerts, outIndices,
                    b0, b1, fBot, fTop, norm, tan,
                    wallMat, hasWin,
                    params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                    params_.windowType, params_.glassType,
                    params_.hasWindowShutters, params_.hasSunLouvres,
                    params_.wallMaterial, isEntrance, params_.doorType, params_.doorWidth, params_.doorHeight,
                    params_.hasEntranceCanopy
                );
                if (f > 0 && params_.hasBalconies && (i == 3 || i == 4 || i == 5) && bay % 2 == 0 && bayW > 2.2f) {
                    BuildingComponents::generateBalcony(outVerts, outIndices,
                        (b0 + b1) * 0.5f + Vec3{0.0f, fBot, 0.0f},
                        std::min(bayW * 0.72f, 2.8f), params_.balconyDepth, norm, tan, params_.balconyType);
                }
            }
        }
    }

    // Courtyard paved plaza & central planted garden island
    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw + wingW, -0.15f, -hd + wingD}, {hw - wingW, 0.04f, hd}, deckMat);
    if ((hw - wingW) > 1.8f && (hd - wingD * 0.5f) > 1.8f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            {-hw + wingW + 1.0f, 0.04f, -hd + wingD + 1.2f},
            {hw - wingW - 1.0f, 0.18f, hd - 1.2f}, greenMat);
    }

    // Top structural wing slabs
    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw, totalH - 0.14f, -hd}, {hw, totalH, -hd + wingD}, roofMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw, totalH - 0.14f, -hd + wingD}, {-hw + wingW, totalH, hd}, roofMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        {hw - wingW, totalH - 0.14f, -hd + wingD}, {hw, totalH, hd}, roofMat);

    // Multi-wing or unified U-contour adaptive roof crowns
    if (params_.roofType != RoofType::NoneFlat) {
        if (shouldUseUnifiedContourRoof(params_.roofType)) {
            std::vector<Vec2> uContour;
            uContour.reserve(corners.size());
            for (const auto& pt : corners) uContour.push_back(Vec2{pt.x, pt.z});
            BuildingComponents::generateRoofForContour(
                outVerts, outIndices,
                uContour,
                totalH,
                params_.roofType,
                params_.roofHeight,
                params_.roofOverhang,
                params_.showRooftopClutter,
                wallMat
            );
        } else {
            float sideSpanZ = (hd * 2.0f) - wingD;
            float sideCenterZ = (-hd + wingD + hd) * 0.5f;
            generateRoofOffset(outVerts, outIndices,
                BuildingVolumeShape::Rectangle, params_.width, wingD, 0.0f, totalH,
                params_.roofType, params_.roofHeight, params_.roofOverhang, params_.showRooftopClutter,
                0.0f, -hd + wingD * 0.5f);
            if (sideSpanZ > 2.2f) {
                generateRoofOffset(outVerts, outIndices,
                    BuildingVolumeShape::Rectangle, wingW, sideSpanZ, 0.0f, totalH,
                    params_.roofType, params_.roofHeight * 0.85f, params_.roofOverhang * 0.7f, false,
                    -hw + wingW * 0.5f, sideCenterZ);
                generateRoofOffset(outVerts, outIndices,
                    BuildingVolumeShape::Rectangle, wingW, sideSpanZ, 0.0f, totalH,
                    params_.roofType, params_.roofHeight * 0.85f, params_.roofOverhang * 0.7f, false,
                    hw - wingW * 0.5f, sideCenterZ);
            }
        }
    }
}

// =========================================================================
// 9. COMPLEX SHAPE: H-Shaped Twin-Wing & Central Skybridge Complex
// =========================================================================
void ProceduralBuildingGenerator::generateHShapedTwinWingMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float hw = params_.width * 0.5f;
    float hd = params_.depth * 0.5f;
    float wingW = std::max(3.2f, params_.width * 0.28f);
    float bridgeHD = std::max(2.0f, params_.depth * 0.22f);
    float totalH = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t deckMat = static_cast<uint32_t>(MaterialSlot::CorniceStone);

    // 12-corner CCW H-shaped perimeter
    std::vector<Vec3> corners = {
        {-hw, 0.0f, -hd},
        {-hw, 0.0f, hd},
        {-hw + wingW, 0.0f, hd},
        {-hw + wingW, 0.0f, bridgeHD},
        {hw - wingW, 0.0f, bridgeHD},
        {hw - wingW, 0.0f, hd},
        {hw, 0.0f, hd},
        {hw, 0.0f, -hd},
        {hw - wingW, 0.0f, -hd},
        {hw - wingW, 0.0f, -bridgeHD},
        {-hw + wingW, 0.0f, -bridgeHD},
        {-hw + wingW, 0.0f, -hd}
    };

    size_t count = corners.size();
    for (size_t i = 0; i < count; ++i) {
        size_t next = (i + 1) % count;
        Vec3 edge = corners[next] - corners[i];
        float segLen = edge.length();
        if (segLen < 0.05f) continue;
        Vec3 tan = edge * (1.0f / segLen);
        Vec3 norm = tan.cross(Vec3{0.0f, 1.0f, 0.0f}).normalized();

        BuildingComponents::appendQuad(outVerts, outIndices,
            corners[i] + Vec3{0.0f, -0.4f, 0.0f},
            corners[next] + Vec3{0.0f, -0.4f, 0.0f},
            corners[next], corners[i],
            norm, foundMat);

        int32_t numBays = std::max(1, static_cast<int32_t>(std::round(segLen / 3.5f)));
        float bayW = segLen / numBays;
        bool isBridgeFace = (i == 3 || i == 9);
        WallMaterialType effWallMat = isBridgeFace ? WallMaterialType::GlassCurtainWall : params_.wallMaterial;

        for (int32_t f = 0; f < params_.floorCount; ++f) {
            float fBot = f * params_.floorHeight;
            float fTop = fBot + params_.floorHeight;
            for (int32_t bay = 0; bay < numBays; ++bay) {
                Vec3 b0 = corners[i] + tan * (bay * bayW);
                Vec3 b1 = corners[i] + tan * ((bay + 1) * bayW);
                bool isEntrance = (f == 0 && i == 3 && bay == numBays / 2);
                appendPerimeterBayWithAperture(
                    outVerts, outIndices,
                    b0, b1, fBot, fTop, norm, tan,
                    wallMat, !isEntrance && (bayW > 1.6f),
                    params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                    params_.windowType, params_.glassType,
                    params_.hasWindowShutters, params_.hasSunLouvres,
                    effWallMat, isEntrance, params_.doorType, params_.doorWidth, params_.doorHeight,
                    params_.hasEntranceCanopy
                );
            }
        }
    }

    // Front & rear recessed entry court aprons
    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw + wingW, -0.15f, bridgeHD}, {hw - wingW, 0.04f, hd}, deckMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw + wingW, -0.15f, -hd}, {hw - wingW, 0.04f, -bridgeHD}, deckMat);

    // Top wing & bridge roof decks
    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw, totalH - 0.14f, -hd}, {-hw + wingW, totalH, hd}, roofMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        {hw - wingW, totalH - 0.14f, -hd}, {hw, totalH, hd}, roofMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw + wingW, totalH - 0.14f, -bridgeHD}, {hw - wingW, totalH, bridgeHD}, roofMat);

    // Crowning roofs across unified H-contour or wing+bridge system
    if (params_.roofType != RoofType::NoneFlat) {
        if (shouldUseUnifiedContourRoof(params_.roofType)) {
            std::vector<Vec2> hContour;
            hContour.reserve(corners.size());
            for (const auto& pt : corners) hContour.push_back(Vec2{pt.x, pt.z});
            BuildingComponents::generateRoofForContour(
                outVerts, outIndices,
                hContour,
                totalH,
                params_.roofType,
                params_.roofHeight,
                params_.roofOverhang,
                params_.showRooftopClutter,
                wallMat
            );
        } else {
            generateRoofOffset(outVerts, outIndices,
                BuildingVolumeShape::Rectangle, wingW, params_.depth, 0.0f, totalH,
                params_.roofType, params_.roofHeight, params_.roofOverhang * 0.7f, params_.showRooftopClutter,
                -hw + wingW * 0.5f, 0.0f);
            generateRoofOffset(outVerts, outIndices,
                BuildingVolumeShape::Rectangle, wingW, params_.depth, 0.0f, totalH,
                params_.roofType, params_.roofHeight, params_.roofOverhang * 0.7f, params_.showRooftopClutter,
                hw - wingW * 0.5f, 0.0f);
            float bridgeSpanW = params_.width - wingW * 2.0f;
            if (bridgeSpanW > 2.0f) {
                generateRoofOffset(outVerts, outIndices,
                    BuildingVolumeShape::Rectangle, bridgeSpanW, bridgeHD * 2.0f, 0.0f, totalH,
                    params_.roofType, params_.roofHeight * 0.85f, params_.roofOverhang * 0.5f, false,
                    0.0f, 0.0f);
            }
        }
    }
}

// =========================================================================
// 10. COMPLEX SHAPE: Curved Wave Ribbon Tower (Aqua-style Undulating Slabs)
// =========================================================================
void ProceduralBuildingGenerator::generateCurvedWaveRibbonMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float hw = params_.width * 0.44f;
    float hd = params_.depth * 0.44f;
    float totalH = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t slabMat = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t glassMat = BuildingComponents::getGlassMaterial(params_.glassType);

    const int segs = 36;
    auto ribbonPoint = [&](float theta, float yNorm, float extraR) -> Vec3 {
        float c = std::cos(theta);
        float s = std::sin(theta);
        // Sculpted superellipse + smooth harmonic ripple shifting vertically across floors
        float rx = hw + extraR + std::sin(theta * 3.0f + yNorm * 4.8f) * 1.15f + std::cos(theta * 2.0f - yNorm * 3.0f) * 0.55f;
        float rz = hd + extraR + std::cos(theta * 3.0f + yNorm * 4.2f) * 1.15f;
        return Vec3{c * rx, 0.0f, s * rz};
    };

    // Foundation plinth
    for (int i = 0; i < segs; ++i) {
        float a0 = (static_cast<float>(i) / segs) * 2.0f * kPi;
        float a1 = (static_cast<float>(i + 1) / segs) * 2.0f * kPi;
        Vec3 p0 = ribbonPoint(a1, 0.0f, 0.4f);
        Vec3 p1 = ribbonPoint(a0, 0.0f, 0.4f);
        Vec3 norm = (p1 - p0).cross(Vec3{0.0f, 1.0f, 0.0f}).normalized();
        BuildingComponents::appendQuad(outVerts, outIndices,
            p0 + Vec3{0.0f, -0.45f, 0.0f}, p1 + Vec3{0.0f, -0.45f, 0.0f},
            p1, p0, norm, foundMat);
    }

    for (int32_t f = 0; f < params_.floorCount; ++f) {
        float fBot = f * params_.floorHeight;
        float fTop = fBot + params_.floorHeight;
        float yN0 = static_cast<float>(f) / std::max(1, params_.floorCount);
        float yN1 = static_cast<float>(f + 1) / std::max(1, params_.floorCount);

        for (int i = 0; i < segs; ++i) {
            float a0 = (static_cast<float>(i) / segs) * 2.0f * kPi;
            float a1 = (static_cast<float>(i + 1) / segs) * 2.0f * kPi;

            // Inner glazed core bay
            Vec3 c0{std::cos(a1) * hw * 0.86f, fBot, std::sin(a1) * hd * 0.86f};
            Vec3 c1{std::cos(a0) * hw * 0.86f, fBot, std::sin(a0) * hd * 0.86f};
            Vec3 c2{std::cos(a0) * hw * 0.86f, fTop, std::sin(a0) * hd * 0.86f};
            Vec3 c3{std::cos(a1) * hw * 0.86f, fTop, std::sin(a1) * hd * 0.86f};
            Vec3 cTang = (c1 - c0).normalized();
            Vec3 cNorm = cTang.cross(Vec3{0.0f, 1.0f, 0.0f}).normalized();

            BuildingComponents::appendQuad(outVerts, outIndices, c0, c1, c2, c3, cNorm,
                (i % 2 == 0) ? glassMat : wallMat);
            // Vertical aluminum mullion fin
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices, c0, c3, 0.05f, 0.05f, 6, frameMat, false, false);

            // Undulating projecting wave floor slab band at fTop
            Vec3 in0{std::cos(a1) * hw * 0.85f, fTop, std::sin(a1) * hd * 0.85f};
            Vec3 in1{std::cos(a0) * hw * 0.85f, fTop, std::sin(a0) * hd * 0.85f};
            Vec3 out0 = ribbonPoint(a1, yN1, 0.0f) + Vec3{0.0f, fTop, 0.0f};
            Vec3 out1 = ribbonPoint(a0, yN1, 0.0f) + Vec3{0.0f, fTop, 0.0f};
            float slabT = 0.28f;
            Vec3 out0B = out0 - Vec3{0.0f, slabT, 0.0f};
            Vec3 out1B = out1 - Vec3{0.0f, slabT, 0.0f};
            Vec3 in0B = in0 - Vec3{0.0f, slabT, 0.0f};
            Vec3 in1B = in1 - Vec3{0.0f, slabT, 0.0f};

            Vec3 waveNorm = (out1 - out0).cross(Vec3{0.0f, 1.0f, 0.0f}).normalized();
            BuildingComponents::appendQuad(outVerts, outIndices, in0, out0, out1, in1, {0.0f, 1.0f, 0.0f}, slabMat);
            BuildingComponents::appendQuad(outVerts, outIndices, out0B, out1B, out1, out0, waveNorm, slabMat);
            BuildingComponents::appendQuad(outVerts, outIndices, out1B, out0B, in0B, in1B, {0.0f, -1.0f, 0.0f}, frameMat);

            (void)yN0;
        }
    }

    // Ground entrance vestibule & doors facing +Z
    Vec3 entryPos{0.0f, 0.0f, hd * 0.90f};
    BuildingComponents::generateDoor(outVerts, outIndices,
        entryPos, params_.doorWidth, params_.doorHeight,
        {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},
        params_.doorType, params_.hasEntranceCanopy);

    // Top cap & adaptive roof crown fitted exactly to inner core ring
    std::vector<Vec2> coreContour;
    coreContour.reserve(segs);
    for (int i = 0; i < segs; ++i) {
        float a = (static_cast<float>(i) / segs) * 2.0f * kPi;
        coreContour.push_back(Vec2{std::cos(a) * hw * 0.86f, std::sin(a) * hd * 0.86f});
    }
    appendFlatCapFanXZ(outVerts, outIndices, coreContour, {0.0f, 0.0f}, totalH, slabMat);

    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        coreContour,
        totalH,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang,
        params_.showRooftopClutter,
        wallMat
    );
}

// =========================================================================
// 11. COMPLEX SHAPE: Diagrid Exoskeleton Tower (Triangulated Lattice Shell)
// =========================================================================
void ProceduralBuildingGenerator::generateDiagridExoskeletonMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float rBase = params_.width * 0.48f;
    float rDepth = params_.depth * 0.48f;
    float totalH = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t glassMat = BuildingComponents::getGlassMaterial(params_.glassType);

    const int sides = 12;
    auto ringVert = [&](int32_t floorIdx, int segIdx, float radiusMul) -> Vec3 {
        float t = static_cast<float>(floorIdx) / std::max(1, params_.floorCount);
        // Gentle entasis swell around 30% height tapering softly to crown
        float entasis = 1.0f + 0.14f * std::sin(t * kPi) - 0.24f * t * t;
        float angle = (static_cast<float>(segIdx) / sides) * 2.0f * kPi;
        return Vec3{
            std::cos(angle) * rBase * entasis * radiusMul,
            floorIdx * params_.floorHeight,
            std::sin(angle) * rDepth * entasis * radiusMul
        };
    };

    // Plinth
    for (int i = 0; i < sides; ++i) {
        int next = (i + 1) % sides;
        Vec3 p0 = ringVert(0, next, 1.06f);
        Vec3 p1 = ringVert(0, i, 1.06f);
        Vec3 n = (p1 - p0).cross(Vec3{0.0f, 1.0f, 0.0f}).normalized();
        BuildingComponents::appendQuad(outVerts, outIndices,
            p0 + Vec3{0.0f, -0.45f, 0.0f}, p1 + Vec3{0.0f, -0.45f, 0.0f},
            p1, p0, n, foundMat);
    }

    // Glazed inner skin + heavy 3D exoskeleton diagrid diamond struts
    for (int32_t f = 0; f < params_.floorCount; ++f) {
        for (int i = 0; i < sides; ++i) {
            int next = (i + 1) % sides;
            Vec3 b0 = ringVert(f, next, 0.96f);
            Vec3 b1 = ringVert(f, i, 0.96f);
            Vec3 t1 = ringVert(f + 1, i, 0.96f);
            Vec3 t0 = ringVert(f + 1, next, 0.96f);
            Vec3 norm = ((b1 - b0).cross(t0 - b0)).normalized();

            uint32_t bayMat = BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)
                ? glassMat : ((i + f) % 2 == 0 ? glassMat : wallMat);
            BuildingComponents::appendQuad(outVerts, outIndices, b0, b1, t1, t0, norm, bayMat);

            // Outer diagrid structural lattice layer (stepped outward 0.22m from glass)
            Vec3 dB0 = ringVert(f, next, 1.01f);
            Vec3 dB1 = ringVert(f, i, 1.01f);
            Vec3 dT1 = ringVert(f + 1, i, 1.01f);
            Vec3 dT0 = ringVert(f + 1, next, 1.01f);
            // X-bracing diagonal struts forming crisp diamond exoskeleton
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices, dB0, dT1, 0.11f, 0.11f, 6, frameMat, true, true);
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices, dB1, dT0, 0.11f, 0.11f, 6, frameMat, true, true);
            // Horizontal ring belt beam every 2 floors
            if (f % 2 == 0) {
                BuildingComponents::appendOrientedCylinder(outVerts, outIndices, dB0, dB1, 0.13f, 0.13f, 6, frameMat, false, false);
            }
        }
    }

    // Entrance lobby portal facing +Z
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{0.0f, 0.0f, rDepth * 1.02f}, params_.doorWidth, params_.doorHeight,
        {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},
        params_.doorType, params_.hasEntranceCanopy);

    // Top roof slab + crowning roof fitted to exact 12-sided top exoskeleton ring
    std::vector<Vec2> diagridTopContour;
    diagridTopContour.reserve(sides);
    for (int i = 0; i < sides; ++i) {
        Vec3 rv = ringVert(params_.floorCount, i, 1.0f);
        diagridTopContour.push_back(Vec2{rv.x, rv.z});
    }
    appendFlatCapFanXZ(outVerts, outIndices, diagridTopContour, {0.0f, 0.0f}, totalH, frameMat);

    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        diagridTopContour,
        totalH,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang * 0.5f,
        params_.showRooftopClutter,
        wallMat
    );
}

// =========================================================================
// 12. COMPLEX SHAPE: Monumental Mobius Arch Portal (Twin Towers + Skybridge)
// =========================================================================
void ProceduralBuildingGenerator::generateMobiusArchPortalMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float hw = params_.width * 0.5f;
    float hd = params_.depth * 0.45f;
    float pierW = std::max(3.2f, params_.width * 0.27f);
    int32_t bridgeFloors = std::max(2, params_.floorCount / 3);
    int32_t pierFloors = std::max(2, params_.floorCount - bridgeFloors);
    float bridgeBotY = pierFloors * params_.floorHeight;
    float totalH = params_.floorCount * params_.floorHeight;

    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t deckMat = static_cast<uint32_t>(MaterialSlot::CorniceStone);
    uint32_t glassMat = BuildingComponents::getGlassMaterial(params_.glassType);

    // Plaza podium slab spanning full portal width
    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw - 0.6f, -0.4f, -hd - 0.6f}, {hw + 0.6f, 0.0f, hd + 0.6f}, foundMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw + pierW, 0.0f, -hd}, {hw - pierW, 0.08f, hd}, deckMat);

    auto buildRectTower = [&](float xMin, float xMax, float zMin, float zMax,
                              int32_t fStart, int32_t fEnd, bool hasDoor) {
        std::vector<Vec3> rect = {
            {xMin, 0.0f, zMin}, {xMin, 0.0f, zMax},
            {xMax, 0.0f, zMax}, {xMax, 0.0f, zMin}
        };
        for (int s = 0; s < 4; ++s) {
            int next = (s + 1) % 4;
            Vec3 edge = rect[next] - rect[s];
            float len = edge.length();
            Vec3 tan = edge * (1.0f / len);
            Vec3 norm = tan.cross(Vec3{0.0f, 1.0f, 0.0f}).normalized();
            int32_t bays = std::max(1, static_cast<int32_t>(std::round(len / 3.4f)));
            float bayW = len / bays;
            for (int32_t f = fStart; f < fEnd; ++f) {
                float fBot = f * params_.floorHeight;
                float fTop = fBot + params_.floorHeight;
                for (int32_t b = 0; b < bays; ++b) {
                    Vec3 b0 = rect[s] + tan * (b * bayW);
                    Vec3 b1 = rect[s] + tan * ((b + 1) * bayW);
                    bool isDoor = hasDoor && (f == 0 && s == 1 && b == bays / 2);
                    appendPerimeterBayWithAperture(outVerts, outIndices,
                        b0, b1, fBot, fTop, norm, tan, wallMat, !isDoor,
                        params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                        params_.windowType, params_.glassType,
                        params_.hasWindowShutters, params_.hasSunLouvres,
                        params_.wallMaterial, isDoor, params_.doorType, params_.doorWidth, params_.doorHeight,
                        params_.hasEntranceCanopy);
                }
            }
        }
    };

    // West & East Vertical Twin Piers up to bridge base
    buildRectTower(-hw, -hw + pierW, -hd, hd, 0, pierFloors, true);
    buildRectTower(hw - pierW, hw, -hd, hd, 0, pierFloors, true);

    // Monumental Spanned Upper Skybridge Crown across full width
    buildRectTower(-hw, hw, -hd, hd, pierFloors, params_.floorCount, false);

    // Underside soffit of skybridge portal + sculptural diagonal steel arch ribs
    BuildingComponents::appendQuad(outVerts, outIndices,
        {-hw + pierW, bridgeBotY, hd}, {hw - pierW, bridgeBotY, hd},
        {hw - pierW, bridgeBotY, -hd}, {-hw + pierW, bridgeBotY, -hd},
        {0.0f, -1.0f, 0.0f}, frameMat);

    // Arch haunch ribs bridging piers to soffit
    for (float zOff : {-hd * 0.85f, hd * 0.85f}) {
        Vec3 lBase{-hw + pierW, bridgeBotY - params_.floorHeight * 1.15f, zOff};
        Vec3 lTop{-hw + pierW + (hw - pierW) * 0.55f, bridgeBotY, zOff};
        Vec3 rBase{hw - pierW, bridgeBotY - params_.floorHeight * 1.15f, zOff};
        Vec3 rTop{hw - pierW - (hw - pierW) * 0.55f, bridgeBotY, zOff};
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices, lBase, lTop, 0.22f, 0.22f, 8, frameMat, true, true);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices, rBase, rTop, 0.22f, 0.22f, 8, frameMat, true, true);
    }

    // Glazed skylight strip embedded into underside soffit
    BuildingComponents::appendQuad(outVerts, outIndices,
        {-hw + pierW + 0.6f, bridgeBotY - 0.02f, hd * 0.45f},
        {hw - pierW - 0.6f, bridgeBotY - 0.02f, hd * 0.45f},
        {hw - pierW - 0.6f, bridgeBotY - 0.02f, -hd * 0.45f},
        {-hw + pierW + 0.6f, bridgeBotY - 0.02f, -hd * 0.45f},
        {0.0f, -1.0f, 0.0f}, glassMat);

    // Top skybridge slab + adaptive roof
    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw, totalH - 0.12f, -hd}, {hw, totalH, hd}, frameMat);
    BuildingComponents::generateRoof(outVerts, outIndices,
        BuildingVolumeShape::Rectangle, params_.width, hd * 2.0f, 0.0f, totalH,
        params_.roofType, params_.roofHeight, params_.roofOverhang, params_.showRooftopClutter);
}

// =========================================================================
// 13. COMPLEX SHAPE: Stepped Marine Sail Tower (Burj Al Arab Exoskeleton)
// =========================================================================
void ProceduralBuildingGenerator::generateSteppedSailMarineMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float hw = params_.width * 0.5f;
    float hd = params_.depth * 0.5f;
    float totalH = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t glassMat = BuildingComponents::getGlassMaterial(params_.glassType);

    // Sculpted Marine Prow/Sail Cross-Section at height fraction t in [0, 1]
    const int sailSegs = 14;
    auto sailContour = [&](float t, float scaleR) -> std::vector<Vec2> {
        // Sinuous sail bulge at t ~ 0.30, tapering gracefully toward mast apex
        float profile = 0.72f + 0.48f * std::sin(t * kPi * 0.85f) - 0.48f * t * t;
        profile = std::max(0.24f, profile) * scaleR;
        float w = hw * profile;
        float d = hd * (1.0f - 0.42f * t) * scaleR;
        std::vector<Vec2> pts;
        pts.reserve(sailSegs);
        for (int i = 0; i < sailSegs; ++i) {
            float u = (static_cast<float>(i) / sailSegs) * 2.0f * kPi;
            // Pointed teardrop boat prow at +Z (u = pi/2), broad flat curved stern at -Z
            float s = std::sin(u);
            float c = std::cos(u);
            float pinch = (s > 0.0f) ? (1.0f - 0.65f * s * s) : 1.0f;
            pts.push_back(Vec2{c * w * pinch, s * d});
        }
        return pts;
    };

    // Marine deck podium plinth
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, -0.45f, 0.0f}, Vec3{0.0f, 0.0f, 0.0f},
        std::max(hw, hd) * 0.96f, std::max(hw, hd) * 0.92f, 16, foundMat, true, true);

    std::vector<Vec2> prevRing = sailContour(0.0f, 1.0f);
    for (int32_t f = 0; f < params_.floorCount; ++f) {
        float fBot = f * params_.floorHeight;
        float fTop = fBot + params_.floorHeight;
        float t1 = static_cast<float>(f + 1) / std::max(1, params_.floorCount);
        std::vector<Vec2> nextRing = sailContour(t1, 1.0f);

        for (int i = 0; i < sailSegs; ++i) {
            int next = (i + 1) % sailSegs;
            Vec3 p0{prevRing[next].x, fBot, prevRing[next].y};
            Vec3 p1{prevRing[i].x, fBot, prevRing[i].y};
            Vec3 p2{nextRing[i].x, fTop, nextRing[i].y};
            Vec3 p3{nextRing[next].x, fTop, nextRing[next].y};

            Vec3 norm = ((p1 - p0).cross(p3 - p0)).normalized();
            uint32_t mat = (BuildingComponents::isCurtainWallMaterial(params_.wallMaterial) || (i >= 1 && i <= 6))
                ? glassMat : wallMat;
            BuildingComponents::appendQuad(outVerts, outIndices, p0, p1, p2, p3, norm, mat);

            // External sweeping marine exoskeleton arc ribs on port, starboard & rear mast spine
            if (i == 0 || i == sailSegs / 2 || i == sailSegs - 3) {
                Vec3 r0{prevRing[i].x * 1.07f, fBot, prevRing[i].y * 1.07f};
                Vec3 r1{nextRing[i].x * 1.07f, fTop, nextRing[i].y * 1.07f};
                BuildingComponents::appendOrientedCylinder(outVerts, outIndices, r0, r1, 0.16f, 0.15f, 8, frameMat, true, true);
            }
        }
        // Horizontal sail rib rings every 3 floors
        if (f % 3 == 0 && f > 0) {
            for (int i = 0; i < sailSegs; ++i) {
                int next = (i + 1) % sailSegs;
                Vec3 a{prevRing[i].x * 1.04f, fBot, prevRing[i].y * 1.04f};
                Vec3 b{prevRing[next].x * 1.04f, fBot, prevRing[next].y * 1.04f};
                BuildingComponents::appendOrientedCylinder(outVerts, outIndices, a, b, 0.10f, 0.10f, 6, frameMat, false, false);
            }
        }
        prevRing = std::move(nextRing);
    }

    // Soaring rear vertical mast spine + helipad deck cantilever near 75% height
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, 0.0f, -hd * 1.08f}, Vec3{0.0f, totalH + params_.roofHeight + 2.5f, -hd * 0.58f},
        0.34f, 0.12f, 10, frameMat, true, true);

    // Entrance doors at stern plaza
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{0.0f, 0.0f, -hd * 0.72f}, params_.doorWidth, params_.doorHeight,
        {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f},
        params_.doorType, params_.hasEntranceCanopy);

    // Top sail cap & adaptive roof fitted strictly to exact teardrop prow/stern sail contour
    appendFlatCapFanXZ(outVerts, outIndices, prevRing, {0.0f, 0.0f}, totalH, frameMat);

    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        prevRing,
        totalH,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang * 0.35f,
        params_.showRooftopClutter,
        wallMat
    );
}

// =========================================================================
// 14. COMPLEX SHAPE: Interlocking Jenga Cantilever Tower (56 Leonard Style)
// =========================================================================
void ProceduralBuildingGenerator::generateInterlockingJengaMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float hw = params_.width * 0.45f;
    float hd = params_.depth * 0.45f;
    float totalH = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t railMat = static_cast<uint32_t>(MaterialSlot::BalconyGlass);

    BuildingComponents::appendBox(outVerts, outIndices,
        {-hw - 0.6f, -0.4f, -hd - 0.6f}, {hw + 0.6f, 0.0f, hd + 0.6f}, foundMat);

    // Deterministic artistic cantilever offsets per floor cluster
    const float offsetsX[6] = {0.0f, -2.6f, 2.8f, -1.8f, 3.1f, -0.9f};
    const float offsetsZ[6] = {0.0f, 2.0f, -1.6f, -2.4f, 1.8f, 0.4f};
    const float scaleW[6]   = {1.0f, 0.94f, 0.88f, 0.92f, 0.82f, 0.76f};
    const float scaleD[6]   = {1.0f, 0.86f, 0.94f, 0.84f, 0.88f, 0.78f};

    float lastOffX = 0.0f, lastOffZ = 0.0f, lastHW = hw, lastHD = hd;

    for (int32_t f = 0; f < params_.floorCount; ++f) {
        int cluster = (f / 2) % 6;
        if (f == 0) cluster = 0;
        float ox = offsetsX[cluster] * std::min(1.3f, params_.width / 18.0f);
        float oz = offsetsZ[cluster] * std::min(1.3f, params_.depth / 14.0f);
        float cHW = hw * scaleW[cluster];
        float cHD = hd * scaleD[cluster];
        float fBot = f * params_.floorHeight;
        float fTop = fBot + params_.floorHeight;

        std::vector<Vec3> rect = {
            {ox - cHW, 0.0f, oz - cHD},
            {ox - cHW, 0.0f, oz + cHD},
            {ox + cHW, 0.0f, oz + cHD},
            {ox + cHW, 0.0f, oz - cHD}
        };

        for (int s = 0; s < 4; ++s) {
            int next = (s + 1) % 4;
            Vec3 edge = rect[next] - rect[s];
            float len = edge.length();
            Vec3 tan = edge * (1.0f / len);
            Vec3 norm = tan.cross(Vec3{0.0f, 1.0f, 0.0f}).normalized();
            int32_t bays = std::max(2, static_cast<int32_t>(std::round(len / 3.4f)));
            float bayW = len / bays;
            for (int32_t b = 0; b < bays; ++b) {
                Vec3 b0 = rect[s] + tan * (b * bayW);
                Vec3 b1 = rect[s] + tan * ((b + 1) * bayW);
                bool isDoor = (f == 0 && s == 1 && b == bays / 2);
                appendPerimeterBayWithAperture(outVerts, outIndices,
                    b0, b1, fBot, fTop, norm, tan, wallMat, !isDoor,
                    params_.windowSillHeight, params_.windowWidth, params_.windowHeight,
                    params_.windowType, params_.glassType,
                    params_.hasWindowShutters, params_.hasSunLouvres,
                    params_.wallMaterial, isDoor, params_.doorType, params_.doorWidth, params_.doorHeight,
                    params_.hasEntranceCanopy);
            }
        }

        // Exposed structural floor & cantilever soffit caps + glass terrace guardrails
        BuildingComponents::appendQuad(outVerts, outIndices,
            {ox - cHW, fBot, oz - cHD}, {ox + cHW, fBot, oz - cHD},
            {ox + cHW, fBot, oz + cHD}, {ox - cHW, fBot, oz + cHD},
            {0.0f, -1.0f, 0.0f}, frameMat);
        BuildingComponents::appendQuad(outVerts, outIndices,
            {ox - cHW, fTop, oz + cHD}, {ox + cHW, fTop, oz + cHD},
            {ox + cHW, fTop, oz - cHD}, {ox - cHW, fTop, oz - cHD},
            {0.0f, 1.0f, 0.0f}, roofMat);

        if (f % 2 == 1 && f < params_.floorCount - 1) {
            // Perimeter glass parapet on protruding cluster terraces
            float rH = 0.85f;
            BuildingComponents::appendQuad(outVerts, outIndices, {ox - cHW, fTop, oz + cHD}, {ox + cHW, fTop, oz + cHD}, {ox + cHW, fTop + rH, oz + cHD}, {ox - cHW, fTop + rH, oz + cHD}, {0,0,1}, railMat);
            BuildingComponents::appendQuad(outVerts, outIndices, {ox + cHW, fTop, oz - cHD}, {ox - cHW, fTop, oz - cHD}, {ox - cHW, fTop + rH, oz - cHD}, {ox + cHW, fTop + rH, oz - cHD}, {0,0,-1}, railMat);
        }

        lastOffX = ox;
        lastOffZ = oz;
        lastHW = cHW;
        lastHD = cHD;
    }

    // Fit adaptive roof cleanly onto uppermost penthouse block
    generateRoofOffset(outVerts, outIndices,
        BuildingVolumeShape::Rectangle, lastHW * 2.0f, lastHD * 2.0f, 0.0f, totalH,
        params_.roofType, params_.roofHeight, params_.roofOverhang, params_.showRooftopClutter,
        lastOffX, lastOffZ);
}

// =========================================================================
// 15. COMPLEX SHAPE: Faceted Crystalline Gem Tower (Deconstructivist)
// =========================================================================
void ProceduralBuildingGenerator::generateFacetedCrystallineGemMesh(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float rBase = params_.width * 0.5f;
    float dBase = params_.depth * 0.5f;
    float totalH = params_.floorCount * params_.floorHeight;
    uint32_t wallMat = BuildingComponents::getWallMaterial(params_.wallMaterial);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t glassMat = BuildingComponents::getGlassMaterial(params_.glassType);

    const int sides = 7;
    auto crystalRing = [&](int32_t floorIdx, float scaleMul) -> std::vector<Vec3> {
        float t = static_cast<float>(floorIdx) / std::max(1, params_.floorCount);
        // Faceted crystalline shifts: asymmetric expansion/contraction and angular crystal twist
        float shiftX = std::sin(t * kPi * 1.6f) * (rBase * 0.14f);
        float shiftZ = std::cos(t * kPi * 1.3f) * (dBase * 0.12f);
        float radialScale = (1.0f + 0.12f * std::sin(t * kPi * 2.0f) - 0.34f * t) * scaleMul;
        float twist = t * 0.58f;
        std::vector<Vec3> ring;
        ring.reserve(sides);
        for (int i = 0; i < sides; ++i) {
            float a = (static_cast<float>(i) / sides) * 2.0f * kPi + twist;
            // Alternate facet ridge perturbation for sharp crystalline bevels
            float facetMod = (i % 2 == 0) ? 1.08f : 0.86f;
            ring.push_back(Vec3{
                shiftX + std::cos(a) * rBase * radialScale * facetMod,
                floorIdx * params_.floorHeight,
                shiftZ + std::sin(a) * dBase * radialScale * facetMod
            });
        }
        return ring;
    };

    std::vector<Vec3> baseRing = crystalRing(0, 1.06f);
    for (int i = 0; i < sides; ++i) {
        int next = (i + 1) % sides;
        Vec3 n = ((baseRing[i] - baseRing[next]).cross(Vec3{0.0f, 1.0f, 0.0f})).normalized();
        BuildingComponents::appendQuad(outVerts, outIndices,
            baseRing[next] + Vec3{0.0f, -0.45f, 0.0f},
            baseRing[i] + Vec3{0.0f, -0.45f, 0.0f},
            baseRing[i], baseRing[next], n, foundMat);
    }

    std::vector<Vec3> prev = crystalRing(0, 1.0f);
    for (int32_t f = 0; f < params_.floorCount; ++f) {
        std::vector<Vec3> curr = crystalRing(f + 1, 1.0f);
        for (int i = 0; i < sides; ++i) {
            int next = (i + 1) % sides;
            Vec3 p0 = prev[next];
            Vec3 p1 = prev[i];
            Vec3 p2 = curr[i];
            Vec3 p3 = curr[next];

            // Triangulate each slanted crystal facet cleanly so non-planar quads never bow
            Vec3 nA = ((p1 - p0).cross(p2 - p0)).normalized();
            Vec3 nB = ((p2 - p0).cross(p3 - p0)).normalized();
            uint32_t facetMatA = ((i + f) % 3 == 0) ? wallMat : glassMat;
            uint32_t facetMatB = ((i + f) % 2 == 0) ? glassMat : wallMat;

            uint32_t bIdx = static_cast<uint32_t>(outVerts.size());
            outVerts.push_back({p0, nA, {0.0f, 0.0f}, facetMatA});
            outVerts.push_back({p1, nA, {1.0f, 0.0f}, facetMatA});
            outVerts.push_back({p2, nA, {1.0f, 1.0f}, facetMatA});
            outIndices.push_back(bIdx); outIndices.push_back(bIdx + 1); outIndices.push_back(bIdx + 2);

            bIdx = static_cast<uint32_t>(outVerts.size());
            outVerts.push_back({p0, nB, {0.0f, 0.0f}, facetMatB});
            outVerts.push_back({p2, nB, {1.0f, 1.0f}, facetMatB});
            outVerts.push_back({p3, nB, {0.0f, 1.0f}, facetMatB});
            outIndices.push_back(bIdx); outIndices.push_back(bIdx + 1); outIndices.push_back(bIdx + 2);

            // Bold metallic crystal ridge mullions along edge & diagonal crease
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices, p0, p3, 0.08f, 0.08f, 6, frameMat, true, true);
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices, p0, p2, 0.055f, 0.055f, 6, frameMat, false, false);
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices, p0, p1, 0.07f, 0.07f, 6, frameMat, false, false);
        }
        prev = std::move(curr);
    }

    // Ground entrance vestibule
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{0.0f, 0.0f, dBase * 0.92f}, params_.doorWidth, params_.doorHeight,
        {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},
        params_.doorType, params_.hasEntranceCanopy);

    // Top crystal crown deck + adaptive roof conforming to exact asymmetric 7-facet crystal contour
    Vec3 topCenter{0.0f, 0.0f, 0.0f};
    std::vector<Vec2> gemTopContour;
    gemTopContour.reserve(sides);
    for (const auto& v : prev) {
        topCenter = topCenter + v;
        gemTopContour.push_back(Vec2{v.x, v.z});
    }
    topCenter = topCenter * (1.0f / sides);
    appendFlatCapFanXZ(outVerts, outIndices, gemTopContour, {topCenter.x, topCenter.z}, totalH, frameMat);

    BuildingComponents::generateRoofForContour(
        outVerts, outIndices,
        gemTopContour,
        totalH,
        params_.roofType,
        params_.roofHeight,
        params_.roofOverhang * 0.35f,
        params_.showRooftopClutter,
        wallMat
    );
}

} // namespace procedural::buildings


