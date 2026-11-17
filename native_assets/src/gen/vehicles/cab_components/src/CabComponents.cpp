#include "CabComponents.h"
#include "ChassisComponents.h"
#include "RunningGear.h"
#include <algorithm>
#include <cmath>

namespace sphere::vehicles::cab {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::chassis::ChassisComponents;

namespace {
constexpr float kPi = 3.14159265358979323846f;

inline void PushTri(Mesh* mesh, const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec4& col) {
    uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
    Vertex v0; v0.position = p0; v0.color = col; v0.uv[0] = 0.0f; v0.uv[1] = 0.0f;
    Vertex v1; v1.position = p1; v1.color = col; v1.uv[0] = 1.0f; v1.uv[1] = 0.0f;
    Vertex v2; v2.position = p2; v2.color = col; v2.uv[0] = 1.0f; v2.uv[1] = 1.0f;
    mesh->vertices.push_back(v0);
    mesh->vertices.push_back(v1);
    mesh->vertices.push_back(v2);
    mesh->indices.push_back(base + 0);
    mesh->indices.push_back(base + 1);
    mesh->indices.push_back(base + 2);
}

// Rigid Y-axis rotated watertight box around (pivotX, pivotZ) preserving outward-CCW winding
void AppendBoxRotY(
    Mesh* mesh,
    const Vec3& minP, const Vec3& maxP,
    float pivotX, float pivotZ,
    float angleRad,
    const Vec4& color
) {
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    auto xf = [&](float x, float y, float z) -> Vec3 {
        float dx = x - pivotX;
        float dz = z - pivotZ;
        return Vec3(pivotX + dx * c - dz * s, y, pivotZ + dx * s + dz * c);
    };
    uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
    Vec3 corners[8] = {
        xf(minP.x, minP.y, maxP.z), xf(maxP.x, minP.y, maxP.z),
        xf(maxP.x, maxP.y, maxP.z), xf(minP.x, maxP.y, maxP.z),
        xf(minP.x, minP.y, minP.z), xf(maxP.x, minP.y, minP.z),
        xf(maxP.x, maxP.y, minP.z), xf(minP.x, maxP.y, minP.z)
    };
    for (int i = 0; i < 8; ++i) {
        Vertex v;
        v.position = corners[i];
        v.uv[0] = (i % 2 == 0) ? 0.0f : 1.0f;
        v.uv[1] = (i < 4) ? 0.0f : 1.0f;
        v.color = color;
        mesh->vertices.push_back(v);
    }
    const uint32_t tris[36] = {
        0, 1, 2, 0, 2, 3,
        5, 4, 7, 5, 7, 6,
        3, 2, 6, 3, 6, 7,
        4, 5, 1, 4, 1, 0,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7
    };
    for (int i = 0; i < 36; ++i) {
        mesh->indices.push_back(base + tris[i]);
    }
}

// Build CCW ring points in Y-Z plane (Bottom->Right->Top->Left) so (T_x x T_k) always points OUTWARD
std::vector<Vec3> BuildStationRing(const HullStationX& st, int arcSegs) {
    std::vector<Vec3> pts;
    float hw = std::max(0.15f, st.halfWidth);
    float spanY = std::max(0.08f, st.yTop - st.yBot);
    float rBot = std::clamp(st.botCornerR, 0.01f, std::min(hw * 0.45f, spanY * 0.35f));
    float rTop = std::clamp(st.topCornerR, 0.01f, std::min(hw * 0.48f, spanY * 0.40f));

    auto addPt = [&](float y, float z) {
        float normZ = std::clamp(std::abs(z) / hw, 0.0f, 1.0f);
        float dome = st.frontCurvature * (1.0f - normZ * normZ);
        float roofArch = 0.0f;
        if (y > (st.yBot + st.yTop) * 0.5f) {
            roofArch = st.roofCrown * (1.0f - normZ * normZ);
        }
        pts.emplace_back(st.x - dome, y + roofArch, z);
    };

    for (int i = 0; i <= arcSegs; ++i) {
        float a = -kPi * 0.5f + (kPi * 0.5f) * (static_cast<float>(i) / static_cast<float>(arcSegs));
        addPt(st.yBot + rBot + rBot * std::sin(a), (hw - rBot) + rBot * std::cos(a));
    }
    for (int i = 0; i <= arcSegs; ++i) {
        float a = (kPi * 0.5f) * (static_cast<float>(i) / static_cast<float>(arcSegs));
        addPt(st.yTop - rTop + rTop * std::sin(a), (hw - rTop) + rTop * std::cos(a));
    }
    addPt(st.yTop, 0.0f);
    for (int i = 0; i <= arcSegs; ++i) {
        float a = kPi * 0.5f + (kPi * 0.5f) * (static_cast<float>(i) / static_cast<float>(arcSegs));
        addPt(st.yTop - rTop + rTop * std::sin(a), -(hw - rTop) + rTop * std::cos(a));
    }
    for (int i = 0; i <= arcSegs; ++i) {
        float a = kPi + (kPi * 0.5f) * (static_cast<float>(i) / static_cast<float>(arcSegs));
        addPt(st.yBot + rBot + rBot * std::sin(a), -(hw - rBot) + rBot * std::cos(a));
    }
    return pts;
}
} // namespace

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
        v.color = color; // Preserve alpha for glass pass vs 1.0f for opaque pass
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
    Vec3 fBL(xFront, yFrontBot, -hwFront);
    Vec3 fBR(xFront, yFrontBot,  hwFront);
    Vec3 fTR(xFront, yFrontTop,  hwFront);
    Vec3 fTL(xFront, yFrontTop, -hwFront);

    Vec3 bBL(xBack, yBackBot, -hwBack);
    Vec3 bBR(xBack, yBackBot,  hwBack);
    Vec3 bTR(xBack, yBackTop,  hwBack);
    Vec3 bTL(xBack, yBackTop, -hwBack);

    // Strictly verified outward-CCW normals
    AppendQuad(mesh, fBL, fBR, fTR, fTL, color); // -X front
    AppendQuad(mesh, bBR, bBL, bTL, bTR, color); // +X rear
    AppendQuad(mesh, fTL, fTR, bTR, bTL, color); // +Y top
    AppendQuad(mesh, bBL, bBR, fBR, fBL, color); // -Y bottom
    AppendQuad(mesh, bBL, fBL, fTL, bTL, color); // -Z left
    AppendQuad(mesh, fBR, bBR, bTR, fTR, color); // +Z right
}

void CabComponents::AppendSculptedHullX(
    Mesh* mesh,
    const std::vector<HullStationX>& stations,
    const Vec4& color,
    int arcSegments
) {
    if (!mesh || stations.size() < 2) return;
    int segs = std::max(2, arcSegments);

    std::vector<std::vector<Vec3>> rings;
    rings.reserve(stations.size());
    for (const auto& st : stations) {
        rings.push_back(BuildStationRing(st, segs));
    }

    size_t nPts = rings[0].size();
    for (size_t s = 0; s + 1 < rings.size(); ++s) {
        const auto& r0 = rings[s];
        const auto& r1 = rings[s + 1];
        for (size_t k = 0; k < nPts; ++k) {
            size_t kn = (k + 1) % nPts;
            AppendQuad(mesh, r0[k], r1[k], r1[kn], r0[kn], color);
        }
    }

    const auto& frontRing = rings.front();
    Vec3 cFront(
        stations.front().x - stations.front().frontCurvature - 0.01f,
        (stations.front().yBot + stations.front().yTop) * 0.5f + stations.front().roofCrown * 0.4f,
        0.0f
    );
    for (size_t k = 0; k < nPts; ++k) {
        size_t kn = (k + 1) % nPts;
        PushTri(mesh, cFront, frontRing[k], frontRing[kn], color);
    }

    const auto& rearRing = rings.back();
    Vec3 cRear(
        stations.back().x + 0.01f,
        (stations.back().yBot + stations.back().yTop) * 0.5f + stations.back().roofCrown * 0.4f,
        0.0f
    );
    for (size_t k = 0; k < nPts; ++k) {
        size_t kn = (k + 1) % nPts;
        PushTri(mesh, cRear, rearRing[kn], rearRing[k], color);
    }
}

