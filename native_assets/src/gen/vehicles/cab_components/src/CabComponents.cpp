#include "CabComponents.h"
#include "RunningGear.h"
#include <algorithm>

namespace sphere::vehicles::cab {

using sphere::trailers::gear::RunningGear;

void CabComponents::AppendQuad(
    Mesh* mesh,
    const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d,
    const Vec4& color
) {
    uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
    Vec3 pts[4] = {a, b, c, d};
    float uvs[4][2] = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};
    for (int i = 0; i < 4; ++i) {
        Vertex v;
        v.position = pts[i];
        v.color = Vec4(color.x, color.y, color.z, 1.0f);
        v.uv[0] = uvs[i][0];
        v.uv[1] = uvs[i][1];
        mesh->vertices.push_back(v);
    }
    mesh->indices.push_back(base + 0);
    mesh->indices.push_back(base + 1);
    mesh->indices.push_back(base + 2);
    mesh->indices.push_back(base + 0);
    mesh->indices.push_back(base + 2);
    mesh->indices.push_back(base + 3);
}

void CabComponents::AppendFrustumX(
    Mesh* mesh,
    float xFront, float yFrontBot, float yFrontTop, float hwFront,
    float xBack, float yBackBot, float yBackTop, float hwBack,
    const Vec4& color
) {
    // xFront < xBack (-X is front, +X is rear)
    Vec3 fBL(xFront, yFrontBot, -hwFront);
    Vec3 fBR(xFront, yFrontBot,  hwFront);
    Vec3 fTR(xFront, yFrontTop,  hwFront);
    Vec3 fTL(xFront, yFrontTop, -hwFront);

    Vec3 bBL(xBack, yBackBot, -hwBack);
    Vec3 bBR(xBack, yBackBot,  hwBack);
    Vec3 bTR(xBack, yBackTop,  hwBack);
    Vec3 bTL(xBack, yBackTop, -hwBack);

    // Front (-X): outward normal -X
    AppendQuad(mesh, fBR, fBL, fTL, fTR, color);
    // Back (+X): outward normal +X
    AppendQuad(mesh, bBL, bBR, bTR, bTL, color);
    // Top (+Y)
    AppendQuad(mesh, fTL, bTL, bTR, fTR, color);
    // Bottom (-Y)
    AppendQuad(mesh, bBL, fBL, fBR, bBR, color);
    // Left (-Z)
    AppendQuad(mesh, fBL, bBL, bTL, fTL, color);
    // Right (+Z)
    AppendQuad(mesh, bBR, fBR, fTR, bTR, color);
}

void CabComponents::AppendDoor(
    Mesh* mesh,
    float doorFrontX, float doorBackX,
    float botY, float topY,
    float sideZ, bool isRightSide,
    const Vec4& paintColor,
    const Vec4& glassColor
) {
    float sign = isRightSide ? 1.0f : -1.0f;
    float innerZ = sideZ;
    float outerZ = sideZ + sign * 0.025f;
    float zMin = std::min(innerZ, outerZ);
    float zMax = std::max(innerZ, outerZ);

    // Subtle door seam frame + lower belt molding
    Vec4 seamCol(0.08f, 0.09f, 0.11f, 1.0f);
    Vec4 handleCol(0.75f, 0.78f, 0.82f, 1.0f);
    RunningGear::AppendBox(mesh, Vec3(doorFrontX, botY, zMin), Vec3(doorBackX, topY, zMax), paintColor);

    // Door seam borders
    RunningGear::AppendBox(mesh, Vec3(doorFrontX, botY, zMin - 0.002f), Vec3(doorFrontX + 0.02f, topY, zMax + 0.002f), seamCol);
    RunningGear::AppendBox(mesh, Vec3(doorBackX - 0.02f, botY, zMin - 0.002f), Vec3(doorBackX, topY, zMax + 0.002f), seamCol);

    // Upper window pane (opaque dark slate tinted glass, alpha=1.0)
    float winBot = botY + (topY - botY) * 0.48f;
    float winTop = topY - 0.09f;
    float gMin = isRightSide ? zMax - 0.005f : zMin - 0.012f;
    float gMax = isRightSide ? zMax + 0.012f : zMin + 0.005f;
    RunningGear::AppendBox(mesh, Vec3(doorFrontX + 0.07f, winBot, gMin), Vec3(doorBackX - 0.07f, winTop, gMax), glassColor);

    // Vertical pull handle
    float hX0 = doorBackX - 0.19f;
    float hX1 = doorBackX - 0.09f;
    float hY0 = winBot - 0.24f;
    float hY1 = winBot - 0.06f;
    float hZMin = isRightSide ? zMax : zMin - 0.03f;
    float hZMax = isRightSide ? zMax + 0.03f : zMin;
    RunningGear::AppendBox(mesh, Vec3(hX0, hY0, hZMin), Vec3(hX1, hY1, hZMax), handleCol);
}

