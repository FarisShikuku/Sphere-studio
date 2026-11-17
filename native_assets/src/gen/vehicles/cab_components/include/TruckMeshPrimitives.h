#pragma once
#include "VehicleDef.h"
#include "CabComponents.h"
#include "ChassisComponents.h"
#include "RunningGear.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace sphere::vehicles::trucks {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::cab::CabComponents;
using sphere::vehicles::cab::HullStationX;
using sphere::vehicles::chassis::ChassisComponents;

inline void PushTri(Mesh* mesh, const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec4& col) {
    uint32_t idx = static_cast<uint32_t>(mesh->vertices.size());
    Vertex v0; v0.position = p0; v0.color = col; v0.uv[0] = 0.0f; v0.uv[1] = 0.0f;
    Vertex v1; v1.position = p1; v1.color = col; v1.uv[0] = 1.0f; v1.uv[1] = 0.0f;
    Vertex v2; v2.position = p2; v2.color = col; v2.uv[0] = 1.0f; v2.uv[1] = 1.0f;
    mesh->vertices.push_back(v0);
    mesh->vertices.push_back(v1);
    mesh->vertices.push_back(v2);
    mesh->indices.push_back(idx);
    mesh->indices.push_back(idx + 1);
    mesh->indices.push_back(idx + 2);
}

inline void PushQuad(Mesh* mesh, const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const Vec4& col) {
    PushTri(mesh, a, b, c, col);
    PushTri(mesh, a, c, d, col);
}

// Smooth front nose X-coordinate curving backward across |z| so fascias, grilles & lamps follow cab curvature
inline float CurvedNoseXAtZ(float z, float centerX, float halfSpanZ, float edgeSweepX) {
    float nz = std::clamp(std::abs(z) / std::max(0.20f, halfSpanZ), 0.0f, 1.0f);
    // Smooth blend matching BuildStationRing dome + carved corner wrap
    return centerX + edgeSweepX * (0.35f * nz + 0.65f * nz * nz);
}

// Curved horizontal/chamfered 3D strip facing -X that conforms seamlessly to the curved cab nose & corners
inline void AppendCurvedPanelX(
    Mesh* mesh,
    float xCenterFront, float thicknessX,
    float yBotLeft, float yTopLeft,
    float yBotRight, float yTopRight,
    float zLeft, float zRight,
    float curveHalfW, float edgeSweepX,
    int segs, const Vec4& col,
    float cornerChamferY = 0.0f
) {
    if (!mesh || zRight <= zLeft + 1e-3f) return;
    int n = std::max(2, segs);
    for (int i = 0; i < n; ++i) {
        float t0 = static_cast<float>(i) / static_cast<float>(n);
        float t1 = static_cast<float>(i + 1) / static_cast<float>(n);
        float z0 = zLeft + (zRight - zLeft) * t0;
        float z1 = zLeft + (zRight - zLeft) * t1;
        float xF0 = CurvedNoseXAtZ(z0, xCenterFront, curveHalfW, edgeSweepX);
        float xF1 = CurvedNoseXAtZ(z1, xCenterFront, curveHalfW, edgeSweepX);
        float xB0 = xF0 + thicknessX;
        float xB1 = xF1 + thicknessX;

        float yb0 = yBotLeft + (yBotRight - yBotLeft) * t0;
        float yt0 = yTopLeft + (yTopRight - yTopLeft) * t0;
        float yb1 = yBotLeft + (yBotRight - yBotLeft) * t1;
        float yt1 = yTopLeft + (yTopRight - yTopLeft) * t1;

        if (cornerChamferY > 1e-4f) {
            if (i == 0) {
                yb0 = std::min(yt0 - 0.005f, yb0 + cornerChamferY);
                yt0 = std::max(yb0 + 0.005f, yt0 - cornerChamferY);
            }
            if (i == n - 1) {
                yb1 = std::min(yt1 - 0.005f, yb1 + cornerChamferY);
                yt1 = std::max(yb1 + 0.005f, yt1 - cornerChamferY);
            }
        }

        Vec3 fBL(xF0, yb0, z0), fBR(xF1, yb1, z1), fTR(xF1, yt1, z1), fTL(xF0, yt0, z0);
        Vec3 bBL(xB0, yb0, z0), bBR(xB1, yb1, z1), bTR(xB1, yt1, z1), bTL(xB0, yt0, z0);

        PushQuad(mesh, fBL, fBR, fTR, fTL, col); // -X outward
        PushQuad(mesh, bBR, bBL, bTL, bTR, col); // +X inner
        PushQuad(mesh, fTL, fTR, bTR, bTL, col); // +Y top
        PushQuad(mesh, bBL, bBR, fBR, fBL, col); // -Y bottom
        if (i == 0)     PushQuad(mesh, bBL, fBL, fTL, bTL, col); // -Z cap
        if (i == n - 1) PushQuad(mesh, fBR, bBR, bTR, fTR, col); // +Z cap
    }
}

// Solid disk along X facing -X (outward normal) for brand circular badges & circular headlamps
inline void AppendFrontDiscX(
    Mesh* mesh,
    float faceX, float cy, float cz,
    float radius, float depthX,
    int segs, const Vec4& faceCol, const Vec4& rimCol
) {
    float xOuter = faceX - depthX;
    float xInner = faceX + 0.006f;
    Vec3 cOuter(xOuter, cy, cz);
    for (int i = 0; i < segs; ++i) {
        float a0 = (static_cast<float>(i) / static_cast<float>(segs)) * 6.2831853f;
        float a1 = (static_cast<float>(i + 1) / static_cast<float>(segs)) * 6.2831853f;
        Vec3 o0(xOuter, cy + radius * std::sin(a0), cz + radius * std::cos(a0));
        Vec3 o1(xOuter, cy + radius * std::sin(a1), cz + radius * std::cos(a1));
        Vec3 i0(xInner, cy + radius * std::sin(a0), cz + radius * std::cos(a0));
        Vec3 i1(xInner, cy + radius * std::sin(a1), cz + radius * std::cos(a1));
        PushTri(mesh, cOuter, o0, o1, faceCol);
        PushQuad(mesh, o0, i0, i1, o1, rimCol);
    }
}