void CabComponents::AppendWheelArchFender(
    Mesh* mesh,
    float axleX,
    float wheelCenterY,
    float archRadius,
    float sideZ,
    bool isRightSide,
    float fenderDepth,
    const Vec4& color
) {
    if (!mesh) return;
    int segs = 22;
    float rIn = archRadius;
    float rOut = archRadius + 0.068f;
    float zIn = isRightSide ? sideZ - 0.045f : sideZ + 0.045f;
    float zOut = isRightSide ? sideZ + fenderDepth : sideZ - fenderDepth;
    float zMin = std::min(zIn, zOut);
    float zMax = std::max(zIn, zOut);

    for (int i = 0; i < segs; ++i) {
        float a0 = kPi * (static_cast<float>(i) / static_cast<float>(segs));
        float a1 = kPi * (static_cast<float>(i + 1) / static_cast<float>(segs));
        float c0 = std::cos(a0), s0 = std::sin(a0);
        float c1 = std::cos(a1), s1 = std::sin(a1);

        Vec3 i0L(axleX + rIn * c0,  wheelCenterY + rIn * s0,  zMin);
        Vec3 i1L(axleX + rIn * c1,  wheelCenterY + rIn * s1,  zMin);
        Vec3 o0L(axleX + rOut * c0, wheelCenterY + rOut * s0, zMin);
        Vec3 o1L(axleX + rOut * c1, wheelCenterY + rOut * s1, zMin);

        Vec3 i0R(axleX + rIn * c0,  wheelCenterY + rIn * s0,  zMax);
        Vec3 i1R(axleX + rIn * c1,  wheelCenterY + rIn * s1,  zMax);
        Vec3 o0R(axleX + rOut * c0, wheelCenterY + rOut * s0, zMax);
        Vec3 o1R(axleX + rOut * c1, wheelCenterY + rOut * s1, zMax);

        AppendQuad(mesh, o0L, o1L, o1R, o0R, color);
        AppendQuad(mesh, i1L, i0L, i0R, i1R, color);
        AppendQuad(mesh, o0L, i0L, i1L, o1L, color);
        AppendQuad(mesh, i0R, o0R, o1R, i1R, color);
    }
}

void CabComponents::AppendWheelWellLiner(
    Mesh* mesh,
    float axleX,
    float archHalfLen,
    float floorY,
    float archTopY,
    float outerAbsZ,
    float innerAbsZ,
    const Vec4& linerColor
) {
    if (!mesh) return;
    // Derive concentric semicircular tub matching wheel center (archTopY = wheelR*2 + 0.14)
    float spanH = std::max(0.40f, archTopY - 0.12f);
    float wheelCenterY = spanH * 0.5f;
    float rTub = std::min(archHalfLen - 0.015f, (archTopY - wheelCenterY) - 0.01f);
    float inZ = std::max(0.20f, std::min(innerAbsZ, outerAbsZ - 0.12f));
    const int segs = 20;

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zA = s * inZ;
        float zB = s * outerAbsZ;
        float zMin = std::min(zA, zB);
        float zMax = std::max(zA, zB);
        float zBack = s * inZ;

        // Curved semicircular ceiling barrel facing inward toward tire
        Vec3 cBack(axleX, wheelCenterY, zBack);
        for (int i = 0; i < segs; ++i) {
            float a0 = kPi * (static_cast<float>(i) / static_cast<float>(segs));
            float a1 = kPi * (static_cast<float>(i + 1) / static_cast<float>(segs));
            float x0 = axleX + rTub * std::cos(a0), y0 = wheelCenterY + rTub * std::sin(a0);
            float x1 = axleX + rTub * std::cos(a1), y1 = wheelCenterY + rTub * std::sin(a1);
            Vec3 p0L(x0, y0, zMin), p1L(x1, y1, zMin);
            Vec3 p0R(x0, y0, zMax), p1R(x1, y1, zMax);
            AppendQuad(mesh, p0L, p0R, p1R, p1L, linerColor);
            // Semicircular inner tub bulkhead facing outward toward wheel
            if (side > 0) {
                PushTri(mesh, cBack, Vec3(x0, y0, zBack), Vec3(x1, y1, zBack), linerColor);
            } else {
                PushTri(mesh, cBack, Vec3(x1, y1, zBack), Vec3(x0, y0, zBack), linerColor);
            }
        }
        // Lower mudguard vertical drops from wheelCenterY down to floorY
        if (wheelCenterY > floorY + 0.02f) {
            RunningGear::AppendBox(mesh, Vec3(axleX - rTub - 0.03f, floorY, zMin), Vec3(axleX - rTub + 0.01f, wheelCenterY, zMax), linerColor);
            RunningGear::AppendBox(mesh, Vec3(axleX + rTub - 0.01f, floorY, zMin), Vec3(axleX + rTub + 0.03f, wheelCenterY, zMax), linerColor);
            RunningGear::AppendBox(mesh, Vec3(axleX - rTub, floorY, side > 0 ? inZ - 0.03f : -inZ), Vec3(axleX + rTub, wheelCenterY, side > 0 ? inZ : -inZ + 0.03f), linerColor);
        }
    }
}

void CabComponents::AppendSemicircularWheelArchSkirt(
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
) {
    if (!mesh || cutRadius <= 0.15f) return;
    float rCut = std::clamp(cutRadius, 0.20f, std::min(archHalfLen - 0.02f, archTopY - wheelCenterY - 0.02f));
    float xMin = axleX - archHalfLen;
    float xMax = axleX + archHalfLen;
    float zOut = sideZ;
    float zIn  = isRightSide ? (sideZ - thickness) : (sideZ + thickness);
    float zMin = std::min(zIn, zOut);
    float zMax = std::max(zIn, zOut);

    // Build sorted angle list [0 .. kPi] including exact corner rays to hit (xMax, archTopY) & (xMin, archTopY) cleanly
    float cornerAngle = std::atan2(archTopY - wheelCenterY, xMax - axleX);
    std::vector<float> angles;
    const int baseSegs = 20;
    for (int i = 0; i <= baseSegs; ++i) {
        angles.push_back(kPi * (static_cast<float>(i) / static_cast<float>(baseSegs)));
    }
    angles.push_back(cornerAngle);
    angles.push_back(kPi - cornerAngle);
    std::sort(angles.begin(), angles.end());
    angles.erase(std::unique(angles.begin(), angles.end(), [](float a, float b) { return std::abs(a - b) < 1e-4f; }), angles.end());

    auto projectToBayRect = [&](float theta) -> Vec3 {
        float dx = std::cos(theta);
        float dy = std::sin(theta);
        float tx = (std::abs(dx) > 1e-5f) ? ((dx >= 0.0f ? (xMax - axleX) : (axleX - xMin)) / std::abs(dx)) : 1e6f;
        float ty = (dy > 1e-5f) ? ((archTopY - wheelCenterY) / dy) : 1e6f;
        float t = std::min(tx, ty);
        return Vec3(axleX + t * dx, wheelCenterY + t * dy, 0.0f);
    };

    for (size_t i = 0; i + 1 < angles.size(); ++i) {
        float a0 = angles[i], a1 = angles[i + 1];
        float px0 = axleX + rCut * std::cos(a0), py0 = wheelCenterY + rCut * std::sin(a0);
        float px1 = axleX + rCut * std::cos(a1), py1 = wheelCenterY + rCut * std::sin(a1);
        Vec3 out0 = projectToBayRect(a0);
        Vec3 out1 = projectToBayRect(a1);

        Vec3 p0Out(px0, py0, zOut), p1Out(px1, py1, zOut);
        Vec3 q0Out(out0.x, out0.y, zOut), q1Out(out1.x, out1.y, zOut);
        Vec3 p0In(px0, py0, zIn), p1In(px1, py1, zIn);
        Vec3 q0In(out0.x, out0.y, zIn), q1In(out1.x, out1.y, zIn);

        if (isRightSide) {
            AppendQuad(mesh, q0Out, q1Out, p1Out, p0Out, skirtColor);
            AppendQuad(mesh, p0In, p1In, q1In, q0In, innerColor);
        } else {
            AppendQuad(mesh, p0Out, p1Out, q1Out, q0Out, skirtColor);
            AppendQuad(mesh, q0In, q1In, p1In, p0In, innerColor);
        }
        // Clean curved semicircular inner edge bevel (thickness)
        Vec3 p0L(px0, py0, zMin), p1L(px1, py1, zMin);
        Vec3 p0R(px0, py0, zMax), p1R(px1, py1, zMax);
        AppendQuad(mesh, p0L, p0R, p1R, p1L, skirtColor);
    }

    // Lower straight painted columns below wheelCenterY down to floorY
    if (wheelCenterY > floorY + 0.01f) {
        if (xMin < axleX - rCut - 0.005f) {
            RunningGear::AppendBox(mesh, Vec3(xMin, floorY, zMin), Vec3(axleX - rCut, wheelCenterY, zMax), skirtColor);
        }
        if (axleX + rCut < xMax - 0.005f) {
            RunningGear::AppendBox(mesh, Vec3(axleX + rCut, floorY, zMin), Vec3(xMax, wheelCenterY, zMax), skirtColor);
        }
    }
}

