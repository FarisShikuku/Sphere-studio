#pragma once
#include "CabComponentsVariants.h"
#include "scene/Mesh.h"
#include "scene/Transform.h"
#include <utility>
#include <vector>

namespace sphere::vehicles::cab {

struct HullStationX {
    float x = 0.0f;
    float yBot = 0.0f;
    float yTop = 2.0f;
    float halfWidth = 1.25f;
    float botCornerR = 0.10f;
    float topCornerR = 0.20f;
    float roofCrown = 0.06f;      // Central arch rise along Z=0
    float frontCurvature = 0.0f;  // Forward bulge at Z=0 (for curved noses)
};

/**
 * Shared Procedural Cab Components & Interior Engine.
 * Builds watertight outward-CCW sculpted aerodynamic vehicle hulls with real
 * open wheel wells, real window/door cutouts, animated hinged/sliding doors,
 * full 3D interiors (seats, cockpit, dashboard, steering wheel, stairwells),
 * and translucent automotive glass panes.
 */
class CabComponents {
public:
    static void AppendQuad(
        Mesh* mesh,
        const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d,
        const Vec4& color
    );

    // Tapered frustum / sloped wedge along X with explicit front/rear rectangles (outward-CCW)
    static void AppendFrustumX(
        Mesh* mesh,
        float xFront, float yFrontBot, float yFrontTop, float hwFront,
        float xBack, float yBackBot, float yBackTop, float hwBack,
        const Vec4& color
    );

    // Lofted multi-station rounded aerodynamic body hull along X
    static void AppendSculptedHullX(
        Mesh* mesh,
        const std::vector<HullStationX>& stations,
        const Vec4& color,
        int arcSegments = 4
    );

    // Smooth curved outer wheel-arch eyebrow flare around a wheel center
    static void AppendWheelArchFender(
        Mesh* mesh,
        float axleX,
        float wheelCenterY,
        float archRadius,
        float sideZ,
        bool isRightSide,
        float fenderDepth,
        const Vec4& color
    );

    // Dark recessed curved semicircular wheel-well pocket behind/above each tire
    static void AppendWheelWellLiner(
        Mesh* mesh,
        float axleX,
        float archHalfLen,
        float floorY,
        float archTopY,
        float outerAbsZ,
        float innerAbsZ,
        const Vec4& linerColor
    );

    // Smooth well-cut semicircular wheel-arch skirt spandrel & inner return lip (replaces boxy cutouts)
    static void AppendSemicircularWheelArchSkirt(
        Mesh* mesh,
        float axleX,
        float wheelCenterY,
        float cutRadius,
        float archHalfLen,
        float floorY,
        float archTopY,
        float sideZ,
        bool isRightSide,
        float thickness,
        const Vec4& skirtColor,
        const Vec4& innerColor
    );

    // Segmented side skirt wall skipping open wheel-well cutouts and doorways
    static void AppendSkirtWithCutouts(
        Mesh* mesh,
        float xStart, float xEnd,
        float yBot, float yTop,
        float sideZ, bool isRightSide,
        float thickness,
        const std::vector<std::pair<float, float>>& skipXRanges,
        const Vec4& color
    );

    // Sleek cantilevered overhead gooseneck coach mirrors (Volvo UNVI / Euro Touring style)
    static void AppendCantileverCoachMirrors(
        Mesh* mesh,
        float pivotX,
        float pivotY,
        float halfWidth,
        const Vec4& housingColor
    );

    // Swept-back angled LED projector headlights + DRL + lower fog valance
    static void AppendModernLedFascia(
        Mesh* mesh,
        float frontX,
        float botY, float topY,
        float halfWidth,
        bool headlightsOn,
        const Vec4& bumperColor,
        const Vec4& maskColor
    );

    // Opaque V-nose gloss-black fascia + emblem bar + LED header sign + wipers
    static void AppendCurvedBlackNoseMask(
        Mesh* mesh,
        float xFrontNose, float xRoofBrow,
        float yMaskBot, float yWinBot, float yWinTop,
        float halfWidth,
        float bulgeX,
        const Vec4& maskColor,
        const Vec4& ledDisplayColor,
        bool addLedSign = true,
        bool addWipers = true
    );

    // Translucent curved wrap-around panoramic windshield glass (drawn in glass pass)
    static void AppendCurvedWindshieldGlass(
        Mesh* mesh,
        float xFrontNose, float xRoofBrow,
        float yWinBot, float yWinTop,
        float halfWidth,
        float bulgeX,
        const Vec4& glassColor
    );

