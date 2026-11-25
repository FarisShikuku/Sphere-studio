#pragma once

#include "BuildingTypes.h"
#include <vector>

namespace procedural::buildings {

class BuildingComponents {
public:
    static void appendQuad(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3,
        const Vec3& normal,
        uint32_t materialId
    );

    static void appendBox(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& min, const Vec3& max,
        uint32_t materialId
    );

    static void appendOrientedBox(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& center,
        const Vec3& tangent,
        const Vec3& up,
        const Vec3& normal,
        float halfT,
        float halfH,
        float halfN,
        uint32_t materialId
    );

    static void appendOrientedCylinder(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& p0,
        const Vec3& p1,
        float radius0,
        float radius1,
        int segments,
        uint32_t materialId,
        bool cap0 = true,
        bool cap1 = true
    );

    static void appendBentCylinderCatenary(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& p0,
        const Vec3& p1,
        float sag,
        float radius,
        int lengthSegments,
        int radialSegments,
        uint32_t materialId
    );

    static uint32_t getGlassMaterial(GlassType glassType);
    static uint32_t getWallMaterial(WallMaterialType wallType);
    static bool isCurtainWallMaterial(WallMaterialType wallType);

    static void generateCurtainWallBay(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& bayStart,
        const Vec3& bayEnd,
        const Vec3& normal,
        const Vec3& tangent,
        float floorBottomY,
        float floorTopY,
        WallMaterialType wallType,
        bool isEntranceBay,
        DoorType doorType,
        float doorWidth,
        float doorHeight,
        bool hasCanopy,
        GlassType glassType = GlassType::ClearFloat
    );

    static void generateSlantedCurtainWallBay(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& p0,
        const Vec3& p1,
        const Vec3& p2,
        const Vec3& p3,
        const Vec3& outwardHint,
        int32_t bayCount,
        WallMaterialType wallType,
        bool isEntranceBay,
        DoorType doorType,
        float doorWidth,
        float doorHeight,
        bool hasCanopy,
        GlassType glassType = GlassType::ClearFloat
    );

    static void generateWindow(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& center,
        float width,
        float height,
        const Vec3& normal,
        const Vec3& tangent,
        WindowType type,
        GlassType glassType = GlassType::ClearFloat
    );

    static void generateDoor(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& centerGround,
        float width,
        float height,
        const Vec3& normal,
        const Vec3& tangent,
        DoorType type,
        bool withCanopy = true
    );

    static void generateRoof(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        BuildingVolumeShape shape,
        float baseWidth,
        float baseDepth,
        float baseRadius,
        float baseTopY,
        RoofType type,
        float roofHeight,
        float overhang,
        bool showClutter
    );

    static void generateRoofForContour(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const std::vector<Vec2>& contourXZ,
        float baseTopY,
        RoofType type,
        float roofHeight,
        float overhang,
        bool showClutter,
        uint32_t wallMaterialId = static_cast<uint32_t>(MaterialSlot::WallConcrete)
    );

    static void generateRingRoof(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        float outerRadius,
        float innerRadius,
        int32_t segments,
        float baseTopY,
        RoofType type,
        float roofHeight,
        float overhang,
        bool showClutter,
        uint32_t wallMaterialId = static_cast<uint32_t>(MaterialSlot::WallConcrete)
    );

    static void generateBalcony(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& baseCenter,
        float width,
        float depth,
        const Vec3& normal,
        const Vec3& tangent,
        BalconyType type = BalconyType::CantileveredGlass
    );

    static void generateWindowShutters(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& center,
        float width,
        float height,
        const Vec3& normal,
        const Vec3& tangent
    );

    static void generateGroundPillars(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& facadeStart,
        const Vec3& facadeEnd,
        float height,
        int32_t count,
        const Vec3& normal,
        uint32_t materialId
    );

    static void generateSunLouvres(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& windowTopCenter,
        float width,
        float projection,
        const Vec3& normal,
        const Vec3& tangent
    );

    static void generateFireEscape(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& startBottom,
        float totalHeight,
        int32_t floorCount,
        float floorHeight,
        const Vec3& normal,
        const Vec3& tangent
    );

    static void generateCornice(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        float width,
        float depth,
        float topY,
        float projection = 0.5f
    );

    static void generateCeilingPendantBulb(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& mountPos,
        float dropLength,
        float bulbRadius,
        bool isDaylight = false
    );

    static void generateWallSconceLamp(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& wallPos,
        const Vec3& outwardNormal,
        bool isDaylight = false
    );

    static void generateFloorStandingLamp(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const Vec3& floorPos,
        float height = 1.75f,
        bool isDaylight = false
    );
};

} // namespace procedural::buildings