void CabComponents::AppendSkirtWithCutouts(
    Mesh* mesh,
    float xStart, float xEnd,
    float yBot, float yTop,
    float sideZ, bool isRightSide,
    float thickness,
    const std::vector<std::pair<float, float>>& skipXRanges,
    const Vec4& color
) {
    if (!mesh || xEnd <= xStart + 0.02f || yTop <= yBot + 0.02f) return;
    std::vector<std::pair<float, float>> sorted = skipXRanges;
    std::sort(sorted.begin(), sorted.end());

    float zMin = isRightSide ? (sideZ - thickness) : sideZ;
    float zMax = isRightSide ? sideZ : (sideZ + thickness);

    float cur = xStart;
    for (const auto& r : sorted) {
        float r0 = std::max(cur, r.first);
        float r1 = std::min(xEnd, r.second);
        if (r0 > cur + 0.03f) {
            RunningGear::AppendBox(mesh, Vec3(cur, yBot, zMin), Vec3(r0, yTop, zMax), color);
        }
        cur = std::max(cur, r1);
    }
    if (xEnd > cur + 0.03f) {
        RunningGear::AppendBox(mesh, Vec3(cur, yBot, zMin), Vec3(xEnd, yTop, zMax), color);
    }
}

void CabComponents::AppendCantileverCoachMirrors(
    Mesh* mesh,
    float pivotX,
    float pivotY,
    float halfWidth,
    const Vec4& housingColor
) {
    if (!mesh) return;
    Vec4 glassMirror(0.58f, 0.68f, 0.80f, 1.0f);
    Vec4 darkBezel(0.10f, 0.11f, 0.14f, 1.0f);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zMount = s * (halfWidth - 0.04f);
        float zOut = s * (halfWidth + 0.25f);
        float zMin = std::min(zMount, zOut);
        float zMax = std::max(zMount, zOut);

        RunningGear::AppendBox(
            mesh,
            Vec3(pivotX - 0.36f, pivotY + 0.04f, zMin),
            Vec3(pivotX + 0.04f, pivotY + 0.11f, zMax),
            housingColor
        );
        float dropX = pivotX - 0.35f;
        float mTop = pivotY + 0.06f;
        float mBot = pivotY - 0.38f;
        float hz0 = s > 0 ? zOut - 0.04f : zOut - 0.10f;
        float hz1 = s > 0 ? zOut + 0.10f : zOut + 0.04f;
        RunningGear::AppendBox(mesh, Vec3(dropX - 0.06f, mBot, hz0), Vec3(dropX + 0.06f, mTop, hz1), housingColor);
        RunningGear::AppendBox(mesh, Vec3(dropX + 0.04f, mBot + 0.03f, hz0 + 0.01f), Vec3(dropX + 0.072f, mTop - 0.03f, hz1 - 0.01f), darkBezel);
        RunningGear::AppendBox(mesh, Vec3(dropX + 0.068f, mBot + 0.05f, hz0 + 0.02f), Vec3(dropX + 0.082f, mTop - 0.05f, hz1 - 0.02f), glassMirror);
    }
}

void CabComponents::AppendModernLedFascia(
    Mesh* mesh,
    float frontX,
    float botY, float topY,
    float halfWidth,
    bool headlightsOn,
    const Vec4& bumperColor,
    const Vec4& maskColor
) {
    if (!mesh) return;
    float hSpan = topY - botY;
    float chinY = botY + hSpan * 0.38f;

    AppendFrustumX(
        mesh,
        frontX - 0.10f, botY, chinY, halfWidth - 0.06f,
        frontX + 0.24f, botY, topY,  halfWidth,
        bumperColor
    );
    RunningGear::AppendBox(mesh, Vec3(frontX - 0.095f, botY + 0.06f, -0.52f), Vec3(frontX + 0.02f, chinY + 0.06f, 0.52f), maskColor);
    RunningGear::AppendBox(mesh, Vec3(frontX - 0.11f, botY + 0.09f, -0.26f), Vec3(frontX - 0.085f, botY + 0.20f, 0.26f), Vec4(0.92f, 0.94f, 0.96f, 1.0f));

    Vec4 drlWhite = headlightsOn ? Vec4(0.96f, 0.99f, 1.00f, 1.0f) : Vec4(0.64f, 0.70f, 0.78f, 1.0f);
    Vec4 projCore = headlightsOn ? Vec4(0.84f, 0.95f, 1.00f, 1.0f) : Vec4(0.45f, 0.52f, 0.62f, 1.0f);
    Vec4 fogCol   = headlightsOn ? Vec4(0.96f, 0.96f, 0.88f, 1.0f) : Vec4(0.55f, 0.58f, 0.62f, 1.0f);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zInner = s * (halfWidth * 0.44f);
        float zOuter = s * (halfWidth - 0.07f);
        float z0 = std::min(zInner, zOuter);
        float z1 = std::max(zInner, zOuter);

        RunningGear::AppendBox(mesh, Vec3(frontX - 0.085f, chinY + 0.02f, z0), Vec3(frontX + 0.05f, topY - 0.02f, z1), maskColor);
        RunningGear::AppendBox(mesh, Vec3(frontX - 0.100f, topY - 0.07f, z0 + 0.02f), Vec3(frontX - 0.07f, topY - 0.03f, z1 - 0.02f), drlWhite);
        float zMid = (z0 + z1) * 0.5f;
        RunningGear::AppendBox(mesh, Vec3(frontX - 0.104f, chinY + 0.05f, zMid - 0.11f), Vec3(frontX - 0.07f, chinY + 0.14f, zMid - 0.02f), projCore);
        RunningGear::AppendBox(mesh, Vec3(frontX - 0.104f, chinY + 0.05f, zMid + 0.02f), Vec3(frontX - 0.07f, chinY + 0.14f, zMid + 0.11f), projCore);
        float f0 = s * (halfWidth - 0.24f);
        float f1 = s * (halfWidth - 0.11f);
        RunningGear::AppendBox(mesh, Vec3(frontX - 0.104f, botY + 0.05f, std::min(f0, f1)), Vec3(frontX - 0.07f, botY + 0.14f, std::max(f0, f1)), fogCol);
    }
}