    // Legacy combined helper (kept for compatibility)
    static void AppendCurvedBlackGlassMask(
        Mesh* mesh,
        float xFrontNose, float xRoofBrow,
        float yMaskBot, float yWinBot, float yWinTop,
        float halfWidth,
        float bulgeX,
        const Vec4& glassColor,
        const Vec4& maskColor,
        const Vec4& ledDisplayColor,
        bool addWipers = true
    );

    // Volvo UNVI / modern coach diagonal "swan-neck" swooping side glass ribbon
    static void AppendSwoopingCoachGlassSide(
        Mesh* mesh,
        float xApillar, float xDropEnd, float xRear,
        float yFrontLowBot, float yMainBeltBot, float yWinTop,
        float halfWidth,
        int windowPanes,
        const Vec4& glassColor,
        const Vec4& trimColor,
        bool emitGlassOnly = false,
        bool emitMullionsOnly = false,
        float rightDoorSkipX0 = 1e6f,
        float rightDoorSkipX1 = -1e6f,
        float xSwoopStart = -1e6f,
        float yRearBeltBot = -1.0f
    );

    // Sloped outer lower waist panel under the swooping swan-neck coach glass
    static void AppendSwoopWaistWallSide(
        Mesh* mesh,
        float xApillar, float xDropEnd,
        float ySkirtTop, float yFrontLowBot, float yMainBeltBot,
        float halfWidth,
        float thickness,
        const Vec4& paintColor,
        const Vec4& trimColor,
        bool skipRightSide = false
    );

    // Real openable hinged OR sliding door with hollow window cutout (frame pass vs glass pass)
    static void AppendAnimatedDoor(
        Mesh* mesh,
        float doorFrontX, float doorBackX,
        float botY, float topY,
        float sideZ, bool isRightSide,
        float openRatio,
        bool isSliding,
        bool emitGlassPass,
        const Vec4& paintColor,
        const Vec4& trimColor,
        const Vec4& glassColor
    );

    // Ergonomic passenger / captain seat
    static void AppendSeat(
        Mesh* mesh,
        const Vec3& floorPos,
        float facingSignX,
        float widthZ,
        float seatH,
        const Vec4& cushionColor,
        const Vec4& headrestColor,
        const Vec4& frameColor
    );

    // Driver cockpit: wrap-around dashboard, lit gauges, steering wheel, driver seat & partition
    static void AppendDriverCockpit(
        Mesh* mesh,
        float dashFrontX,
        float driverZ,
        float floorY,
        const Vec4& dashColor,
        const Vec4& seatColor,
        const Vec4& trimColor
    );

    // Full bus/coach cabin interior: floor, entry stairwell steps, stanchions, seats, ceiling & bulkhead
    static void AppendBusInterior(
        Mesh* mesh,
        float xFrontCabin, float xRearCabin,
        float floorY, float ceilingY,
        float hw,
        float doorX0, float doorX1,
        float doorStepBotY,
        int seatRows,
        bool dualSideDoors,
        float secondDoorX0, float secondDoorX1,
        const Vec4& seatColor,
        const Vec4& floorColor,
        const Vec4& trimColor
    );

    // Full truck cab interior: floor, dashboard, steering wheel, twin air-ride seats & optional sleeper bunk
    static void AppendTruckCabInterior(
        Mesh* mesh,
        float xDashFront, float xCabRear,
        float floorY, float ceilingY,
        float cabHW,
        bool hasSleeperBunk,
        const Vec4& dashColor,
        const Vec4& seatColor,
        const Vec4& accentColor
    );

    static void AppendDoor(
        Mesh* mesh,
        float doorFrontX, float doorBackX,
        float botY, float topY,
        float sideZ, bool isRightSide,
        const Vec4& paintColor,
        const Vec4& glassColor
    );

    static void AppendWindshield(
        Mesh* mesh,
        float xBot, float xTop,
        float yBot, float yTop,
        float halfWidth,
        const Vec4& glassColor,
        const Vec4& rubberSealColor
    );

    static void AppendHeavyGrille(
        Mesh* mesh,
        float faceX,
        float botY, float topY,
        float halfWidth,
        int slats,
        const Vec4& surroundColor,
        const Vec4& innerBackingColor
    );

    static void AppendWingMirrors(
        Mesh* mesh,
        float anchorX,
        float mirrorCenterY,
        float cabHalfWidth,
        const Vec4& housingColor
    );

    static void AppendFrontBumperAndLights(
        Mesh* mesh,
        float frontX,
        float botY, float topY,
        float halfWidth,
        bool headlightsOn,
        const Vec4& bumperColor
    );

    static void AppendAmberRoofMarkers(
        Mesh* mesh,
        float frontX,
        float roofY,
        float halfWidth,
        bool lightsOn
    );
};

} // namespace sphere::vehicles::cab