void CabComponents::AppendWindshield(
    Mesh* mesh,
    float xBot, float xTop,
    float yBot, float yTop,
    float halfWidth,
    const Vec4& glassColor,
    const Vec4& rubberSealColor
) {
    // Outer rubber trim strip surrounding sloped windshield pane
    AppendFrustumX(
        mesh,
        xBot - 0.015f, yBot - 0.035f, yTop + 0.035f, halfWidth + 0.035f,
        xTop + 0.045f, yBot - 0.035f, yTop + 0.035f, halfWidth + 0.035f,
        rubberSealColor
    );

    // Opaque reflective glass surface protruding 1.8 cm forward of frame
    float hwGlass = std::max(0.2f, halfWidth - 0.03f);
    Vec4 solidGlass(glassColor.x, glassColor.y, glassColor.z, 1.0f);
    AppendFrustumX(
        mesh,
        xBot - 0.032f, yBot, yTop, hwGlass,
        xTop + 0.025f, yBot, yTop, hwGlass,
        solidGlass
    );

    // Twin windshield wiper blades along bottom cowling
    RunningGear::AppendBox(mesh, Vec3(xBot - 0.055f, yBot + 0.03f, -hwGlass * 0.72f), Vec3(xBot - 0.025f, yBot + 0.06f, -0.08f), rubberSealColor);
    RunningGear::AppendBox(mesh, Vec3(xBot - 0.055f, yBot + 0.03f, 0.08f), Vec3(xBot - 0.025f, yBot + 0.06f, hwGlass * 0.72f), rubberSealColor);
}

void CabComponents::AppendHeavyGrille(
    Mesh* mesh,
    float faceX,
    float botY, float topY,
    float halfWidth,
    int slats,
    const Vec4& surroundColor,
    const Vec4& innerBackingColor
) {
    // Dark radiator backing cavity
    RunningGear::AppendBox(
        mesh,
        Vec3(faceX - 0.01f, botY, -halfWidth),
        Vec3(faceX + 0.05f, topY,  halfWidth),
        innerBackingColor
    );
    // Chrome/metallic outer bezel border
    float b = 0.045f;
    RunningGear::AppendBox(mesh, Vec3(faceX - 0.035f, topY - b, -halfWidth - b), Vec3(faceX + 0.01f, topY + b, halfWidth + b), surroundColor);
    RunningGear::AppendBox(mesh, Vec3(faceX - 0.035f, botY - b, -halfWidth - b), Vec3(faceX + 0.01f, botY + b, halfWidth + b), surroundColor);
    RunningGear::AppendBox(mesh, Vec3(faceX - 0.035f, botY, -halfWidth - b), Vec3(faceX + 0.01f, topY, -halfWidth), surroundColor);
    RunningGear::AppendBox(mesh, Vec3(faceX - 0.035f, botY,  halfWidth), Vec3(faceX + 0.01f, topY,  halfWidth + b), surroundColor);

    int count = std::max(2, slats);
    float spanY = topY - botY;
    for (int i = 1; i <= count; ++i) {
        float sy = botY + (spanY * static_cast<float>(i)) / static_cast<float>(count + 1);
        RunningGear::AppendBox(
            mesh,
            Vec3(faceX - 0.030f, sy - 0.016f, -halfWidth),
            Vec3(faceX, sy + 0.016f, halfWidth),
            surroundColor
        );
    }
}