void CabComponents::AppendCurvedBlackNoseMask(
    Mesh* mesh,
    float xFrontNose, float xRoofBrow,
    float yMaskBot, float yWinBot, float yWinTop,
    float halfWidth,
    float bulgeX,
    const Vec4& maskColor,
    const Vec4& ledDisplayColor,
    bool addLedSign,
    bool addWipers
) {
    if (!mesh) return;
    Vec4 solidMask(maskColor.x, maskColor.y, maskColor.z, 1.0f);
    Vec4 chromeBadge(0.86f, 0.89f, 0.94f, 1.0f);

    int segs = 6;
    float hwG = std::max(0.3f, halfWidth - 0.02f);
    float xMidBase = xFrontNose + (xRoofBrow - xFrontNose) * 0.18f;

    for (int i = 0; i < segs; ++i) {
        float t0 = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(segs));
        float t1 = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(segs));
        float z0 = t0 * hwG;
        float z1 = t1 * hwG;
        float b0 = bulgeX * (1.0f - t0 * t0);
        float b1 = bulgeX * (1.0f - t1 * t1);

        // Strictly outward-CCW (-X front normal: (mBL -> mBR -> mTR -> mTL))
        Vec3 mBL(xFrontNose - b0 - 0.025f, yMaskBot, z0);
        Vec3 mBR(xFrontNose - b1 - 0.025f, yMaskBot, z1);
        Vec3 mTR(xMidBase - b1 - 0.030f,  yWinBot,  z1);
        Vec3 mTL(xMidBase - b0 - 0.030f,  yWinBot,  z0);
        AppendQuad(mesh, mBL, mBR, mTR, mTL, solidMask);
    }

    // Center metallic emblem bar across black V-mask
    RunningGear::AppendBox(
        mesh,
        Vec3(xFrontNose - bulgeX - 0.05f, (yMaskBot + yWinBot) * 0.5f - 0.035f, -0.34f),
        Vec3(xFrontNose - bulgeX - 0.015f, (yMaskBot + yWinBot) * 0.5f + 0.035f,  0.34f),
        chromeBadge
    );

    // Top illuminated LED route destination sign inside upper windshield brow
    if (addLedSign) {
        float destX = xRoofBrow - bulgeX * 0.86f;
        RunningGear::AppendBox(
            mesh,
            Vec3(destX - 0.01f, yWinTop - 0.24f, -0.68f),
            Vec3(destX + 0.05f, yWinTop - 0.07f,  0.68f),
            solidMask
        );
        RunningGear::AppendBox(
            mesh,
            Vec3(destX - 0.016f, yWinTop - 0.21f, -0.58f),
            Vec3(destX + 0.01f,  yWinTop - 0.10f,  0.58f),
            Vec4(ledDisplayColor.x, ledDisplayColor.y, ledDisplayColor.z, 1.0f)
        );
    }

    if (addWipers) {
        RunningGear::AppendBox(
            mesh,
            Vec3(xMidBase - bulgeX - 0.055f, yWinBot + 0.03f, -hwG * 0.72f),
            Vec3(xMidBase - bulgeX - 0.025f, yWinBot + 0.06f, -0.06f),
            solidMask
        );
        RunningGear::AppendBox(
            mesh,
            Vec3(xMidBase - bulgeX - 0.055f, yWinBot + 0.03f, 0.06f),
            Vec3(xMidBase - bulgeX - 0.025f, yWinBot + 0.06f, hwG * 0.72f),
            solidMask
        );
    }
}

void CabComponents::AppendCurvedWindshieldGlass(
    Mesh* mesh,
    float xFrontNose, float xRoofBrow,
    float yWinBot, float yWinTop,
    float halfWidth,
    float bulgeX,
    const Vec4& glassColor
) {
    if (!mesh) return;
    int segs = 6;
    float hwG = std::max(0.3f, halfWidth - 0.03f);
    float xMidBase = xFrontNose + (xRoofBrow - xFrontNose) * 0.18f;

    for (int i = 0; i < segs; ++i) {
        float t0 = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(segs));
        float t1 = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(segs));
        float z0 = t0 * hwG;
        float z1 = t1 * hwG;
        float b0 = bulgeX * (1.0f - t0 * t0);
        float b1 = bulgeX * (1.0f - t1 * t1);

        // Strictly outward-CCW (-X front normal: (gBL -> gBR -> gTR -> gTL))
        Vec3 gBL(xMidBase - b0 - 0.032f, yWinBot, z0);
        Vec3 gBR(xMidBase - b1 - 0.032f, yWinBot, z1);
        Vec3 gTR(xRoofBrow - b1 * 0.85f - 0.020f, yWinTop, z1);
        Vec3 gTL(xRoofBrow - b0 * 0.85f - 0.020f, yWinTop, z0);
        AppendQuad(mesh, gBL, gBR, gTR, gTL, glassColor);
    }
}

void CabComponents::AppendCurvedBlackGlassMask(
    Mesh* mesh,
    float xFrontNose, float xRoofBrow,
    float yMaskBot, float yWinBot, float yWinTop,
    float halfWidth,
    float bulgeX,
    const Vec4& glassColor,
    const Vec4& maskColor,
    const Vec4& ledDisplayColor,
    bool addWipers
) {
    AppendCurvedBlackNoseMask(
        mesh, xFrontNose, xRoofBrow, yMaskBot, yWinBot, yWinTop,
        halfWidth, bulgeX, maskColor, ledDisplayColor, true, addWipers
    );
    AppendCurvedWindshieldGlass(
        mesh, xFrontNose, xRoofBrow, yWinBot, yWinTop,
        halfWidth, bulgeX, glassColor
    );
}

