// ============================================================================
// TreeImpostor.cpp - Billboard Impostor Slicing Implementation
// ============================================================================

#include "TreeImpostor.h"
#include <cmath>

namespace vegetation {

std::vector<ImpostorPlane> TreeImpostor::generateOctahedralPlanes(
    float treeHeightMeters,
    float treeWidthMeters,
    int angleSlices
) {
    std::vector<ImpostorPlane> planes;
    planes.reserve(angleSlices);

    float halfW = treeWidthMeters * 0.5f;
    float angleStep = 360.0f / static_cast<float>(angleSlices);

    for (int i = 0; i < angleSlices; ++i) {
        float deg = static_cast<float>(i) * angleStep;
        float rad = deg * (3.14159265f / 180.0f);

        Vec3 normal(std::sin(rad), 0.0f, std::cos(rad));
        Vec3 tangent(std::cos(rad), 0.0f, -std::sin(rad));

        ImpostorPlane plane;
        plane.azimuthDeg = deg;
        plane.normal = normal;

        Vec3 p0 = tangent * (-halfW);
        Vec3 p1 = tangent * (halfW);
        Vec3 p2 = tangent * (halfW) + Vec3(0, treeHeightMeters, 0);
        Vec3 p3 = tangent * (-halfW) + Vec3(0, treeHeightMeters, 0);

        plane.quadVerts[0] = {p0.toC(), normal.toC(), {0.0f, 0.0f}, {0.3f, 0.5f, 0.2f, 1.0f}, 0.0f};
        plane.quadVerts[1] = {p1.toC(), normal.toC(), {1.0f, 0.0f}, {0.3f, 0.5f, 0.2f, 1.0f}, 0.0f};
        plane.quadVerts[2] = {p2.toC(), normal.toC(), {1.0f, 1.0f}, {0.3f, 0.5f, 0.2f, 1.0f}, 0.2f};
        plane.quadVerts[3] = {p3.toC(), normal.toC(), {0.0f, 1.0f}, {0.3f, 0.5f, 0.2f, 1.0f}, 0.2f};

        planes.push_back(plane);
    }

    return planes;
}

} // namespace vegetation
