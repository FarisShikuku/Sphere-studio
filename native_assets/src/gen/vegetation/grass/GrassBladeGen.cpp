// ============================================================================
// GrassBladeGen.cpp - Parametric Grass Blade Tessellator
// ============================================================================

#include "GrassBladeGen.h"
#include <cmath>

namespace vegetation {

GrassBladeMeshSegment GrassBladeGen::buildBladeGeometry(
    const GrassBlade_C& blade,
    int verticalSegments,
    float windDeflectionRad
) {
    GrassBladeMeshSegment mesh;
    uint32_t baseIdx = 0;

    float cosH = std::cos(blade.headingRad);
    float sinH = std::sin(blade.headingRad);
    Vec3 sideDir(cosH, 0.0f, sinH);

    float totalCurvature = blade.curvature + windDeflectionRad;

    for (int seg = 0; seg <= verticalSegments; ++seg) {
        float t = static_cast<float>(seg) / static_cast<float>(verticalSegments);
        float width = blade.widthMeters * (1.0f - t * 0.85f);
        float currentHeight = blade.heightMeters * t;
        float forwardBend = totalCurvature * (t * t) * blade.heightMeters * 0.5f;

        Vec3 center(
            blade.basePosition.x - sinH * forwardBend,
            blade.basePosition.y + currentHeight,
            blade.basePosition.z + cosH * forwardBend
        );

        Vec3 normal(sinH, 0.3f, -cosH);
        normal = normal.normalized();

        Vec3 pLeft = center - sideDir * (width * 0.5f);
        Vec3 pRight = center + sideDir * (width * 0.5f);

        float r = 0.18f + 0.15f * t;
        float g = 0.45f + 0.25f * t;
        float b = 0.12f;

        mesh.vertices.push_back({pLeft.toC(), normal.toC(), {0.0f, t}, {r, g, b, 1.0f}, t * blade.windBendFactor});
        mesh.vertices.push_back({pRight.toC(), normal.toC(), {1.0f, t}, {r, g, b, 1.0f}, t * blade.windBendFactor});
    }

    // Connect vertical strips with quads
    for (int seg = 0; seg < verticalSegments; ++seg) {
        uint32_t v0 = seg * 2;
        uint32_t v1 = v0 + 1;
        uint32_t v2 = v0 + 2;
        uint32_t v3 = v0 + 3;

        mesh.indices.push_back(v0);
        mesh.indices.push_back(v1);
        mesh.indices.push_back(v2);

        mesh.indices.push_back(v1);
        mesh.indices.push_back(v3);
        mesh.indices.push_back(v2);
    }

    return mesh;
}

} // namespace vegetation