void CabComponents::AppendSwoopingCoachGlassSide(
    Mesh* mesh,
    float xApillar, float xDropEnd, float xRear,
    float yFrontLowBot, float yMainBeltBot, float yWinTop,
    float halfWidth,
    int windowPanes,
    const Vec4& glassColor,
    const Vec4& trimColor,
    bool emitGlassOnly,
    bool emitMullionsOnly,
    float rightDoorSkipX0,
    float rightDoorSkipX1,
    float xSwoopStart,
    float yRearBeltBot
) {
    if (!mesh || xRear <= xDropEnd) return;
    Vec4 solidTrim(trimColor.x, trimColor.y, trimColor.z, 1.0f);
    float swoop0 = (xSwoopStart > xApillar + 0.05f && xSwoopStart < xDropEnd - 0.05f) ? xSwoopStart : xApillar;
    float xMid = swoop0 + (xDropEnd - swoop0) * 0.55f;
    float yMidBot = yFrontLowBot + (yMainBeltBot - yFrontLowBot) * 0.42f;
    float endMainY = (yRearBeltBot > 0.0f) ? yRearBeltBot : yMainBeltBot;

    auto evalBotY = [&](float x) -> float {
        if (x <= swoop0) return yFrontLowBot;
        if (x < xMid) {
            float t = (x - swoop0) / std::max(0.01f, xMid - swoop0);
            return yFrontLowBot + (yMidBot - yFrontLowBot) * t;
        }
        if (x < xDropEnd) {
            float t = (x - xMid) / std::max(0.01f, xDropEnd - xMid);
            return yMidBot + (yMainBeltBot - yMidBot) * t;
        }
        if (xRear > xDropEnd + 0.01f) {
            float t = std::clamp((x - xDropEnd) / (xRear - xDropEnd), 0.0f, 1.0f);
            return yMainBeltBot + (endMainY - yMainBeltBot) * t;
        }
        return endMainY;
    };

    auto emitRibbonQuad = [&](float a, float b, float zOut, bool positiveZ) {
        if (b <= a + 0.01f) return;
        float yA = evalBotY(a);
        float yB = evalBotY(b);
        if (positiveZ) {
            AppendQuad(mesh, Vec3(a, yA, zOut), Vec3(b, yB, zOut), Vec3(b, yWinTop, zOut), Vec3(a, yWinTop, zOut), glassColor);
        } else {
            AppendQuad(mesh, Vec3(b, yB, zOut), Vec3(a, yA, zOut), Vec3(a, yWinTop, zOut), Vec3(b, yWinTop, zOut), glassColor);
        }
    };

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zOut = s * (halfWidth + 0.008f);
        float zIn  = s * (halfWidth - 0.015f);
        float zMin = std::min(zIn, zOut);
        float zMax = std::max(zIn, zOut);
        bool skipDoor = (side > 0) && (rightDoorSkipX1 > rightDoorSkipX0 + 0.05f);

        auto emitClippedSpan = [&](float a, float b) {
            if (!skipDoor) {
                emitRibbonQuad(a, b, zOut, side > 0);
                return;
            }
            if (b <= rightDoorSkipX0 || a >= rightDoorSkipX1) {
                emitRibbonQuad(a, b, zOut, side > 0);
                return;
            }
            if (a < rightDoorSkipX0) {
                emitRibbonQuad(a, rightDoorSkipX0, zOut, side > 0);
            }
            if (b > rightDoorSkipX1) {
                emitRibbonQuad(rightDoorSkipX1, b, zOut, side > 0);
            }
        };

        if (!emitMullionsOnly) {
            if (swoop0 > xApillar + 0.02f) {
                emitClippedSpan(xApillar, swoop0);
            }
            emitClippedSpan(swoop0, xMid);
            emitClippedSpan(xMid, xDropEnd);
            emitClippedSpan(xDropEnd, xRear);
        }

        if (!emitGlassOnly) {
            if (swoop0 > xApillar + 0.15f) {
                if (!skipDoor || swoop0 < rightDoorSkipX0 - 0.02f || swoop0 > rightDoorSkipX1 + 0.02f) {
                    RunningGear::AppendBox(
                        mesh,
                        Vec3(swoop0 - 0.022f, yFrontLowBot, zMin),
                        Vec3(swoop0 + 0.022f, yWinTop,      zMax),
                        solidTrim
                    );
                }
            }
            int panes = std::max(2, windowPanes);
            float span = xRear - xDropEnd;
            for (int i = 1; i < panes; ++i) {
                float sx = xDropEnd + (span * static_cast<float>(i)) / static_cast<float>(panes);
                if (skipDoor && sx >= rightDoorSkipX0 - 0.02f && sx <= rightDoorSkipX1 + 0.02f) continue;
                float mBot = evalBotY(sx);
                RunningGear::AppendBox(
                    mesh,
                    Vec3(sx - 0.018f, mBot, zMin),
                    Vec3(sx + 0.018f, yWinTop, zMax),
                    solidTrim
                );
            }
            auto emitTrimBar = [&](float a, float b, float yb, float yt) {
                if (b <= a + 0.02f) return;
                if (!skipDoor || b <= rightDoorSkipX0 || a >= rightDoorSkipX1) {
                    RunningGear::AppendBox(mesh, Vec3(a, yb, zMin), Vec3(b, yt, zMax), solidTrim);
                    return;
                }
                if (rightDoorSkipX0 > a + 0.02f) RunningGear::AppendBox(mesh, Vec3(a, yb, zMin), Vec3(rightDoorSkipX0, yt, zMax), solidTrim);
                if (b > rightDoorSkipX1 + 0.02f) RunningGear::AppendBox(mesh, Vec3(rightDoorSkipX1, yb, zMin), Vec3(b, yt, zMax), solidTrim);
            };
            if (swoop0 > xApillar + 0.02f) {
                emitTrimBar(xApillar, swoop0, yFrontLowBot - 0.025f, yFrontLowBot + 0.01f);
            }
            if (std::abs(endMainY - yMainBeltBot) < 0.02f) {
                emitTrimBar(xDropEnd, xRear, yMainBeltBot - 0.025f, yMainBeltBot + 0.01f);
            } else {
                float zTrim = s * (halfWidth + 0.011f);
                if (side > 0) {
                    AppendQuad(mesh, Vec3(xDropEnd, yMainBeltBot - 0.025f, zTrim), Vec3(xRear, endMainY - 0.025f, zTrim), Vec3(xRear, endMainY + 0.01f, zTrim), Vec3(xDropEnd, yMainBeltBot + 0.01f, zTrim), solidTrim);
                } else {
                    AppendQuad(mesh, Vec3(xRear, endMainY - 0.025f, zTrim), Vec3(xDropEnd, yMainBeltBot - 0.025f, zTrim), Vec3(xDropEnd, yMainBeltBot + 0.01f, zTrim), Vec3(xRear, endMainY + 0.01f, zTrim), solidTrim);
                }
            }
            emitTrimBar(xApillar, xRear, yWinTop - 0.015f, yWinTop + 0.025f);
        }
    }
}

void CabComponents::AppendSwoopWaistWallSide(
    Mesh* mesh,
    float xApillar, float xDropEnd,
    float ySkirtTop, float yFrontLowBot, float yMainBeltBot,
    float halfWidth,
    float thickness,
    const Vec4& paintColor,
    const Vec4& trimColor,
    bool skipRightSide
) {
    if (!mesh || xDropEnd <= xApillar) return;
    float xMid = xApillar + (xDropEnd - xApillar) * 0.55f;
    float yMidBot = yFrontLowBot + (yMainBeltBot - yFrontLowBot) * 0.42f;

    for (int side : {-1, 1}) {
        if (side > 0 && skipRightSide) continue;
        float s = static_cast<float>(side);
        float zOut = s * halfWidth;
        float zIn  = s * (halfWidth - thickness);
        float zMin = std::min(zIn, zOut);
        float zMax = std::max(zIn, zOut);

        // Front low waist wall + sloped swoop ramp wall + interior side backing
        if (side > 0) {
            AppendQuad(mesh, Vec3(xApillar, ySkirtTop, zOut), Vec3(xMid, ySkirtTop, zOut), Vec3(xMid, yMidBot, zOut), Vec3(xApillar, yFrontLowBot, zOut), paintColor);
            AppendQuad(mesh, Vec3(xMid, ySkirtTop, zOut), Vec3(xDropEnd, ySkirtTop, zOut), Vec3(xDropEnd, yMainBeltBot, zOut), Vec3(xMid, yMidBot, zOut), paintColor);
            AppendQuad(mesh, Vec3(xMid, ySkirtTop, zIn), Vec3(xApillar, ySkirtTop, zIn), Vec3(xApillar, yFrontLowBot, zIn), Vec3(xMid, yMidBot, zIn), trimColor);
            AppendQuad(mesh, Vec3(xDropEnd, ySkirtTop, zIn), Vec3(xMid, ySkirtTop, zIn), Vec3(xMid, yMidBot, zIn), Vec3(xDropEnd, yMainBeltBot, zIn), trimColor);
        } else {
            AppendQuad(mesh, Vec3(xMid, ySkirtTop, zOut), Vec3(xApillar, ySkirtTop, zOut), Vec3(xApillar, yFrontLowBot, zOut), Vec3(xMid, yMidBot, zOut), paintColor);
            AppendQuad(mesh, Vec3(xDropEnd, ySkirtTop, zOut), Vec3(xMid, ySkirtTop, zOut), Vec3(xMid, yMidBot, zOut), Vec3(xDropEnd, yMainBeltBot, zOut), paintColor);
            AppendQuad(mesh, Vec3(xApillar, ySkirtTop, zIn), Vec3(xMid, ySkirtTop, zIn), Vec3(xMid, yMidBot, zIn), Vec3(xApillar, yFrontLowBot, zIn), trimColor);
            AppendQuad(mesh, Vec3(xMid, ySkirtTop, zIn), Vec3(xDropEnd, ySkirtTop, zIn), Vec3(xDropEnd, yMainBeltBot, zIn), Vec3(xMid, yMidBot, zIn), trimColor);
        }
        // Top sloped sill cap
        AppendQuad(mesh, Vec3(xApillar, yFrontLowBot, zMin), Vec3(xMid, yMidBot, zMin), Vec3(xMid, yMidBot, zMax), Vec3(xApillar, yFrontLowBot, zMax), trimColor);
        AppendQuad(mesh, Vec3(xMid, yMidBot, zMin), Vec3(xDropEnd, yMainBeltBot, zMin), Vec3(xDropEnd, yMainBeltBot, zMax), Vec3(xMid, yMidBot, zMax), trimColor);
    }
}

