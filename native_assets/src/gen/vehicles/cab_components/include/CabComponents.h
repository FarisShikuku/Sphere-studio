#pragma once
#include "CabComponentsVariants.h"
#include "scene/Mesh.h"
#include "scene/Transform.h"

namespace sphere::vehicles::cab {

/**
 * Shared Procedural Cab Components Engine.
 * Builds outward-CCW opaque doors, windshields, chrome heavy grilles,
 * wing mirrors, bumper headlight assemblies, and aerodynamic cab roofs.
 */
class CabComponents {
public:
    static void AppendQuad(
        Mesh* mesh,
        const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d,
        const Vec4& color
    );

    // Tapered frustum / sloped wedge along X with explicit front/rear rectangles
    static void AppendFrustumX(
        Mesh* mesh,
        float xFront, float yFrontBot, float yFrontTop, float hwFront,
        float xBack, float yBackBot, float yBackTop, float hwBack,
        const Vec4& color
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