// Recessed round projector headlamp pod curved to corner X facing -X (dark bezel + proud lens core)
inline void AppendCircularLampPodX(
    Mesh* mesh,
    float faceX, float cy, float cz,
    float outerR, float lensR,
    int segs, const Vec4& bezelCol, const Vec4& ringCol, const Vec4& lensCol
) {
    AppendFrontDiscX(mesh, faceX + 0.008f, cy, cz, outerR, 0.014f, segs, bezelCol, bezelCol);
    AppendFrontDiscX(mesh, faceX - 0.002f, cy, cz, lensR + 0.010f, 0.012f, segs, ringCol, ringCol);
    AppendFrontDiscX(mesh, faceX - 0.010f, cy, cz, lensR, 0.012f, segs, lensCol, ringCol);
}

// Mercedes-Benz Three-Pointed Star medallion facing -X
inline void AppendMercedesStarX(
    Mesh* mesh,
    float faceX, float cy, float cz,
    float radius,
    const Vec4& discCol, const Vec4& ringCol, const Vec4& starCol
) {
    AppendFrontDiscX(mesh, faceX + 0.008f, cy, cz, radius, 0.018f, 24, discCol, ringCol);
    AppendFrontDiscX(mesh, faceX - 0.004f, cy, cz, radius * 0.90f, 0.010f, 24, discCol, ringCol);
    float xFront = faceX - 0.020f;
    float xBack  = faceX - 0.004f;
    float innerR = radius * 0.16f;
    float tipR   = radius * 0.84f;
    const float kAngles[3] = { 1.5707963f, 1.5707963f + 2.0943951f, 1.5707963f + 4.1887902f };
    for (int k = 0; k < 3; ++k) {
        float a = kAngles[k];
        float aL = a + 1.0471976f;
        float aR = a - 1.0471976f;
        Vec3 cF(xFront - 0.005f, cy, cz);
        Vec3 tipF(xFront, cy + tipR * std::sin(a), cz + tipR * std::cos(a));
        Vec3 pLF(xFront, cy + innerR * std::sin(aL), cz + innerR * std::cos(aL));
        Vec3 pRF(xFront, cy + innerR * std::sin(aR), cz + innerR * std::cos(aR));
        PushTri(mesh, cF, pRF, tipF, starCol);
        PushTri(mesh, cF, tipF, pLF, starCol);
        Vec3 tipB(xBack, tipF.y, tipF.z);
        Vec3 pLB(xBack, pLF.y, pLF.z);
        Vec3 pRB(xBack, pRF.y, pRF.z);
        PushQuad(mesh, pRF, pRB, tipB, tipF, starCol);
        PushQuad(mesh, tipF, tipB, pLB, pLF, starCol);
    }
}

// Smooth slanted 3D ribbon along front face -X curving across Z
inline void AppendSlantedRibbonX(
    Mesh* mesh,
    float faceXCenter, float depthX,
    float y0, float z0,
    float y1, float z1,
    float halfW,
    const Vec4& col,
    float curveHalfW = 1.22f,
    float edgeSweepX = 0.09f
) {
    const int segs = 4;
    float dy = y1 - y0;
    float dz = z1 - z0;
    float len = std::sqrt(dy * dy + dz * dz);
    if (len < 1e-4f) return;
    float ny = -dz / len * halfW;
    float nz =  dy / len * halfW;

    for (int i = 0; i < segs; ++i) {
        float t0 = static_cast<float>(i) / static_cast<float>(segs);
        float t1 = static_cast<float>(i + 1) / static_cast<float>(segs);
        float ya = y0 + dy * t0, za = z0 + dz * t0;
        float yb = y0 + dy * t1, zb = z0 + dz * t1;
        float xFa = CurvedNoseXAtZ(za, faceXCenter, curveHalfW, edgeSweepX) - depthX;
        float xFb = CurvedNoseXAtZ(zb, faceXCenter, curveHalfW, edgeSweepX) - depthX;
        float xBa = xFa + depthX + 0.010f;
        float xBb = xFb + depthX + 0.010f;

        Vec3 f0A(xFa, ya - ny, za - nz), f0B(xFa, ya + ny, za + nz);
        Vec3 f1B(xFb, yb + ny, zb + nz), f1A(xFb, yb - ny, zb - nz);
        Vec3 b0A(xBa, ya - ny, za - nz), b0B(xBa, ya + ny, za + nz);
        Vec3 b1B(xBb, yb + ny, zb + nz), b1A(xBb, yb - ny, zb - nz);

        Vec3 e1(f0B.x - f0A.x, f0B.y - f0A.y, f0B.z - f0A.z);
        Vec3 e2(f1B.x - f0A.x, f1B.y - f0A.y, f1B.z - f0A.z);
        float nx = e1.y * e2.z - e1.z * e2.y;
        if (nx <= 0.0f) {
            PushQuad(mesh, f0A, f0B, f1B, f1A, col);
            PushQuad(mesh, b1A, b1B, b0B, b0A, col);
            PushQuad(mesh, f0A, f1A, b1A, b0A, col);
            PushQuad(mesh, f1B, f0B, b0B, b1B, col);
            if (i == 0)        PushQuad(mesh, f0B, f0A, b0A, b0B, col);
            if (i == segs - 1) PushQuad(mesh, f1A, f1B, b1B, b1A, col);
        } else {
            PushQuad(mesh, f0B, f0A, f1A, f1B, col);
            PushQuad(mesh, b1B, b1A, b0A, b0B, col);
            PushQuad(mesh, f0B, f1B, b1B, b0B, col);
            PushQuad(mesh, f1A, f0A, b0A, b1A, col);
            if (i == 0)        PushQuad(mesh, f0A, f0B, b0B, b0A, col);
            if (i == segs - 1) PushQuad(mesh, f1B, f1A, b1A, b1B, col);
        }
    }
}

