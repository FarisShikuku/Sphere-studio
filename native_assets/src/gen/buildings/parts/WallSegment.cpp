#include "WallSegment.h"
#include "../BuildingComponents.h"
#include <cmath>

namespace procedural::buildings {

void WallSegment::generateWall(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    std::vector<Socket>& sockets,
    const WallSegmentParams& params
) {
    Vec3 diff = params.end - params.start;
    float len = diff.length();
    if (len < 1e-4f) return;

    Vec3 dir = diff / len;
    Vec3 norm{-dir.z, 0.0f, dir.x}; // Outward normal in XZ plane
    norm = norm.normalized();

    float h = params.height;
    float t = params.thickness;
    Vec3 offsetNorm = norm * (t * 0.5f);

    if (!params.hasOpening) {
        // Solid wall segment
        Vec3 p0 = params.start + offsetNorm;
        Vec3 p1 = params.end + offsetNorm;
        Vec3 p2 = p1 + Vec3{0.0f, h, 0.0f};
        Vec3 p3 = p0 + Vec3{0.0f, h, 0.0f};

        BuildingComponents::appendQuad(vertices, indices, p0, p1, p2, p3, norm, params.materialId);

        // Inner face
        Vec3 ip0 = params.end - offsetNorm;
        Vec3 ip1 = params.start - offsetNorm;
        Vec3 ip2 = ip1 + Vec3{0.0f, h, 0.0f};
        Vec3 ip3 = ip0 + Vec3{0.0f, h, 0.0f};
        BuildingComponents::appendQuad(vertices, indices, ip0, ip1, ip2, ip3, -norm, params.materialId);
    } else {
        // Wall with opening: Left pier, right pier, sill block, lintel block
        float opStartDist = len * params.openingStartT;
        float opEndDist = len * params.openingEndT;

        Vec3 ptA = params.start;
        Vec3 ptB = params.start + dir * opStartDist;
        Vec3 ptC = params.start + dir * opEndDist;
        Vec3 ptD = params.end;

        // 1. Left pier (A to B, full height)
        Vec3 p0 = ptA + offsetNorm;
        Vec3 p1 = ptB + offsetNorm;
        Vec3 p2 = p1 + Vec3{0.0f, h, 0.0f};
        Vec3 p3 = p0 + Vec3{0.0f, h, 0.0f};
        BuildingComponents::appendQuad(vertices, indices, p0, p1, p2, p3, norm, params.materialId);

        // 2. Right pier (C to D, full height)
        p0 = ptC + offsetNorm;
        p1 = ptD + offsetNorm;
        p2 = p1 + Vec3{0.0f, h, 0.0f};
        p3 = p0 + Vec3{0.0f, h, 0.0f};
        BuildingComponents::appendQuad(vertices, indices, p0, p1, p2, p3, norm, params.materialId);

        // 3. Spandrel sill below window opening (B to C, 0 to sillHeight)
        if (params.sillHeight > 0.05f) {
            p0 = ptB + offsetNorm;
            p1 = ptC + offsetNorm;
            p2 = p1 + Vec3{0.0f, params.sillHeight, 0.0f};
            p3 = p0 + Vec3{0.0f, params.sillHeight, 0.0f};
            BuildingComponents::appendQuad(vertices, indices, p0, p1, p2, p3, norm, params.materialId);
        }

        // 4. Lintel above window opening (B to C, headerHeight to h)
        if (params.headerHeight < h - 0.05f) {
            p0 = ptB + offsetNorm + Vec3{0.0f, params.headerHeight, 0.0f};
            p1 = ptC + offsetNorm + Vec3{0.0f, params.headerHeight, 0.0f};
            p2 = p1 + Vec3{0.0f, h - params.headerHeight, 0.0f};
            p3 = p0 + Vec3{0.0f, h - params.headerHeight, 0.0f};
            BuildingComponents::appendQuad(vertices, indices, p0, p1, p2, p3, norm, params.materialId);
        }

        // Register socket for window or door insert
        Socket sock;
        sock.type = (params.sillHeight < 0.1f) ? SocketType::Door : SocketType::Window;
        sock.id = "wall_opening_f" + std::to_string(params.floorIndex);
        sock.position = (ptB + ptC) * 0.5f + offsetNorm + Vec3{0.0f, (params.sillHeight + params.headerHeight) * 0.5f, 0.0f};
        sock.normal = norm;
        sock.tangent = dir;
        sock.width = opEndDist - opStartDist;
        sock.height = params.headerHeight - params.sillHeight;
        sockets.push_back(sock);
    }
}

} // namespace procedural::buildings