void CabComponents::AppendWingMirrors(
    Mesh* mesh,
    float anchorX,
    float mirrorCenterY,
    float cabHalfWidth,
    const Vec4& housingColor
) {
    Vec4 armCol(0.12f, 0.13f, 0.16f, 1.0f);
    Vec4 glassMirror(0.55f, 0.65f, 0.78f, 1.0f);
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zIn = s * cabHalfWidth;
        float zOut = s * (cabHalfWidth + 0.22f);
        float zMin = std::min(zIn, zOut);
        float zMax = std::max(zIn, zOut);

        // Upper & lower steel mounting arms
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.02f, mirrorCenterY + 0.19f, zMin), Vec3(anchorX + 0.02f, mirrorCenterY + 0.23f, zMax), armCol);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.02f, mirrorCenterY - 0.21f, zMin), Vec3(anchorX + 0.02f, mirrorCenterY - 0.17f, zMax), armCol);

        // Aerodynamic main + wide-angle mirror housing
        float mZ0 = s > 0 ? zOut - 0.03f : zOut - 0.09f;
        float mZ1 = s > 0 ? zOut + 0.09f : zOut + 0.03f;
        RunningGear::AppendBox(
            mesh,
            Vec3(anchorX - 0.06f, mirrorCenterY - 0.25f, mZ0),
            Vec3(anchorX + 0.05f, mirrorCenterY + 0.27f, mZ1),
            housingColor
        );
        // Reflective glass face (+X facing backward toward cab)
        RunningGear::AppendBox(
            mesh,
            Vec3(anchorX + 0.045f, mirrorCenterY - 0.22f, mZ0 + 0.015f),
            Vec3(anchorX + 0.062f, mirrorCenterY + 0.24f, mZ1 - 0.015f),
            glassMirror
        );
    }
}

void CabComponents::AppendFrontBumperAndLights(
    Mesh* mesh,
    float frontX,
    float botY, float topY,
    float halfWidth,
    bool headlightsOn,
    const Vec4& bumperColor
) {
    RunningGear::AppendBox(
        mesh,
        Vec3(frontX - 0.08f, botY, -halfWidth),
        Vec3(frontX + 0.20f, topY,  halfWidth),
        bumperColor
    );
    // Tow hook slot + license plate plinth
    Vec4 plateCol(0.92f, 0.93f, 0.95f, 1.0f);
    RunningGear::AppendBox(
        mesh,
        Vec3(frontX - 0.095f, botY + 0.06f, -0.26f),
        Vec3(frontX - 0.075f, botY + 0.19f,  0.26f),
        plateCol
    );

    Vec4 headCol = headlightsOn
        ? Vec4(0.99f, 0.96f, 0.80f, 1.0f)
        : Vec4(0.68f, 0.72f, 0.78f, 1.0f);
    Vec4 drlAmber(0.96f, 0.58f, 0.06f, 1.0f);

    float lightBot = botY + (topY - botY) * 0.25f;
    float lightTop = topY - 0.04f;
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float z0 = s * (halfWidth - 0.34f);
        float z1 = s * (halfWidth - 0.07f);
        RunningGear::AppendBox(
            mesh,
            Vec3(frontX - 0.10f, lightBot, std::min(z0, z1)),
            Vec3(frontX - 0.06f, lightTop, std::max(z0, z1)),
            headCol
        );
        // Side corner amber indicator
        float a0 = s * (halfWidth - 0.07f);
        float a1 = s * halfWidth;
        RunningGear::AppendBox(
            mesh,
            Vec3(frontX - 0.09f, lightBot, std::min(a0, a1)),
            Vec3(frontX + 0.06f, lightTop, std::max(a0, a1)),
            drlAmber
        );
    }
}

void CabComponents::AppendAmberRoofMarkers(
    Mesh* mesh,
    float frontX,
    float roofY,
    float halfWidth,
    bool lightsOn
) {
    Vec4 col = lightsOn
        ? Vec4(0.98f, 0.62f, 0.06f, 1.0f)
        : Vec4(0.55f, 0.35f, 0.08f, 1.0f);
    float offsets[5] = {-halfWidth * 0.72f, -0.18f, 0.0f, 0.18f, halfWidth * 0.72f};
    for (float oz : offsets) {
        RunningGear::AppendBox(
            mesh,
            Vec3(frontX, roofY, oz - 0.045f),
            Vec3(frontX + 0.09f, roofY + 0.04f, oz + 0.045f),
            col
        );
    }
}

} // namespace sphere::vehicles::cab