// Angled 3D side stripe / decal / trim strip along +Z / -Z cab flank
inline void AppendSideRibbonZ(
    Mesh* mesh,
    float sideZ, float thickness, bool isRightSide,
    float x0, float y0,
    float x1, float y1,
    float halfH,
    const Vec4& col
) {
    float zOut = isRightSide ? (sideZ + thickness) : (sideZ - thickness);
    float zIn  = isRightSide ? (sideZ - 0.005f)    : (sideZ + 0.005f);
    Vec3 p0B(x0, y0 - halfH, zOut), p0T(x0, y0 + halfH, zOut);
    Vec3 p1B(x1, y1 - halfH, zOut), p1T(x1, y1 + halfH, zOut);
    Vec3 q0B(x0, y0 - halfH, zIn),  q0T(x0, y0 + halfH, zIn);
    Vec3 q1B(x1, y1 - halfH, zIn),  q1T(x1, y1 + halfH, zIn);

    if (isRightSide) {
        PushQuad(mesh, p0B, p1B, p1T, p0T, col);
        PushQuad(mesh, p0T, p1T, q1T, q0T, col);
        PushQuad(mesh, q0B, q1B, p1B, p0B, col);
        PushQuad(mesh, q0B, p0B, p0T, q0T, col);
        PushQuad(mesh, p1B, q1B, q1T, p1T, col);
    } else {
        PushQuad(mesh, p1B, p0B, p0T, p1T, col);
        PushQuad(mesh, q0T, q1T, p1T, p0T, col);
        PushQuad(mesh, p0B, p1B, q1B, q0B, col);
        PushQuad(mesh, p0B, q0B, q0T, p0T, col);
        PushQuad(mesh, q1B, p1B, p1T, q1T, col);
    }
}

// Angled box rotated around Z in X-Y plane (tipper bodies)
inline void AppendTiltedBoxZ(
    Mesh* mesh,
    const Vec3& minP, const Vec3& maxP,
    float pivotX, float pivotY,
    float tiltAngleRad,
    const Vec4& col
) {
    float c = std::cos(tiltAngleRad);
    float s = std::sin(tiltAngleRad);
    auto xf = [&](float x, float y, float z) -> Vec3 {
        float dx = x - pivotX;
        float dy = y - pivotY;
        return Vec3(pivotX + dx * c - dy * s, pivotY + dx * s + dy * c, z);
    };
    Vec3 corners[8] = {
        xf(minP.x, minP.y, maxP.z), xf(maxP.x, minP.y, maxP.z),
        xf(maxP.x, maxP.y, maxP.z), xf(minP.x, maxP.y, maxP.z),
        xf(minP.x, minP.y, minP.z), xf(maxP.x, minP.y, minP.z),
        xf(maxP.x, maxP.y, minP.z), xf(minP.x, maxP.y, minP.z)
    };
    const int faces[6][4] = {
        {0, 1, 2, 3}, {5, 4, 7, 6}, {3, 2, 6, 7},
        {4, 5, 1, 0}, {1, 5, 6, 2}, {4, 0, 3, 7}
    };
    for (int f = 0; f < 6; ++f) {
        PushQuad(mesh, corners[faces[f][0]], corners[faces[f][1]], corners[faces[f][2]], corners[faces[f][3]], col);
    }
}