void CabComponents::AppendAnimatedDoor(
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
) {
    if (!mesh || doorBackX <= doorFrontX + 0.1f) return;
    float clampOpen = std::clamp(openRatio, 0.0f, 1.0f);
    float sign = isRightSide ? 1.0f : -1.0f;
    float spanY = topY - botY;
    float winBot = (spanY > 2.30f) ? (botY + spanY * 0.30f) : (botY + spanY * 0.44f);
    float winTop = topY - 0.07f;
    float stileW = 0.065f;

    float shiftX = 0.0f;
    float shiftZ = 0.0f;
    float angleRad = 0.0f;
    float pivotX = doorFrontX;
    float pivotZ = sideZ;

    if (isSliding) {
        shiftX = clampOpen * (doorBackX - doorFrontX) * 0.78f;
        shiftZ = sign * (0.015f + clampOpen * 0.055f);
    } else {
        angleRad = (isRightSide ? 1.0f : -1.0f) * clampOpen * 1.24f; // Swing outward ~71 deg
    }

    float zA = sideZ + shiftZ - sign * 0.030f;
    float zB = sideZ + shiftZ + sign * 0.018f;
    float zMin = std::min(zA, zB);
    float zMax = std::max(zA, zB);
    float x0 = doorFrontX + shiftX;
    float x1 = doorBackX + shiftX;

    if (!emitGlassPass) {
        // Lower painted door panel (up to window sill)
        AppendBoxRotY(mesh, Vec3(x0, botY, zMin), Vec3(x1, winBot, zMax), pivotX, pivotZ, angleRad, paintColor);
        // Front hinge stile & rear latch stile framing the door window cutout
        AppendBoxRotY(mesh, Vec3(x0, winBot, zMin), Vec3(x0 + stileW, topY, zMax), pivotX, pivotZ, angleRad, trimColor);
        AppendBoxRotY(mesh, Vec3(x1 - stileW, winBot, zMin), Vec3(x1, topY, zMax), pivotX, pivotZ, angleRad, trimColor);
        // Top header rail
        AppendBoxRotY(mesh, Vec3(x0 + stileW, winTop, zMin), Vec3(x1 - stileW, topY, zMax), pivotX, pivotZ, angleRad, trimColor);
        // Dark rubber beltline & vertical edge seal
        AppendBoxRotY(mesh, Vec3(x0, winBot - 0.025f, zMin - 0.004f), Vec3(x1, winBot + 0.01f, zMax + 0.004f), pivotX, pivotZ, angleRad, trimColor);
        // Chrome exterior handle
        float hZ0 = isRightSide ? zMax : zMin - 0.024f;
        float hZ1 = isRightSide ? zMax + 0.024f : zMin;
        AppendBoxRotY(mesh, Vec3(x1 - 0.18f, winBot - 0.22f, hZ0), Vec3(x1 - 0.07f, winBot - 0.08f, hZ1), pivotX, pivotZ, angleRad, Vec4(0.84f, 0.87f, 0.92f, 1.0f));
    } else {
        // Translucent door window glass inside hollow frame
        float gzMin = (zMin + zMax) * 0.5f - 0.005f;
        float gzMax = (zMin + zMax) * 0.5f + 0.005f;
        AppendBoxRotY(mesh, Vec3(x0 + stileW, winBot, gzMin), Vec3(x1 - stileW, winTop, gzMax), pivotX, pivotZ, angleRad, glassColor);
    }
}

void CabComponents::AppendSeat(
    Mesh* mesh,
    const Vec3& floorPos,
    float facingSignX,
    float widthZ,
    float seatH,
    const Vec4& cushionColor,
    const Vec4& headrestColor,
    const Vec4& frameColor
) {
    if (!mesh) return;
    float hwZ = widthZ * 0.5f;
    float cushY = floorPos.y + seatH * 0.42f;
    float backX = floorPos.x - facingSignX * 0.16f;
    float frontX = floorPos.x + facingSignX * 0.20f;
    float xMin = std::min(backX, frontX);
    float xMax = std::max(backX, frontX);

    // Pedestal base
    RunningGear::AppendBox(mesh, Vec3(floorPos.x - 0.06f, floorPos.y, floorPos.z - 0.06f), Vec3(floorPos.x + 0.06f, cushY, floorPos.z + 0.06f), frameColor);
    // Horizontal seat cushion
    RunningGear::AppendBox(mesh, Vec3(xMin, cushY, floorPos.z - hwZ), Vec3(xMax, cushY + 0.10f, floorPos.z + hwZ), cushionColor);
    // Backrest + headrest
    float bx0 = backX - 0.045f;
    float bx1 = backX + 0.045f;
    float topBackY = floorPos.y + seatH * 0.86f;
    float topHeadY = floorPos.y + seatH;
    RunningGear::AppendBox(mesh, Vec3(bx0, cushY + 0.08f, floorPos.z - hwZ * 0.94f), Vec3(bx1, topBackY, floorPos.z + hwZ * 0.94f), cushionColor);
    RunningGear::AppendBox(mesh, Vec3(bx0, topBackY, floorPos.z - hwZ * 0.65f), Vec3(bx1, topHeadY, floorPos.z + hwZ * 0.65f), headrestColor);
}