// Smoothly carved lower cab nose & corner wrap (zero boxy corners!)
inline void AppendCarvedLowerNose(
    Mesh* mesh,
    float xNoseTip, float cabBackNoseX,
    float cabBaseY, float windBotY, float cabHW,
    const Vec4& paintColor
) {
    std::vector<HullStationX> nose = {
        {xNoseTip + 0.10f, cabBaseY - 0.04f, windBotY - 0.06f, cabHW - 0.22f, 0.16f, 0.14f, 0.02f, 0.08f},
        {xNoseTip + 0.20f, cabBaseY - 0.12f, windBotY - 0.02f, cabHW - 0.08f, 0.14f, 0.12f, 0.02f, 0.05f},
        {xNoseTip + 0.34f, cabBaseY - 0.14f, windBotY,         cabHW - 0.01f, 0.12f, 0.10f, 0.02f, 0.02f},
        {cabBackNoseX,     cabBaseY - 0.14f, windBotY,         cabHW,         0.12f, 0.10f, 0.02f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh, nose, paintColor, 6);
}

// Smooth curved cab roof brow + rounded crown + carved rear sleeper pillars (NO boxy A-pillars or brick sleepers!)
inline void AppendCarvedRoofAndSleeper(
    Mesh* mesh,
    float xFront, float cabBackX,
    float doorX1, float cabClearY,
    float windBotY, float windTopY, float cabTopY, float cabHW,
    const Vec4& paintColor, const Vec4& pillarColor
) {
    // Smooth carved roof canopy arching gently over panoramic windshield
    float roofFrontX = xFront + 0.34f;
    float rTop = std::clamp((cabTopY - windTopY) * 0.45f, 0.14f, 0.28f);
    std::vector<HullStationX> roof = {
        {roofFrontX,         windTopY - 0.02f, cabTopY - 0.16f, cabHW - 0.14f, 0.04f, rTop,         0.06f, 0.08f},
        {roofFrontX + 0.22f, windTopY - 0.02f, cabTopY - 0.06f, cabHW - 0.03f, 0.04f, rTop + 0.02f, 0.07f, 0.04f},
        {roofFrontX + 0.72f, windTopY - 0.02f, cabTopY - 0.02f, cabHW,         0.04f, rTop + 0.04f, 0.08f, 0.01f},
        {cabBackX,           windTopY - 0.02f, cabTopY - 0.02f, cabHW,         0.04f, rTop + 0.04f, 0.08f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh, roof, paintColor, 6);

    // Sleek slanted curved A-pillars wrapping around windshield side edges (flush, curved outer corners!)
    const int pSegs = 5;
    float wFrontX = xFront + 0.14f;
    float wBrowX  = xFront + 0.34f;
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zInner = s * (cabHW - 0.15f);
        float zOuter = s * cabHW;
        bool posSide = (side > 0);
        for (int i = 0; i < pSegs; ++i) {
            float t0 = static_cast<float>(i) / static_cast<float>(pSegs);
            float t1 = static_cast<float>(i + 1) / static_cast<float>(pSegs);
            float y0 = (windBotY - 0.03f) + ((windTopY + 0.03f) - (windBotY - 0.03f)) * t0;
            float y1 = (windBotY - 0.03f) + ((windTopY + 0.03f) - (windBotY - 0.03f)) * t1;
            float xBase0 = wFrontX + (wBrowX - wFrontX) * t0;
            float xBase1 = wFrontX + (wBrowX - wFrontX) * t1;
            float xFin0 = xBase0 + 0.015f, xFin1 = xBase1 + 0.015f;
            float xFout0 = xBase0 + 0.075f, xFout1 = xBase1 + 0.075f;
            float xB0 = xBase0 + 0.24f, xB1 = xBase1 + 0.24f;
            float zA = std::min(zInner, zOuter), zB = std::max(zInner, zOuter);
            float xFL0 = posSide ? xFin0 : xFout0, xFR0 = posSide ? xFout0 : xFin0;
            float xFL1 = posSide ? xFin1 : xFout1, xFR1 = posSide ? xFout1 : xFin1;
            Vec3 f0(xFL0, y0, zA), f1(xFR0, y0, zB), f2(xFR1, y1, zB), f3(xFL1, y1, zA);
            Vec3 b0(xB0,  y0, zA), b1(xB0,  y0, zB), b2(xB1,  y1, zB), b3(xB1,  y1, zA);
            PushQuad(mesh, f0, f1, f2, f3, pillarColor);
            PushQuad(mesh, b1, b0, b3, b2, pillarColor);
            PushQuad(mesh, b0, f0, f3, b3, pillarColor);
            PushQuad(mesh, f1, b1, b2, f2, pillarColor);
        }
    }

    // Carved rounded rear sleeper wall [doorX1 .. cabBackX] (soft rounded rear corners)
    if (cabBackX > doorX1 + 0.15f) {
        std::vector<HullStationX> sleeper = {
            {doorX1,          cabClearY, windTopY + 0.04f, cabHW,         0.12f, 0.12f, 0.0f, 0.0f},
            {cabBackX - 0.14f, cabClearY, windTopY + 0.04f, cabHW,         0.12f, 0.14f, 0.0f, 0.0f},
            {cabBackX,        cabClearY + 0.04f, windTopY + 0.04f, cabHW - 0.08f, 0.14f, 0.18f, 0.0f, 0.0f},
        };
        CabComponents::AppendSculptedHullX(mesh, sleeper, paintColor, 5);
    }
}

// Sculpted curved front cab sunvisor brow wrapping around A-pillar corners (tapered outer wingtips!)
inline void AppendSunvisorBrow(
    Mesh* mesh,
    float xFrontBrow, float xBackBrow,
    float yBot, float yTop, float hw,
    const Vec4& visorCol, const Vec4& accentBarCol, bool withCenterTrim = true
) {
    const int n = 8;
    float edgeSweep = 0.09f;
    float spanH = yTop - yBot;
    for (int i = 0; i < n; ++i) {
        float t0 = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(n));
        float t1 = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(n));
        float z0 = t0 * (hw - 0.03f);
        float z1 = t1 * (hw - 0.03f);
        float s0 = edgeSweep * (0.30f * std::abs(t0) + 0.70f * t0 * t0);
        float s1 = edgeSweep * (0.30f * std::abs(t1) + 0.70f * t1 * t1);

        // Taper visor height slightly at outer tips so corners look sleek and sculpted
        float taper0 = 1.0f - 0.28f * t0 * t0;
        float taper1 = 1.0f - 0.28f * t1 * t1;
        float yb0 = yTop - spanH * taper0;
        float yb1 = yTop - spanH * taper1;
        float xf0 = xFrontBrow + s0 - 0.04f;
        float xf1 = xFrontBrow + s1 - 0.04f;
        float xb0 = std::max(xf0 + 0.08f, xBackBrow + s0 * 0.4f);
        float xb1 = std::max(xf1 + 0.08f, xBackBrow + s1 * 0.4f);

        Vec3 fBL(xf0, yb0, z0), fBR(xf1, yb1, z1), fTR(xf1, yTop, z1), fTL(xf0, yTop, z0);
        Vec3 bBL(xb0, yb0 + 0.03f, z0), bBR(xb1, yb1 + 0.03f, z1), bTR(xb1, yTop + 0.02f, z1), bTL(xb0, yTop + 0.02f, z0);

        PushQuad(mesh, fBL, fBR, fTR, fTL, visorCol);
        PushQuad(mesh, bBR, bBL, bTL, bTR, visorCol);
        PushQuad(mesh, fTL, fTR, bTR, bTL, visorCol);
        PushQuad(mesh, bBL, bBR, fBR, fBL, visorCol);
        if (i == 0)     PushQuad(mesh, bBL, fBL, fTL, bTL, visorCol);
        if (i == n - 1) PushQuad(mesh, fBR, bBR, bTR, fTR, visorCol);
    }
    if (withCenterTrim) {
        AppendCurvedPanelX(mesh, xFrontBrow - 0.048f, 0.018f, (yBot + yTop) * 0.5f - 0.022f, (yBot + yTop) * 0.5f + 0.022f,
                           (yBot + yTop) * 0.5f - 0.022f, (yBot + yTop) * 0.5f + 0.022f, -0.44f, 0.44f, hw - 0.03f, edgeSweep, 4, accentBarCol, 0.008f);
    }
}

// Curved beveled trapezoidal grille surround + recessed dark backing plate facing -X
inline void AppendTrapezoidGrillePocketX(
    Mesh* mesh,
    float xCenterFront, float xCenterBack,
    float yBot, float yTop,
    float hwBot, float hwTop,
    float rimThick,
    const Vec4& bezelCol, const Vec4& backingCol,
    float curveHalfW = 1.22f, float edgeSweepX = 0.09f
) {
    float depth = std::max(0.02f, xCenterBack - xCenterFront);
    // Curved dark backing plate across 6 lateral strips
    const int n = 6;
    for (int i = 0; i < n; ++i) {
        float t0 = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(n));
        float t1 = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(n));
        float zB0 = t0 * hwBot, zB1 = t1 * hwBot;
        float zT0 = t0 * hwTop, zT1 = t1 * hwTop;
        float xB0 = CurvedNoseXAtZ(zB0, xCenterBack, curveHalfW, edgeSweepX);
        float xB1 = CurvedNoseXAtZ(zB1, xCenterBack, curveHalfW, edgeSweepX);
        float xT0 = CurvedNoseXAtZ(zT0, xCenterBack, curveHalfW, edgeSweepX);
        float xT1 = CurvedNoseXAtZ(zT1, xCenterBack, curveHalfW, edgeSweepX);
        PushQuad(mesh, Vec3(xB0, yBot, zB0), Vec3(xB1, yBot, zB1), Vec3(xT1, yTop, zT1), Vec3(xT0, yTop, zT0), backingCol);
    }
    // Curved top & bottom rails + slanted side cheeks
    AppendCurvedPanelX(mesh, xCenterFront, depth, yTop - rimThick, yTop + 0.005f, yTop - rimThick, yTop + 0.005f, -hwTop, hwTop, curveHalfW, edgeSweepX, 6, bezelCol, 0.006f);
    AppendCurvedPanelX(mesh, xCenterFront, depth, yBot - 0.005f, yBot + rimThick, yBot - 0.005f, yBot + rimThick, -hwBot, hwBot, curveHalfW, edgeSweepX, 6, bezelCol, 0.006f);
    AppendSlantedRibbonX(mesh, xCenterBack, depth, yBot, -hwBot, yTop, -hwTop, rimThick * 0.65f, bezelCol, curveHalfW, edgeSweepX);
    AppendSlantedRibbonX(mesh, xCenterBack, depth, yBot,  hwBot, yTop,  hwTop, rimThick * 0.65f, bezelCol, curveHalfW, edgeSweepX);
}

// Curved tapered V-shaped front grille with proud curved slats in front of curved backing plate
inline void AppendVGrilleSlats(
    Mesh* mesh,
    float xCenterFront, float yBot, float yTop,
    float halfWidthBot, float halfWidthTop,
    int bars, const Vec4& barCol, const Vec4& backingCol,
    int accentBarIdx = -1, const Vec4& accentBarCol = Vec4(0.1f, 0.5f, 0.9f, 1.0f),
    float curveHalfW = 1.22f, float edgeSweepX = 0.09f
) {
    // Curved backing plate
    const int n = 6;
    float xBack = xCenterFront + 0.018f;
    for (int i = 0; i < n; ++i) {
        float t0 = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(n));
        float t1 = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(n));
        float zB0 = t0 * halfWidthBot, zB1 = t1 * halfWidthBot;
        float zT0 = t0 * halfWidthTop, zT1 = t1 * halfWidthTop;
        float xB0 = CurvedNoseXAtZ(zB0, xBack, curveHalfW, edgeSweepX);
        float xB1 = CurvedNoseXAtZ(zB1, xBack, curveHalfW, edgeSweepX);
        float xT0 = CurvedNoseXAtZ(zT0, xBack, curveHalfW, edgeSweepX);
        float xT1 = CurvedNoseXAtZ(zT1, xBack, curveHalfW, edgeSweepX);
        PushQuad(mesh, Vec3(xB0, yBot, zB0), Vec3(xB1, yBot, zB1), Vec3(xT1, yTop, zT1), Vec3(xT0, yTop, zT0), backingCol);
    }

    float span = yTop - yBot;
    for (int i = 0; i < bars; ++i) {
        float t = (static_cast<float>(i) + 0.5f) / static_cast<float>(bars);
        float y = yBot + t * span;
        float barHW = halfWidthBot + (halfWidthTop - halfWidthBot) * t - 0.025f;
        const Vec4& col = (i == accentBarIdx) ? accentBarCol : barCol;
        AppendCurvedPanelX(mesh, xCenterFront - 0.008f, 0.026f, y - 0.021f, y + 0.021f, y - 0.021f, y + 0.021f, -barHW, barHW, curveHalfW, edgeSweepX, 6, col, 0.007f);
    }
}

// Carved curved corner headlamp cluster (chamfered/sloped bezel hugging curved bumper corner, NEVER a protruding box!)
inline void AppendCarvedHeadlampClusterX(
    Mesh* mesh,
    float xCenterFront, float cabHW, float edgeSweepX,
    float yBot, float yTop,
    float zAbsIn, float zAbsOut,
    int side, // -1 left, +1 right
    float outerTopDropY, float innerBotRiseY,
    const Vec4& bezelCol, const Vec4& lensCol, const Vec4& drlCol, const Vec4& amberCol,
    bool hasAmberCorner = true,
    bool isDrlTop = false
) {
    float s = static_cast<float>(side);
    float zA = s * zAbsIn;
    float zB = s * zAbsOut;
    float zLeft  = std::min(zA, zB);
    float zRight = std::max(zA, zB);
    // Left vs Right side mapping for inner vs outer edges
    float yBotL = (side > 0) ? (yBot + innerBotRiseY) : yBot;
    float yBotR = (side > 0) ? yBot : (yBot + innerBotRiseY);
    float yTopL = (side > 0) ? yTop : (yTop - outerTopDropY);
    float yTopR = (side > 0) ? (yTop - outerTopDropY) : yTop;

    // 1. Carved curved dark bezel pocket nestled deeply into curved bumper corner
    AppendCurvedPanelX(mesh, xCenterFront, 0.115f, yBotL, yTopL, yBotR, yTopR, zLeft, zRight, cabHW, edgeSweepX, 4, bezelCol, 0.025f);

    // 2. Glowing inner projector lenses (curved & chamfered inside dark pocket)
    float padZ = 0.030f;
    float padY = 0.032f;
    float spanZ = zRight - zLeft;
    float zMainL = zLeft + padZ;
    float zMainR = zRight - padZ;
    if (hasAmberCorner && spanZ > 0.18f) {
        float ambW = spanZ * 0.24f;
        if (side > 0) {
            float zAmbL = zRight - ambW;
            AppendCurvedPanelX(mesh, xCenterFront - 0.006f, 0.018f, yBotR + padY + 0.01f, yTopR - padY - 0.01f, yBotR + padY + 0.01f, yTopR - padY - 0.01f, zAmbL, zRight - 0.015f, cabHW, edgeSweepX, 2, amberCol, 0.010f);
            zMainR = zAmbL - 0.012f;
        } else {
            float zAmbR = zLeft + ambW;
            AppendCurvedPanelX(mesh, xCenterFront - 0.006f, 0.018f, yBotL + padY + 0.01f, yTopL - padY - 0.01f, yBotL + padY + 0.01f, yTopL - padY - 0.01f, zLeft + 0.015f, zAmbR, cabHW, edgeSweepX, 2, amberCol, 0.010f);
            zMainL = zAmbR + 0.012f;
        }
    }
    if (zMainR > zMainL + 0.04f) {
        AppendCurvedPanelX(mesh, xCenterFront - 0.006f, 0.018f, yBot + padY + 0.02f, yTop - padY - 0.02f, yBot + padY + 0.02f, yTop - padY - 0.02f, zMainL, zMainR, cabHW, edgeSweepX, 3, lensCol, 0.012f);
    }

    // 3. Signature curved DRL LED eyebrow/blade (bottom or top edge)
    float drlH = 0.026f;
    if (isDrlTop) {
        AppendCurvedPanelX(mesh, xCenterFront - 0.009f, 0.014f, yTopL - drlH, yTopL, yTopR - drlH, yTopR, zLeft + 0.012f, zRight - 0.012f, cabHW, edgeSweepX, 4, drlCol, 0.005f);
    } else {
        AppendCurvedPanelX(mesh, xCenterFront - 0.009f, 0.014f, yBotL, yBotL + drlH, yBotR, yBotR + drlH, zLeft + 0.012f, zRight - 0.012f, cabHW, edgeSweepX, 4, drlCol, 0.005f);
    }
}