void CabComponents::AppendDriverCockpit(
    Mesh* mesh,
    float dashFrontX,
    float driverZ,
    float floorY,
    const Vec4& dashColor,
    const Vec4& seatColor,
    const Vec4& trimColor
) {
    if (!mesh) return;
    Vec4 screenCyan(0.18f, 0.72f, 0.96f, 1.0f);
    Vec4 rimCol(0.14f, 0.15f, 0.18f, 1.0f);

    // Full-width lower dashboard binnacle
    RunningGear::AppendBox(mesh, Vec3(dashFrontX, floorY, driverZ - 0.42f), Vec3(dashFrontX + 0.44f, floorY + 0.72f, driverZ + 0.48f), dashColor);
    // Illuminated digital instrument screen facing driver (+X)
    RunningGear::AppendBox(mesh, Vec3(dashFrontX + 0.43f, floorY + 0.56f, driverZ - 0.18f), Vec3(dashFrontX + 0.46f, floorY + 0.70f, driverZ + 0.14f), screenCyan);

    // Steering column & steering wheel ring
    float colX = dashFrontX + 0.52f;
    float colY = floorY + 0.76f;
    RunningGear::AppendBox(mesh, Vec3(dashFrontX + 0.36f, floorY + 0.52f, driverZ - 0.03f), Vec3(colX, colY, driverZ + 0.03f), trimColor);
    RunningGear::AppendBox(mesh, Vec3(colX - 0.03f, colY - 0.18f, driverZ - 0.18f), Vec3(colX + 0.03f, colY + 0.18f, driverZ + 0.18f), rimCol);

    // Air-ride driver captain seat
    AppendSeat(mesh, Vec3(dashFrontX + 0.96f, floorY, driverZ), -1.0f, 0.50f, 1.02f, seatColor, Vec4(0.84f, 0.86f, 0.90f, 1.0f), trimColor);
    // Modesty screen partition behind driver
    RunningGear::AppendBox(mesh, Vec3(dashFrontX + 1.32f, floorY, driverZ - 0.40f), Vec3(dashFrontX + 1.36f, floorY + 1.18f, driverZ + 0.32f), trimColor);
}

void CabComponents::AppendBusInterior(
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
) {
    if (!mesh || xRearCabin <= xFrontCabin + 1.2f) return;
    Vec4 ceilingCol(0.86f, 0.88f, 0.92f, 1.0f);
    Vec4 aisleCol(0.46f, 0.50f, 0.56f, 1.0f);
    Vec4 headrestCol(0.84f, 0.86f, 0.90f, 1.0f);
    Vec4 stairAmber(0.95f, 0.76f, 0.16f, 1.0f);
    Vec4 chromeCol(0.82f, 0.85f, 0.90f, 1.0f);
    Vec4 ledStrip(0.96f, 0.96f, 0.88f, 1.0f);

    float inHW = hw - 0.05f;
    // Cabin floor slab (skipping front entry stairwell cutout on +Z side)
    RunningGear::AppendBox(mesh, Vec3(xFrontCabin, floorY - 0.08f, -inHW), Vec3(xRearCabin, floorY, 0.32f), floorColor);
    if (doorX0 > xFrontCabin + 0.02f) {
        RunningGear::AppendBox(mesh, Vec3(xFrontCabin, floorY - 0.08f, 0.32f), Vec3(doorX0, floorY, inHW), floorColor);
    }
    RunningGear::AppendBox(mesh, Vec3(doorX1, floorY - 0.08f, 0.32f), Vec3(xRearCabin, floorY, inHW), floorColor);
    // Center aisle runner
    RunningGear::AppendBox(mesh, Vec3(xFrontCabin + 0.6f, floorY, -0.24f), Vec3(xRearCabin - 0.25f, floorY + 0.015f, 0.24f), aisleCol);

    // 3-step illuminated stairwell inside front (+Z) entry door
    int steps = 3;
    for (int s = 0; s < steps; ++s) {
        float t0 = static_cast<float>(s) / static_cast<float>(steps);
        float t1 = static_cast<float>(s + 1) / static_cast<float>(steps);
        float sy = doorStepBotY + (floorY - doorStepBotY) * t0;
        float sz1 = inHW - t0 * (inHW - 0.32f);
        float sz0 = inHW - t1 * (inHW - 0.32f);
        RunningGear::AppendBox(mesh, Vec3(doorX0, doorStepBotY - 0.04f, sz0), Vec3(doorX1, sy + 0.04f, sz1), floorColor);
        RunningGear::AppendBox(mesh, Vec3(doorX0, sy + 0.03f, sz1 - 0.035f), Vec3(doorX1, sy + 0.048f, sz1), stairAmber);
    }
    // Vertical chrome grab poles at doorway
    ChassisComponents::AppendClosedCylinderY(mesh, doorX0 + 0.08f, inHW - 0.14f, doorStepBotY + 0.05f, ceilingY, 0.022f, 10, chromeCol);
    ChassisComponents::AppendClosedCylinderY(mesh, doorX1 - 0.08f, 0.36f, floorY, ceilingY, 0.022f, 10, chromeCol);

    // Driver cockpit on left (-Z) side
    AppendDriverCockpit(mesh, xFrontCabin + 0.10f, -inHW * 0.56f, floorY, trimColor, seatColor, trimColor);

    // Passenger 2+2 seating rows along left (-Z) and right (+Z) sides
    float rowsStart = doorX1 + 0.36f;
    float rowsEnd = xRearCabin - 0.45f;
    int rows = std::max(3, seatRows);
    float pitch = (rowsEnd - rowsStart) / static_cast<float>(rows);
    for (int r = 0; r < rows; ++r) {
        float rx = rowsStart + (static_cast<float>(r) + 0.5f) * pitch;
        // Left pair (-Z)
        AppendSeat(mesh, Vec3(rx, floorY, -inHW + 0.25f), -1.0f, 0.42f, 0.94f, seatColor, headrestCol, trimColor);
        AppendSeat(mesh, Vec3(rx, floorY, -inHW + 0.70f), -1.0f, 0.42f, 0.94f, seatColor, headrestCol, trimColor);

        // Right pair (+Z), skipping second transit doorway if present
        bool inSecondDoor = dualSideDoors && (rx >= secondDoorX0 - 0.25f && rx <= secondDoorX1 + 0.25f);
        if (!inSecondDoor) {
            AppendSeat(mesh, Vec3(rx, floorY, inHW - 0.70f), -1.0f, 0.42f, 0.94f, seatColor, headrestCol, trimColor);
            AppendSeat(mesh, Vec3(rx, floorY, inHW - 0.25f), -1.0f, 0.42f, 0.94f, seatColor, headrestCol, trimColor);
        }
    }

    // Interior ceiling liner + overhead parcel racks + warm LED cove lighting (inset behind front roof dome)
    float xCeilFront = std::min(xRearCabin - 0.40f, xFrontCabin + 0.48f);
    float ceilHW = std::max(0.40f, inHW - 0.12f);
    RunningGear::AppendBox(mesh, Vec3(xCeilFront, ceilingY, -ceilHW), Vec3(xRearCabin, ceilingY + 0.04f, ceilHW), ceilingCol);
    RunningGear::AppendBox(mesh, Vec3(xCeilFront + 0.12f, ceilingY - 0.02f, -0.28f), Vec3(xRearCabin - 0.3f, ceilingY + 0.005f, -0.22f), ledStrip);
    RunningGear::AppendBox(mesh, Vec3(xCeilFront + 0.12f, ceilingY - 0.02f,  0.22f), Vec3(xRearCabin - 0.3f, ceilingY + 0.005f,  0.28f), ledStrip);
    // Overhead baggage shelves along windows
    RunningGear::AppendBox(mesh, Vec3(rowsStart - 0.1f, ceilingY - 0.34f, -inHW), Vec3(rowsEnd + 0.2f, ceilingY - 0.29f, -inHW + 0.46f), trimColor);
    RunningGear::AppendBox(mesh, Vec3(rowsStart - 0.1f, ceilingY - 0.34f,  inHW - 0.46f), Vec3(rowsEnd + 0.2f, ceilingY - 0.29f,  inHW), trimColor);

    // Interior rear bulkhead wall
    RunningGear::AppendBox(mesh, Vec3(xRearCabin - 0.06f, floorY, -inHW), Vec3(xRearCabin, ceilingY, inHW), ceilingCol);
}