// Curved matte-black lower windshield cowl tray & dual wipers (clean without generic chrome overlap)
inline void AppendTruckCowlAndWipersX(
    Mesh* mesh,
    float xFront, float cabHW, float windBotY,
    const Vec4& cowlCol
) {
    float spanZ = cabHW - 0.14f;
    AppendCurvedPanelX(mesh, xFront + 0.095f, 0.065f, windBotY - 0.045f, windBotY + 0.010f, windBotY - 0.045f, windBotY + 0.010f, -spanZ, spanZ, cabHW, 0.09f, 6, cowlCol, 0.008f);
    // Dual parked wiper blades
    AppendCurvedPanelX(mesh, xFront + 0.088f, 0.018f, windBotY + 0.015f, windBotY + 0.042f, windBotY + 0.015f, windBotY + 0.042f, -spanZ * 0.78f, -0.05f, cabHW, 0.09f, 4, cowlCol);
    AppendCurvedPanelX(mesh, xFront + 0.088f, 0.018f, windBotY + 0.015f, windBotY + 0.042f, windBotY + 0.015f, windBotY + 0.042f,  0.05f, spanZ * 0.78f, cabHW, 0.09f, 4, cowlCol);
}

// Digital camera wing pods (Volvo FH Electric MirrorCam, eActros 600, GenH2, F-MAX Aero)
inline void AppendMirrorCamWings(
    Mesh* mesh,
    float anchorX, float anchorY, float cabHW,
    const Vec4& armCol, const Vec4& podCol, const Vec4& lensCol
) {
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zInner = s * (cabHW - 0.02f);
        float zTip   = s * (cabHW + 0.25f);
        float z0 = std::min(zInner, zTip);
        float z1 = std::max(zInner, zTip);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.040f, anchorY - 0.018f, z0), Vec3(anchorX + 0.028f, anchorY + 0.018f, z1), armCol);
        float zCam0 = s > 0 ? (zTip - 0.050f) : (zTip - 0.014f);
        float zCam1 = s > 0 ? (zTip + 0.014f) : (zTip + 0.050f);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.042f, anchorY - 0.085f, zCam0), Vec3(anchorX + 0.050f, anchorY + 0.022f, zCam1), podCol);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.046f, anchorY - 0.072f, zCam0 + 0.008f), Vec3(anchorX - 0.036f, anchorY - 0.014f, zCam1 - 0.008f), lensCol);
    }
}

// Front A-pillar corner kerb mirror
inline void AppendFrontKerbMirror(Mesh* mesh, float frontX, float topY, float sideZ, const Vec4& housingCol) {
    float s = sideZ >= 0.0f ? 1.0f : -1.0f;
    float zArm = sideZ + s * 0.13f;
    RunningGear::AppendBox(mesh, Vec3(frontX - 0.16f, topY - 0.018f, std::min(sideZ, zArm)), Vec3(frontX + 0.02f, topY + 0.018f, std::max(sideZ, zArm)), housingCol);
    RunningGear::AppendBox(mesh, Vec3(frontX - 0.20f, topY - 0.10f, zArm - 0.055f), Vec3(frontX - 0.11f, topY + 0.018f, zArm + 0.055f), housingCol);
}

// Twin roof-mounted air horns
inline void AppendRoofAirHorns(Mesh* mesh, float xFront, float roofY, float spanZ, const Vec4& hornCol) {
    for (float zSign : {-1.0f, 1.0f}) {
        float cz = zSign * spanZ;
        ChassisComponents::AppendClosedCylinderX(mesh, xFront, xFront + 0.56f, roofY + 0.045f, cz, 0.038f, 12, hornCol, Vec4(0.10f, 0.10f, 0.12f, 1.0f), 0.04f);
        RunningGear::AppendBox(mesh, Vec3(xFront + 0.22f, roofY - 0.01f, cz - 0.02f), Vec3(xFront + 0.34f, roofY + 0.02f, cz + 0.02f), hornCol);
    }
}

// Exposed step cutouts + grab treads
inline void AppendCabEntrySteps(
    Mesh* mesh,
    float stepX0, float stepX1,
    float frameBotY, float cabClearY, float cabHW,
    const Vec4& recessCol, const Vec4& treadCol
) {
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zInner = s * (cabHW - 0.18f);
        float zOuter = s * (cabHW - 0.01f);
        float z0 = std::min(zInner, zOuter);
        float z1 = std::max(zInner, zOuter);
        RunningGear::AppendBox(mesh, Vec3(stepX0, frameBotY - 0.06f, z0), Vec3(stepX1, cabClearY + 0.02f, z1), recessCol);
        float midY = (frameBotY + cabClearY) * 0.5f;
        RunningGear::AppendBox(mesh, Vec3(stepX0 + 0.02f, frameBotY - 0.04f, z0), Vec3(stepX1 - 0.02f, frameBotY + 0.01f, s > 0 ? zOuter + 0.015f : zOuter - 0.015f), treadCol);
        RunningGear::AppendBox(mesh, Vec3(stepX0 + 0.02f, midY - 0.01f,      z0), Vec3(stepX1 - 0.02f, midY + 0.03f,      s > 0 ? zOuter + 0.015f : zOuter - 0.015f), treadCol);
    }
}

// Bespoke animated truck door supporting kerb-dip forward beltline, optional split quarter-glass bar,
// AND exact beltline alignment with panoramic windshield (never overly tall!).
inline void AppendSteppedCabDoor(
    Mesh* mesh,
    float doorFrontX, float doorBackX,
    float botY, float topY,
    float sideZ, bool isRightSide,
    float openRatio,
    float frontDropDepthY,
    bool hasQuarterPost,
    bool emitGlassPass,
    const Vec4& paintColor,
    const Vec4& trimColor,
    const Vec4& glassColor,
    float explicitMainBeltY = -1.0f
) {
    if (!mesh || doorBackX <= doorFrontX + 0.20f) return;
    float clampOpen = std::clamp(openRatio, 0.0f, 1.0f);
    float sign = isRightSide ? 1.0f : -1.0f;
    float spanY = topY - botY;
    float winBotMain = (explicitMainBeltY > botY + 0.20f) ? explicitMainBeltY : (botY + spanY * 0.54f);
    float winBotFront = std::max(botY + 0.24f, winBotMain - frontDropDepthY);
    float winTop = topY - 0.05f;
    float stileW = 0.058f;
    float xKnee = doorFrontX + (doorBackX - doorFrontX) * 0.44f;

    float angleRad = (isRightSide ? 1.0f : -1.0f) * clampOpen * 1.24f;
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    auto rotPt = [&](float x, float y, float z) -> Vec3 {
        float dx = x - doorFrontX;
        float dz = z - sideZ;
        return Vec3(doorFrontX + dx * c - dz * s, y, sideZ + dx * s + dz * c);
    };

    float zA = sideZ - sign * 0.030f;
    float zB = sideZ + sign * 0.018f;
    float zMin = std::min(zA, zB);
    float zMax = std::max(zA, zB);

    auto emitBoxRot = [&](const Vec3& p0, const Vec3& p1, const Vec4& col) {
        Vec3 c000 = rotPt(p0.x, p0.y, p0.z), c100 = rotPt(p1.x, p0.y, p0.z);
        Vec3 c110 = rotPt(p1.x, p1.y, p0.z), c010 = rotPt(p0.x, p1.y, p0.z);
        Vec3 c001 = rotPt(p0.x, p0.y, p1.z), c101 = rotPt(p1.x, p0.y, p1.z);
        Vec3 c111 = rotPt(p1.x, p1.y, p1.z), c011 = rotPt(p0.x, p1.y, p1.z);
        PushQuad(mesh, c001, c101, c111, c011, col);
        PushQuad(mesh, c100, c000, c010, c110, col);
        PushQuad(mesh, c011, c111, c110, c010, col);
        PushQuad(mesh, c000, c100, c101, c001, col);
        PushQuad(mesh, c101, c100, c110, c111, col);
        PushQuad(mesh, c000, c001, c011, c010, col);
    };

    auto emitSlopedPanel = [&](float xA, float xB, float yB0, float yT0, float yB1, float yT1, float z0, float z1, const Vec4& col) {
        Vec3 oBL = rotPt(xA, yB0, z1), oBR = rotPt(xB, yB1, z1), oTR = rotPt(xB, yT1, z1), oTL = rotPt(xA, yT0, z1);
        Vec3 iBL = rotPt(xA, yB0, z0), iBR = rotPt(xB, yB1, z0), iTR = rotPt(xB, yT1, z0), iTL = rotPt(xA, yT0, z0);
        if (isRightSide) {
            PushQuad(mesh, oBL, oBR, oTR, oTL, col);
            PushQuad(mesh, iBR, iBL, iTL, iTR, col);
            PushQuad(mesh, oTL, oTR, iTR, iTL, col);
            PushQuad(mesh, iBL, iBR, oBR, oBL, col);
        } else {
            PushQuad(mesh, iBR, iBL, iTL, iTR, col);
            PushQuad(mesh, oBL, oBR, oTR, oTL, col);
            PushQuad(mesh, iTL, iTR, oTR, oTL, col);
            PushQuad(mesh, oBL, oBR, iBR, iBL, col);
        }
        PushQuad(mesh, iBL, oBL, oTL, iTL, col);
        PushQuad(mesh, oBR, iBR, iTR, oTR, col);
    };

    if (!emitGlassPass) {
        emitSlopedPanel(doorFrontX, xKnee, botY, winBotFront, botY, winBotMain, zMin, zMax, paintColor);
        emitBoxRot(Vec3(xKnee, botY, zMin), Vec3(doorBackX, winBotMain, zMax), paintColor);

        emitBoxRot(Vec3(doorFrontX, winBotFront, zMin), Vec3(doorFrontX + stileW, topY, zMax), trimColor);
        emitBoxRot(Vec3(doorBackX - stileW, winBotMain, zMin), Vec3(doorBackX, topY, zMax), trimColor);
        emitBoxRot(Vec3(doorFrontX + stileW, winTop, zMin), Vec3(doorBackX - stileW, topY, zMax), trimColor);
        emitSlopedPanel(doorFrontX + stileW, xKnee, winBotFront - 0.022f, winBotFront + 0.01f, winBotMain - 0.022f, winBotMain + 0.01f, zMin - 0.004f, zMax + 0.004f, trimColor);
        emitBoxRot(Vec3(xKnee, winBotMain - 0.022f, zMin - 0.004f), Vec3(doorBackX, winBotMain + 0.01f, zMax + 0.004f), trimColor);

        if (hasQuarterPost) {
            float postX = doorBackX - (doorBackX - doorFrontX) * 0.28f;
            emitBoxRot(Vec3(postX - 0.022f, winBotMain, zMin - 0.003f), Vec3(postX + 0.022f, winTop, zMax + 0.003f), trimColor);
        }

        float hZ0 = isRightSide ? zMax : zMin - 0.022f;
        float hZ1 = isRightSide ? zMax + 0.022f : zMin;
        emitBoxRot(Vec3(doorBackX - 0.18f, winBotMain - 0.19f, hZ0), Vec3(doorBackX - 0.06f, winBotMain - 0.07f, hZ1), Vec4(0.10f, 0.11f, 0.13f, 1.0f));
    } else {
        float gzMin = (zMin + zMax) * 0.5f - 0.005f;
        float gzMax = (zMin + zMax) * 0.5f + 0.005f;
        emitSlopedPanel(doorFrontX + stileW, xKnee, winBotFront, winTop, winBotMain, winTop, gzMin, gzMax, glassColor);
        emitBoxRot(Vec3(xKnee, winBotMain, gzMin), Vec3(doorBackX - stileW, winTop, gzMax), glassColor);
    }
}