void CabComponents::AppendTruckCabInterior(
    Mesh* mesh,
    float xDashFront, float xCabRear,
    float floorY, float ceilingY,
    float cabHW,
    bool hasSleeperBunk,
    const Vec4& dashColor,
    const Vec4& seatColor,
    const Vec4& accentColor
) {
    if (!mesh || xCabRear <= xDashFront + 0.8f) return;
    float inHW = cabHW - 0.06f;
    Vec4 ceilingCol(0.78f, 0.80f, 0.84f, 1.0f);
    Vec4 trimCol(0.14f, 0.15f, 0.18f, 1.0f);
    Vec4 screenCyan(0.18f, 0.72f, 0.96f, 1.0f);

    // Cab floor + center engine doghouse / console
    RunningGear::AppendBox(mesh, Vec3(xDashFront, floorY - 0.04f, -inHW), Vec3(xCabRear, floorY, inHW), trimCol);
    RunningGear::AppendBox(mesh, Vec3(xDashFront + 0.3f, floorY, -0.18f), Vec3(xDashFront + 1.05f, floorY + 0.22f, 0.18f), dashColor);

    // Full-width sculpted truck dashboard
    RunningGear::AppendBox(mesh, Vec3(xDashFront, floorY, -inHW), Vec3(xDashFront + 0.38f, floorY + 0.68f, inHW), dashColor);
    RunningGear::AppendBox(mesh, Vec3(xDashFront + 0.37f, floorY + 0.50f, -inHW * 0.56f - 0.14f), Vec3(xDashFront + 0.40f, floorY + 0.64f, -inHW * 0.56f + 0.14f), screenCyan);

    // Steering column & steering wheel on driver (-Z) side
    float colX = xDashFront + 0.48f;
    float colY = floorY + 0.72f;
    RunningGear::AppendBox(mesh, Vec3(xDashFront + 0.32f, floorY + 0.48f, -inHW * 0.56f - 0.03f), Vec3(colX, colY, -inHW * 0.56f + 0.03f), trimCol);
    RunningGear::AppendBox(mesh, Vec3(colX - 0.025f, colY - 0.17f, -inHW * 0.56f - 0.17f), Vec3(colX + 0.025f, colY + 0.17f, -inHW * 0.56f + 0.17f), trimCol);

    // Twin air-ride high-back seats
    float seatX = xDashFront + 0.92f;
    AppendSeat(mesh, Vec3(seatX, floorY, -inHW * 0.54f), -1.0f, 0.52f, 1.04f, seatColor, accentColor, trimCol);
    AppendSeat(mesh, Vec3(seatX, floorY,  inHW * 0.54f), -1.0f, 0.52f, 1.04f, seatColor, accentColor, trimCol);

    // Sleeper berth mattress behind seats if long sleeper cab
    if (hasSleeperBunk && (xCabRear - seatX) > 0.70f) {
        RunningGear::AppendBox(mesh, Vec3(seatX + 0.36f, floorY, -inHW), Vec3(xCabRear - 0.05f, floorY + 0.34f, inHW), dashColor);
        RunningGear::AppendBox(mesh, Vec3(seatX + 0.38f, floorY + 0.34f, -inHW + 0.04f), Vec3(xCabRear - 0.06f, floorY + 0.48f, inHW - 0.04f), Vec4(0.84f, 0.86f, 0.90f, 1.0f));
        RunningGear::AppendBox(mesh, Vec3(seatX + 0.44f, floorY + 0.48f, -inHW + 0.10f), Vec3(seatX + 0.78f, floorY + 0.56f, -inHW + 0.55f), accentColor);
    }

    // Interior cab ceiling & rear bulkhead
    RunningGear::AppendBox(mesh, Vec3(xDashFront + 0.1f, ceilingY, -inHW), Vec3(xCabRear, ceilingY + 0.04f, inHW), ceilingCol);
    RunningGear::AppendBox(mesh, Vec3(xCabRear - 0.05f, floorY, -inHW), Vec3(xCabRear, ceilingY, inHW), ceilingCol);
}

void CabComponents::AppendDoor(
    Mesh* mesh,
    float doorFrontX, float doorBackX,
    float botY, float topY,
    float sideZ, bool isRightSide,
    const Vec4& paintColor,
    const Vec4& glassColor
) {
    AppendAnimatedDoor(
        mesh, doorFrontX, doorBackX, botY, topY, sideZ, isRightSide,
        0.0f, false, false, paintColor, Vec4(0.10f, 0.11f, 0.13f, 1.0f), glassColor
    );
    AppendAnimatedDoor(
        mesh, doorFrontX, doorBackX, botY, topY, sideZ, isRightSide,
        0.0f, false, true, paintColor, Vec4(0.10f, 0.11f, 0.13f, 1.0f), glassColor
    );
}

void CabComponents::AppendWindshield(
    Mesh* mesh,
    float xBot, float xTop,
    float yBot, float yTop,
    float halfWidth,
    const Vec4& glassColor,
    const Vec4& rubberSealColor
) {
    AppendFrustumX(
        mesh,
        xBot - 0.015f, yBot - 0.035f, yTop + 0.035f, halfWidth + 0.035f,
        xTop + 0.045f, yBot - 0.035f, yTop + 0.035f, halfWidth + 0.035f,
        rubberSealColor
    );

    float hwGlass = std::max(0.2f, halfWidth - 0.03f);
    AppendFrustumX(
        mesh,
        xBot - 0.032f, yBot, yTop, hwGlass,
        xTop + 0.025f, yBot, yTop, hwGlass,
        glassColor
    );

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
    RunningGear::AppendBox(
        mesh,
        Vec3(faceX - 0.01f, botY, -halfWidth),
        Vec3(faceX + 0.05f, topY,  halfWidth),
        innerBackingColor
    );
    float b = 0.04f;
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
        float zOut = s * (cabHalfWidth + 0.21f);
        float zMin = std::min(zIn, zOut);
        float zMax = std::max(zIn, zOut);

        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.02f, mirrorCenterY + 0.18f, zMin), Vec3(anchorX + 0.02f, mirrorCenterY + 0.22f, zMax), armCol);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.02f, mirrorCenterY - 0.20f, zMin), Vec3(anchorX + 0.02f, mirrorCenterY - 0.16f, zMax), armCol);

        float mZ0 = s > 0 ? zOut - 0.03f : zOut - 0.09f;
        float mZ1 = s > 0 ? zOut + 0.09f : zOut + 0.03f;
        RunningGear::AppendBox(
            mesh,
            Vec3(anchorX - 0.055f, mirrorCenterY - 0.24f, mZ0),
            Vec3(anchorX + 0.045f, mirrorCenterY + 0.26f, mZ1),
            housingColor
        );
        RunningGear::AppendBox(
            mesh,
            Vec3(anchorX + 0.042f, mirrorCenterY - 0.21f, mZ0 + 0.015f),
            Vec3(anchorX + 0.058f, mirrorCenterY + 0.23f, mZ1 - 0.015f),
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
    AppendModernLedFascia(
        mesh, frontX, botY, topY, halfWidth, headlightsOn,
        bumperColor, Vec4(0.08f, 0.09f, 0.11f, 1.0f)
    );
}

void CabComponents::AppendAmberRoofMarkers(
    Mesh* mesh,
    float frontX,
    float roofY,
    float halfWidth,
    bool lightsOn
) {
    Vec4 col = lightsOn
        ? Vec4(0.98f, 0.72f, 0.18f, 1.0f)
        : Vec4(0.55f, 0.35f, 0.08f, 1.0f);
    float offsets[5] = {-halfWidth * 0.72f, -0.18f, 0.0f, 0.18f, halfWidth * 0.72f};
    for (float oz : offsets) {
        RunningGear::AppendBox(
            mesh,
            Vec3(frontX, roofY, oz - 0.045f),
            Vec3(frontX + 0.09f, roofY + 0.035f, oz + 0.045f),
            col
        );
    }
}

} // namespace sphere::vehicles::cab