// Complete tractor chassis: C-channel rails, steered front axle, 4x2/6x2/6x4 rear axles, wheel arches & 5th wheel
inline void AppendTractorChassisBase(
    Mesh* mesh,
    const VehicleDef& def,
    float xFront, float xRear,
    float frontAxleX, float driveCenterX, float fifthWheelX,
    float wheelR, float tireW, float hw,
    float cabHW, float frameBotY, float frameTopY, float archTopY, float archHalfLen,
    const Vec4& frameDark, const Vec4& fenderCol, const Vec4& linerDark, const Vec4& alloyTank
) {
    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = hw - tireW * 1.06f - 0.025f;

    ChassisComponents::AppendFrameRails(mesh, xFront + 0.16f, xRear - 0.06f, frameBotY, frameTopY, 0.43f, frameDark);

    ChassisComponents::AppendVehicleAxle(mesh, frontAxleX, wheelR, tireW, frontHalfTrack, false, def.steer_angle_deg);
    CabComponents::AppendWheelWellLiner(mesh, frontAxleX, archHalfLen, frameBotY - 0.12f, archTopY, cabHW, cabHW - tireW - 0.14f, linerDark);
    CabComponents::AppendWheelArchFender(mesh, frontAxleX, wheelR, wheelR + 0.09f, -cabHW, false, 0.038f, fenderCol);
    CabComponents::AppendWheelArchFender(mesh, frontAxleX, wheelR, wheelR + 0.09f,  cabHW, true,  0.038f, fenderCol);

    if (def.drive_config == VehicleDriveConfig::DRIVE_4X2) {
        ChassisComponents::AppendVehicleAxle(mesh, driveCenterX, wheelR, tireW, rearHalfTrack, true, 0.0f);
        CabComponents::AppendWheelArchFender(mesh, driveCenterX, wheelR, wheelR + 0.08f, -hw, false, tireW * 1.15f, fenderCol);
        CabComponents::AppendWheelArchFender(mesh, driveCenterX, wheelR, wheelR + 0.08f,  hw, true,  tireW * 1.15f, fenderCol);
    } else if (def.drive_config == VehicleDriveConfig::DRIVE_6X2_TAG) {
        float d0 = driveCenterX - 0.68f;
        float tagX = driveCenterX + 0.68f;
        ChassisComponents::AppendVehicleAxle(mesh, d0, wheelR, tireW, rearHalfTrack, true, 0.0f);
        ChassisComponents::AppendVehicleAxle(mesh, tagX, wheelR, tireW, frontHalfTrack, false, -def.steer_angle_deg * 0.28f);
        for (float dx : {d0, tagX}) {
            CabComponents::AppendWheelArchFender(mesh, dx, wheelR, wheelR + 0.08f, -hw, false, tireW * 1.15f, fenderCol);
            CabComponents::AppendWheelArchFender(mesh, dx, wheelR, wheelR + 0.08f,  hw, true,  tireW * 1.15f, fenderCol);
        }
    } else {
        float d0 = driveCenterX - 0.68f;
        float d1 = driveCenterX + 0.68f;
        ChassisComponents::AppendVehicleAxle(mesh, d0, wheelR, tireW, rearHalfTrack, true, 0.0f);
        ChassisComponents::AppendVehicleAxle(mesh, d1, wheelR, tireW, rearHalfTrack, true, 0.0f);
        for (float dx : {d0, d1}) {
            CabComponents::AppendWheelArchFender(mesh, dx, wheelR, wheelR + 0.08f, -hw, false, tireW * 1.15f, fenderCol);
            CabComponents::AppendWheelArchFender(mesh, dx, wheelR, wheelR + 0.08f,  hw, true,  tireW * 1.15f, fenderCol);
        }
    }

    ChassisComponents::AppendDrivetrain(mesh, frontAxleX + 0.25f, driveCenterX, std::max(0.24f, def.ground_clearance_m), wheelR);
    ChassisComponents::AppendFifthWheelSaddle(mesh, fifthWheelX, frameTopY);

    RunningGear::AppendBox(mesh, Vec3(xRear - 0.06f, frameBotY - 0.06f, -hw + 0.04f), Vec3(xRear + 0.02f, frameTopY + 0.02f, -hw + 0.48f), fenderCol);
    RunningGear::AppendBox(mesh, Vec3(xRear - 0.06f, frameBotY - 0.06f,  hw - 0.48f), Vec3(xRear + 0.02f, frameTopY + 0.02f,  hw - 0.04f), fenderCol);
    RunningGear::AppendBox(mesh, Vec3(xRear - 0.03f, frameBotY + 0.04f, -0.46f), Vec3(xRear + 0.025f, frameBotY + 0.14f, -0.22f), Vec4(0.92f, 0.10f, 0.12f, 1.0f));
    RunningGear::AppendBox(mesh, Vec3(xRear - 0.03f, frameBotY + 0.04f,  0.22f), Vec3(xRear + 0.025f, frameBotY + 0.14f,  0.46f), Vec4(0.92f, 0.10f, 0.12f, 1.0f));
}

} // namespace sphere::vehicles::trucks
