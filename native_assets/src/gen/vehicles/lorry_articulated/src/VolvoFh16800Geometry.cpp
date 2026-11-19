#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace sphere::vehicles::trucks {

namespace {

// Unified smooth aerodynamic front fascia surface X(y, z) shared across all front panels, grilles & lamps
// so nothing sticks out or buries adjacent trim.
inline float AeroFrontLatSweep(float z, float cabHW, float maxSweepX) {
    float nz = std::clamp(std::abs(z) / std::max(0.25f, cabHW), 0.0f, 1.0f);
    return maxSweepX * (0.24f * nz + 0.76f * nz * nz);
}

inline float AeroLowerFrontX(float y, float z, float xFront, float cabHW, float splitterBotY, float windBotY) {
    float ty = std::clamp((y - splitterBotY) / std::max(0.2f, windBotY - splitterBotY), 0.0f, 1.0f);
    return xFront + 0.135f * ty + AeroFrontLatSweep(z, cabHW, 0.095f);
}

inline float AeroRoofBrowX(float y, float z, float windBrowX, float cabBackX, float windTopY, float cabTopY, float cabHW) {
    float tr = std::clamp((y - windTopY) / std::max(0.2f, cabTopY - windTopY), 0.0f, 1.0f);
    // Smooth convex airfoil curve rolling cleanly back from above sunvisor into roof crown
    float archBack = (0.04f * tr + 0.42f * tr * tr * tr);
    float maxBack = std::min(cabBackX - 0.85f, windBrowX + archBack);
    (void)maxBack;
    return windBrowX + archBack + AeroFrontLatSweep(z, cabHW, 0.092f) * (1.0f - 0.30f * tr * tr);
}

// Smooth curved rectangular surface strip on the front face using a custom surface evaluator surfX(y, z)
template <typename SurfFn>
void AppendCurvedSurfPatchX(
    Mesh* mesh,
    float y0, float y1,
    float z0, float z1,
    int segY, int segZ,
    float depthOffsetX,
    SurfFn&& surfX,
    const Vec4& col
) {
    if (!mesh || y1 <= y0 + 1e-4f || z1 <= z0 + 1e-4f) return;
    int ny = std::max(1, segY);
    int nz = std::max(1, segZ);
    for (int iy = 0; iy < ny; ++iy) {
        float ty0 = static_cast<float>(iy) / static_cast<float>(ny);
        float ty1 = static_cast<float>(iy + 1) / static_cast<float>(ny);
        float ya = y0 + (y1 - y0) * ty0;
        float yb = y0 + (y1 - y0) * ty1;
        for (int iz = 0; iz < nz; ++iz) {
            float tz0 = static_cast<float>(iz) / static_cast<float>(nz);
            float tz1 = static_cast<float>(iz + 1) / static_cast<float>(nz);
            float za = z0 + (z1 - z0) * tz0;
            float zb = z0 + (z1 - z0) * tz1;

            Vec3 a(surfX(ya, za) + depthOffsetX, ya, za);
            Vec3 b(surfX(ya, zb) + depthOffsetX, ya, zb);
            Vec3 c(surfX(yb, zb) + depthOffsetX, yb, zb);
            Vec3 d(surfX(yb, za) + depthOffsetX, yb, za);
            // (+dz) x (+dy) = -X outward
            PushQuad(mesh, a, b, c, d, col);
        }
    }
}

// Tapered curved surface strip between variable left/right bounds zLeft(y)..zRight(y) on surfX(y, z)
template <typename SurfFn>
void AppendCurvedTaperedPatchX(
    Mesh* mesh,
    float y0, float y1,
    float zL0, float zL1,
    float zR0, float zR1,
    int segY, int segZ,
    float depthOffsetX,
    SurfFn&& surfX,
    const Vec4& col
) {
    if (!mesh || y1 <= y0 + 1e-4f) return;
    int ny = std::max(1, segY);
    int nz = std::max(1, segZ);
    for (int iy = 0; iy < ny; ++iy) {
        float ty0 = static_cast<float>(iy) / static_cast<float>(ny);
        float ty1 = static_cast<float>(iy + 1) / static_cast<float>(ny);
        float ya = y0 + (y1 - y0) * ty0;
        float yb = y0 + (y1 - y0) * ty1;
        float zLa = zL0 + (zL1 - zL0) * ty0, zRa = zR0 + (zR1 - zR0) * ty0;
        float zLb = zL0 + (zL1 - zL0) * ty1, zRb = zR0 + (zR1 - zR0) * ty1;
        if (zRa <= zLa + 1e-4f || zRb <= zLb + 1e-4f) continue;
        for (int iz = 0; iz < nz; ++iz) {
            float tz0 = static_cast<float>(iz) / static_cast<float>(nz);
            float tz1 = static_cast<float>(iz + 1) / static_cast<float>(nz);
            float za0 = zLa + (zRa - zLa) * tz0, za1 = zLa + (zRa - zLa) * tz1;
            float zb0 = zLb + (zRb - zLb) * tz0, zb1 = zLb + (zRb - zLb) * tz1;
            Vec3 a(surfX(ya, za0) + depthOffsetX, ya, za0);
            Vec3 b(surfX(ya, za1) + depthOffsetX, ya, za1);
            Vec3 c(surfX(yb, zb1) + depthOffsetX, yb, zb1);
            Vec3 d(surfX(yb, zb0) + depthOffsetX, yb, zb0);
            PushQuad(mesh, a, b, c, d, col);
        }
    }
}

// 3D proud curved horizontal/beveled strip flush with AeroLowerFrontX
inline void AppendFlushFrontStripX(
    Mesh* mesh,
    float xFront, float cabHW, float splitterBotY, float windBotY,
    float yBot, float yTop,
    float zLeft, float zRight,
    float proudFrontX, float depthBackX,
    int segsZ, const Vec4& col
) {
    if (!mesh || zRight <= zLeft + 1e-3f || yTop <= yBot + 1e-4f) return;
    int n = std::max(2, segsZ);
    for (int i = 0; i < n; ++i) {
        float t0 = static_cast<float>(i) / static_cast<float>(n);
        float t1 = static_cast<float>(i + 1) / static_cast<float>(n);
        float z0 = zLeft + (zRight - zLeft) * t0;
        float z1 = zLeft + (zRight - zLeft) * t1;

        float xF00 = AeroLowerFrontX(yBot, z0, xFront, cabHW, splitterBotY, windBotY) - proudFrontX;
        float xF01 = AeroLowerFrontX(yBot, z1, xFront, cabHW, splitterBotY, windBotY) - proudFrontX;
        float xF11 = AeroLowerFrontX(yTop, z1, xFront, cabHW, splitterBotY, windBotY) - proudFrontX;
        float xF10 = AeroLowerFrontX(yTop, z0, xFront, cabHW, splitterBotY, windBotY) - proudFrontX;

        float xB00 = xF00 + depthBackX;
        float xB01 = xF01 + depthBackX;
        float xB11 = xF11 + depthBackX;
        float xB10 = xF10 + depthBackX;

        Vec3 fBL(xF00, yBot, z0), fBR(xF01, yBot, z1), fTR(xF11, yTop, z1), fTL(xF10, yTop, z0);
        Vec3 bBL(xB00, yBot, z0), bBR(xB01, yBot, z1), bTR(xB11, yTop, z1), bTL(xB10, yTop, z0);

        PushQuad(mesh, fBL, fBR, fTR, fTL, col);
        PushQuad(mesh, fTL, fTR, bTR, bTL, col);
        PushQuad(mesh, bBL, bBR, fBR, fBL, col);
        if (i == 0)     PushQuad(mesh, bBL, fBL, fTL, bTL, col);
        if (i == n - 1) PushQuad(mesh, fBR, bBR, bTR, fTR, col);
    }
}

// 3D tapered grille louvers with slightly swept outer wing tips aligned to AeroLowerFrontX
inline void AppendFlushTaperedGrille(
    Mesh* mesh,
    float xFront, float cabHW, float splitterBotY, float windBotY,
    float yBot, float yTop,
    float hwBot, float hwTop,
    int slats,
    const Vec4& slatCol, const Vec4& highlightCol, const Vec4& trimCol
) {
    float spanY = yTop - yBot;
    // Slim tapered outer side border trims
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        const int sy = 5;
        for (int i = 0; i < sy; ++i) {
            float t0 = static_cast<float>(i) / static_cast<float>(sy);
            float t1 = static_cast<float>(i + 1) / static_cast<float>(sy);
            float ya = yBot + spanY * t0, yb = yBot + spanY * t1;
            float wa = hwBot + (hwTop - hwBot) * t0;
            float wb = hwBot + (hwTop - hwBot) * t1;
            float zA = s * wa, zB = s * wb;
            float z0a = s > 0 ? (zA - 0.024f) : zA;
            float z1a = s > 0 ? zA : (zA + 0.024f);
            float z0b = s > 0 ? (zB - 0.024f) : zB;
            float z1b = s > 0 ? zB : (zB + 0.024f);
            float xA0 = AeroLowerFrontX(ya, z0a, xFront, cabHW, splitterBotY, windBotY) - 0.004f;
            float xA1 = AeroLowerFrontX(ya, z1a, xFront, cabHW, splitterBotY, windBotY) - 0.004f;
            float xB1 = AeroLowerFrontX(yb, z1b, xFront, cabHW, splitterBotY, windBotY) - 0.004f;
            float xB0 = AeroLowerFrontX(yb, z0b, xFront, cabHW, splitterBotY, windBotY) - 0.004f;
            PushQuad(mesh, Vec3(xA0, ya, z0a), Vec3(xA1, ya, z1a), Vec3(xB1, yb, z1b), Vec3(xB0, yb, z0b), trimCol);
        }
    }

    // Horizontal sculpted slats curving smoothly across Z
    for (int i = 0; i < slats; ++i) {
        float t = (static_cast<float>(i) + 0.5f) / static_cast<float>(slats);
        float yMid = yBot + t * spanY;
        float barHW = hwBot + (hwTop - hwBot) * t - 0.024f;
        float slatH = std::min(0.036f, (spanY / static_cast<float>(slats)) * 0.28f);
        AppendFlushFrontStripX(
            mesh, xFront, cabHW, splitterBotY, windBotY,
            yMid - slatH, yMid + slatH * 0.72f, -barHW, barHW,
            0.008f, 0.018f, 12, slatCol
        );
        AppendFlushFrontStripX(
            mesh, xFront, cabHW, splitterBotY, windBotY,
            yMid + slatH * 0.72f, yMid + slatH, -barHW, barHW,
            0.010f, 0.012f, 12, highlightCol
        );
    }
}

// Crisp slanted 3D trim line following AeroLowerFrontX (without radial cones)
inline void AppendFlushSlantedLineX(
    Mesh* mesh,
    float xFront, float cabHW, float splitterBotY, float windBotY,
    float y0, float z0,
    float y1, float z1,
    float halfThick, float proudFrontX, float depthBackX,
    const Vec4& col, int segs = 6
) {
    float dy = y1 - y0, dz = z1 - z0;
    float len = std::sqrt(dy * dy + dz * dz);
    if (len < 1e-4f) return;
    float ny = -dz / len * halfThick;
    float nz =  dy / len * halfThick;

    for (int i = 0; i < segs; ++i) {
        float t0 = static_cast<float>(i) / static_cast<float>(segs);
        float t1 = static_cast<float>(i + 1) / static_cast<float>(segs);
        float ya = y0 + dy * t0, za = z0 + dz * t0;
        float yb = y0 + dy * t1, zb = z0 + dz * t1;

        float xFa = AeroLowerFrontX(ya, za, xFront, cabHW, splitterBotY, windBotY) - proudFrontX;
        float xFb = AeroLowerFrontX(yb, zb, xFront, cabHW, splitterBotY, windBotY) - proudFrontX;
        float xBa = xFa + depthBackX, xBb = xFb + depthBackX;

        Vec3 f0A(xFa, ya - ny, za - nz), f0B(xFa, ya + ny, za + nz);
        Vec3 f1B(xFb, yb + ny, zb + nz), f1A(xFb, yb - ny, zb - nz);
        Vec3 b0A(xBa, ya - ny, za - nz), b0B(xBa, ya + ny, za + nz);
        Vec3 b1B(xBb, yb + ny, zb + nz), b1A(xBb, yb - ny, zb - nz);

        Vec3 e1 = f0B - f0A, e2 = f1B - f0A;
        float nx = e1.y * e2.z - e1.z * e2.y;
        if (nx <= 0.0f) {
            PushQuad(mesh, f0A, f0B, f1B, f1A, col);
            PushQuad(mesh, f0A, f1A, b1A, b0A, col);
            PushQuad(mesh, f1B, f0B, b0B, b1B, col);
        } else {
            PushQuad(mesh, f0B, f0A, f1A, f1B, col);
            PushQuad(mesh, f0B, f1B, b1B, b0B, col);
            PushQuad(mesh, f1A, f0A, b0A, b1A, col);
        }
    }
}

// 3D micro chrome emblem badge ("800") flush on AeroLowerFrontX
void AppendEmblemDigit(
    Mesh* mesh,
    char ch,
    float xFront, float cabHW, float splitterBotY, float windBotY,
    float yBot, float zStart,
    float charH, float charW, float stroke, float proudX,
    const Vec4& col
) {
    uint8_t mask = 0;
    if (ch == '8')      mask = 0x7F;
    else if (ch == '0') mask = 0x3F;
    if (mask == 0) return;

    float yMid = yBot + charH * 0.5f;
    float yTop = yBot + charH;
    float zL = zStart, zR = zStart + charW;
    float sH = stroke, sV = stroke * 0.92f;

    auto seg = [&](float ya, float yb, float za, float zb) {
        AppendFlushFrontStripX(mesh, xFront, cabHW, splitterBotY, windBotY, ya, yb, za, zb, proudX, proudX + 0.008f, 2, col);
    };

    if (mask & 0x01) seg(yTop - sH, yTop, zL, zR);
    if (mask & 0x02) seg(yMid, yTop, zR - sV, zR);
    if (mask & 0x04) seg(yBot, yMid, zR - sV, zR);
    if (mask & 0x08) seg(yBot, yBot + sH, zL, zR);
    if (mask & 0x10) seg(yBot, yMid, zL, zL + sV);
    if (mask & 0x20) seg(yMid, yTop, zL, zL + sV);
    if (mask & 0x40) seg(yMid - sH * 0.5f, yMid + sH * 0.5f, zL, zR);
}

// Slim sculpted twin-arm aerodynamic Volvo FH16 Aero wing mirrors with silver/LED brow stripe
void AppendVolvoWingMirrors(
    Mesh* mesh,
    float anchorX,
    float yBot,
    float yTop,
    float cabHW,
    const Vec4& housingCol,
    const Vec4& glassCol,
    const Vec4& accentCol
) {
    const float yLowArm = yBot + 0.08f;
    const float yHighArm = yTop - 0.06f;

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zInner = s * (cabHW - 0.02f);
        float zShellIn = s * (cabHW + 0.16f);
        float zShellOut = s * (cabHW + 0.28f);

        float armZ0 = std::min(zInner, zShellIn + s * 0.03f);
        float armZ1 = std::max(zInner, zShellIn + s * 0.03f);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.06f, yHighArm - 0.020f, armZ0), Vec3(anchorX + 0.02f, yHighArm + 0.020f, armZ1), housingCol);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.03f, yLowArm - 0.018f,  armZ0), Vec3(anchorX + 0.04f, yLowArm + 0.018f,  armZ1), housingCol);

        float shZ0 = std::min(zShellIn, zShellOut);
        float shZ1 = std::max(zShellIn, zShellOut);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.085f, yBot, shZ0), Vec3(anchorX + 0.055f, yTop, shZ1), housingCol);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.070f, yTop, shZ0 + 0.012f), Vec3(anchorX + 0.045f, yTop + 0.042f, shZ1 - 0.012f), housingCol);

        // Rear-facing (+X) mirror glass + upper outer silver/LED L-trim signal stripe matching photo
        RunningGear::AppendBox(mesh, Vec3(anchorX + 0.050f, yBot + 0.035f, shZ0 + 0.016f), Vec3(anchorX + 0.064f, yTop - 0.035f, shZ1 - 0.016f), glassCol);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.090f, yTop - 0.025f, shZ0 + 0.014f), Vec3(anchorX - 0.078f, yTop + 0.014f, shZ1 - 0.014f), accentCol);
        float outerZ0 = s > 0 ? (shZ1 - 0.022f) : shZ0;
        float outerZ1 = s > 0 ? shZ1 : (shZ0 + 0.022f);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.090f, yTop - 0.16f, outerZ0), Vec3(anchorX - 0.078f, yTop + 0.014f, outerZ1), accentCol);
    }
}

// Alcoa polished chrome center cap + lug-ring accents aligned exactly to RunningGear::AppendWheelZ
void AppendAlcoaHubAccent(
    Mesh* mesh,
    float axleX,
    float wheelR,
    float tireW,
    float halfTrack,
    float steerAngleDeg,
    bool isDualRear,
    const Vec4& chromeCol,
    const Vec4& darkRing
) {
    float steerRad = steerAngleDeg * (3.14159265f / 180.0f);
    float cS = std::cos(steerRad);
    float sS = std::sin(steerRad);
    float outerOff = isDualRear ? (tireW * 0.56f) : 0.0f;
    float wheelHW = tireW * 0.5f;
    float dishZ = wheelHW * 0.58f;
    float hubZ = wheelHW * 0.80f;
    float dishR = wheelR * 0.48f;
    float hubR  = wheelR * 0.20f;
    int segs = 18;

    for (int side : {-1, 1}) {
        float sideSign = static_cast<float>(side);
        float cz = sideSign * (halfTrack + outerOff);
        float zDish = sideSign * dishZ;
        float zHub  = sideSign * hubZ;
        auto rotPt = [&](float lx, float ly, float lz) -> Vec3 {
            return Vec3(axleX + lx * cS - lz * sS, wheelR + ly, cz + lx * sS + lz * cS);
        };
        Vec3 cHub = rotPt(0.0f, 0.0f, zHub + sideSign * 0.004f);
        for (int i = 0; i < segs; ++i) {
            float a0 = (static_cast<float>(i) / static_cast<float>(segs)) * 6.2831853f;
            float a1 = (static_cast<float>(i + 1) / static_cast<float>(segs)) * 6.2831853f;
            float c0 = std::cos(a0), s0 = std::sin(a0);
            float c1 = std::cos(a1), s1 = std::sin(a1);
            Vec3 h0 = rotPt(hubR * c0,  hubR * s0,  zHub);
            Vec3 h1 = rotPt(hubR * c1,  hubR * s1,  zHub);
            Vec3 d0 = rotPt(dishR * c0, dishR * s0, zDish);
            Vec3 d1 = rotPt(dishR * c1, dishR * s1, zDish);
            if (side > 0) {
                PushTri(mesh, cHub, h0, h1, chromeCol);
                PushQuad(mesh, h0, d0, d1, h1, (i % 2 == 0) ? chromeCol : darkRing);
            } else {
                PushTri(mesh, cHub, h1, h0, chromeCol);
                PushQuad(mesh, h1, d1, d0, h0, (i % 2 == 0) ? chromeCol : darkRing);
            }
        }
    }
}

} // namespace

// Photo-matched procedural C++20 3D generator for Volvo FH16 800 Aero 4x2:
// - Continuous, smoothly curved aerodynamic front shell (no sharp single-vertex X-facets or jutting boxes)
// - Wide two-tier tapered gloss-black grilles (5-slat upper w/ Iron Mark + "800", 4-slat lower + silver-trimmed ducts)
// - Tapered painted olive cheeks tightly hugging upper & lower grille silhouettes + olive upper hood cowl
// - Flush curved corner LED headlamps (upper circular projector + 7-segment vertical beaded DRL blade)
// - Smoothly curved flush black sunvisor + airfoil Globetrotter XL brow w/ subtle V-crease + swept corner roof aero wings
std::shared_ptr<Mesh> GenerateVolvoFh16800Mesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 5.4f, 8.6f);
    const float W = std::clamp(def.dimensions.y, 2.38f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.35f, 4.10f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(def.wheel_radius_m, 0.46f, 0.56f);
    const float tireW = std::clamp(def.tire_width_m, 0.28f, 0.36f);
    const float groundClear = std::clamp(def.ground_clearance_m, 0.22f, 0.35f);

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float frameBotY = std::max(0.68f, wheelR * 2.0f - 0.14f);
    const float frameTopY = frameBotY + 0.25f;
    const float archTopY = wheelR * 2.0f + 0.14f;
    const float archHalfLen = wheelR + 0.12f;

    const float splitterBotY = groundClear - 0.02f;  // 0.23m
    const float bumperBotY   = groundClear + 0.03f;  // 0.28m
    const float cabBaseY     = frameBotY - 0.04f;    // 0.86m
    const float cabClearY    = std::max(cabBaseY + 0.24f, archTopY + 0.02f); // 1.20m
    const float cabHW        = hw - 0.03f;           // 1.23m
    const float cabBackX     = xFront + 2.54f;
    const float cabTopY      = std::clamp(def.cab_height_m, 3.45f, 3.98f);

    const float frontAxleX   = xFront + 1.42f;
    const float driveCenterX = std::min(xRear - 0.90f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX  = std::clamp(def.fifth_wheel_offset_m, 0.80f, xRear - 0.88f);

    const Vec4 olivePaint = def.base_color;
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 darkBacking(0.030f, 0.035f, 0.042f, 1.0f);
    const Vec4 slatHighlight(0.13f, 0.14f, 0.16f, 1.0f);
    const Vec4 darkTrim(0.10f, 0.11f, 0.13f, 1.0f);
    const Vec4 frameDark(0.12f, 0.14f, 0.17f, 1.0f);
    const Vec4 chromeCol(0.91f, 0.94f, 0.98f, 1.0f);
    const Vec4 satinSilver(0.76f, 0.79f, 0.84f, 1.0f);
    const Vec4 linerDark(0.045f, 0.05f, 0.06f, 1.0f);
    const Vec4 interiorSeat(0.24f, 0.34f, 0.12f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.14f, 1.0f);
    const Vec4 neonGreen(0.16f, 0.98f, 0.36f, 1.0f);
    const Vec4 windshieldGlass(0.24f, 0.42f, 0.54f, 0.24f);
    const Vec4 sideGlass(0.18f, 0.34f, 0.48f, 0.40f);
    const Vec4 amberLed(0.98f, 0.62f, 0.10f, 1.0f);
    const Vec4 ledWhite = def.headlights_on ? Vec4(0.96f, 0.99f, 1.00f, 1.0f) : Vec4(0.72f, 0.78f, 0.84f, 1.0f);
    const Vec4 drlIce   = def.headlights_on ? Vec4(0.88f, 0.98f, 1.00f, 1.0f) : Vec4(0.66f, 0.74f, 0.82f, 1.0f);

    // 1. Chassis Rails, Steer & 4x2 Drive Axles, Rear Mudguards, Fifth-Wheel & Alcoa Chrome Hubs
    AppendTractorChassisBase(
        mesh.get(), def, xFront, xRear, frontAxleX, driveCenterX, fifthWheelX,
        wheelR, tireW, hw, cabHW, frameBotY, frameTopY, archTopY, archHalfLen,
        frameDark, glossBlack, linerDark, satinSilver
    );
    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = hw - tireW * 1.06f - 0.025f;
    AppendAlcoaHubAccent(mesh.get(), frontAxleX, wheelR, tireW, frontHalfTrack, def.steer_angle_deg, false, chromeCol, darkTrim);
    AppendAlcoaHubAccent(mesh.get(), driveCenterX, wheelR, tireW, rearHalfTrack, 0.0f, true, chromeCol, darkTrim);

    // Panoramic 2.78:1 Windshield & Door Geometry
    const float windBrowX  = xFront + 0.42f;
    const float windBotY   = 2.34f;
    const float windTopY   = std::min(cabTopY - 0.58f, windBotY + 0.82f); // 3.16m -> 2.78:1 aspect
    const float doorX0     = xFront + 0.48f;
    const float doorX1     = cabBackX - 0.72f;
    const float doorTopY   = windTopY + 0.04f;
    const float doorBeltY  = windBotY - 0.02f;

    auto lowerSurf = [&](float y, float z) {
        return AeroLowerFrontX(y, z, xFront, cabHW, splitterBotY, windBotY);
    };
    auto roofSurf = [&](float y, float z) {
        return AeroRoofBrowX(y, z, windBrowX, cabBackX, windTopY + 0.11f, cabTopY, cabHW);
    };

    // 2. Bespoke Volvo FH16 Cockpit Interior (Neon-green ceiling halo, quilted seats, visible wheel rim)
    CabComponents::AppendVolvoCabInterior(
        mesh.get(), xFront + 0.34f, cabBackX - 0.08f,
        cabClearY + 0.04f, windBotY, cabTopY - 0.16f,
        cabHW, true, def.is_rhd, def.steer_angle_deg, def.headlights_on,
        dashDark, interiorSeat, chromeCol, neonGreen
    );

    // 3. Smooth Curved Front Lower Fascia Grid & Wrap-Around Bumper Side Aprons (ZERO cone facets!)
    const float bumperEndX = frontAxleX - archHalfLen + 0.02f;
    const float upGrBot = 1.13f, upGrTop = 2.04f;
    const float lowGrBot = 0.45f, lowGrTop = 1.07f;
    const float upGrHWBot = 0.72f, upGrHWTop = 0.80f;
    const float lowGrHWBot = 0.62f, lowGrHWTop = 0.70f;

    // 3A. Gloss-Black Chin Splitter (0.23..0.265m) + Olive Lower Chin Bar (0.265..0.30m) + Corner Winglets
    AppendFlushFrontStripX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, splitterBotY, splitterBotY + 0.035f, -cabHW + 0.04f, cabHW - 0.04f, 0.010f, 0.16f, 12, glossBlack);
    AppendCurvedSurfPatchX(mesh.get(), splitterBotY + 0.035f, 0.30f, -cabHW + 0.08f, cabHW - 0.08f, 1, 14, 0.0f, lowerSurf, olivePaint);
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zA = s * (cabHW - 0.10f), zB = s * (cabHW + 0.015f);
        RunningGear::AppendBox(
            mesh.get(),
            Vec3(xFront + 0.10f, splitterBotY, std::min(zA, zB)),
            Vec3(bumperEndX - 0.04f, bumperBotY + 0.055f, std::max(zA, zB)),
            glossBlack
        );
        AppendFlushFrontStripX(
            mesh.get(), xFront, cabHW, splitterBotY, windBotY,
            bumperBotY - 0.008f, bumperBotY + 0.008f,
            std::min(zA, zB), std::max(zA, zB), 0.012f, 0.03f, 2, satinSilver
        );
    }

    // 3B. Dark Recessed Grille/Lamp/Duct Pockets (8mm behind outer olive shell)
    AppendCurvedSurfPatchX(mesh.get(), 0.295f, lowGrBot, -0.66f, 0.66f, 2, 10, 0.010f, lowerSurf, darkBacking);
    AppendCurvedSurfPatchX(mesh.get(), lowGrBot, lowGrTop, -0.72f, 0.72f, 3, 12, 0.008f, lowerSurf, darkBacking);
    AppendCurvedSurfPatchX(mesh.get(), upGrBot, upGrTop, -0.82f, 0.82f, 4, 14, 0.008f, lowerSurf, darkBacking);
    // Corner headlamp dark recessed housings [0.30..0.95m]
    for (int side : {-1, 1}) {
        float z0 = (side > 0) ? 0.70f : -1.06f;
        float z1 = (side > 0) ? 1.06f : -0.70f;
        AppendCurvedSurfPatchX(mesh.get(), 0.30f, 0.95f, z0, z1, 3, 4, 0.007f, lowerSurf, darkBacking);
    }

    // 3C. Tapered Olive Metallic Painted Front Skin Tightly Hugging Both Grilles & Corner Headlamps
    // Outer cheeks next to headlamps & lower bumper [0.30 .. 0.95m]
    AppendCurvedSurfPatchX(mesh.get(), 0.30f, 0.95f, -cabHW + 0.08f, -1.04f, 3, 2, 0.0f, lowerSurf, olivePaint);
    AppendCurvedSurfPatchX(mesh.get(), 0.30f, 0.95f,  1.04f,  cabHW - 0.08f, 3, 2, 0.0f, lowerSurf, olivePaint);
    // Tapered pillars between lower brake ducts/grille and corner headlamps [0.30 .. 1.07m]
    AppendCurvedTaperedPatchX(mesh.get(), 0.30f, 0.45f, -0.72f, -0.70f, -0.61f, -0.62f, 2, 2, 0.0f, lowerSurf, olivePaint);
    AppendCurvedTaperedPatchX(mesh.get(), 0.30f, 0.45f,  0.61f,  0.62f,  0.70f,  0.72f, 2, 2, 0.0f, lowerSurf, olivePaint);
    AppendCurvedTaperedPatchX(mesh.get(), 0.45f, 0.95f, -0.71f, -0.74f, -0.62f, -0.69f, 3, 2, 0.0f, lowerSurf, olivePaint);
    AppendCurvedTaperedPatchX(mesh.get(), 0.45f, 0.95f,  0.62f,  0.69f,  0.71f,  0.74f, 3, 2, 0.0f, lowerSurf, olivePaint);
    // Olive painted bumper shoulder above corner headlamps [0.94 .. 1.07m]
    AppendCurvedTaperedPatchX(mesh.get(), 0.94f, 1.07f, -cabHW + 0.08f, -cabHW + 0.08f, -0.68f, -0.70f, 2, 4, 0.0f, lowerSurf, olivePaint);
    AppendCurvedTaperedPatchX(mesh.get(), 0.94f, 1.07f,  0.68f, 0.70f, cabHW - 0.08f, cabHW - 0.08f, 2, 4, 0.0f, lowerSurf, olivePaint);
    // Slim Horizontal Olive Bumper Divider Bar [1.06 .. 1.14m] separating lower & upper grille
    AppendCurvedSurfPatchX(mesh.get(), 1.06f, 1.14f, -cabHW + 0.08f, cabHW - 0.08f, 2, 14, 0.0f, lowerSurf, olivePaint);
    // Tapered Upper Olive Painted Cheeks tightly hugging upper 5-slat grille [1.13 .. 2.04m]
    AppendCurvedTaperedPatchX(mesh.get(), 1.13f, 2.04f, -cabHW + 0.08f, -cabHW + 0.08f, -upGrHWBot, -upGrHWTop, 5, 4, 0.0f, lowerSurf, olivePaint);
    AppendCurvedTaperedPatchX(mesh.get(), 1.13f, 2.04f,  upGrHWBot,  upGrHWTop, cabHW - 0.08f, cabHW - 0.08f, 5, 4, 0.0f, lowerSurf, olivePaint);
    // Olive Metallic Upper Hood Cowl Band [2.04 .. 2.24] directly above "800" + slim black wiper strip [2.24 .. 2.34]
    AppendCurvedSurfPatchX(mesh.get(), 2.04f, 2.24f, -cabHW + 0.08f, cabHW - 0.08f, 2, 14, 0.0f, lowerSurf, olivePaint);
    AppendCurvedSurfPatchX(mesh.get(), 2.24f, windBotY, -cabHW + 0.08f, cabHW - 0.08f, 2, 14, 0.002f, lowerSurf, glossBlack);

    // 3D. Curved Corner Wrap-Around Side Aprons connecting front edges (|Z| = cabHW - 0.08 -> cabHW) to truck flanks
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        bool rightSide = (side > 0);
        float zInner = s * (cabHW - 0.08f);
        float zOuter = s * cabHW;
        float xSideStart = xFront + 0.24f;
        const int cySegs = 6;
        for (int iy = 0; iy < cySegs; ++iy) {
            float ya = bumperBotY + (windBotY - bumperBotY) * (static_cast<float>(iy) / static_cast<float>(cySegs));
            float yb = bumperBotY + (windBotY - bumperBotY) * (static_cast<float>(iy + 1) / static_cast<float>(cySegs));
            Vec4 col = (yb > 2.24f) ? glossBlack : olivePaint;
            float xa = lowerSurf(ya, zInner);
            float xb = lowerSurf(yb, zInner);
            if (rightSide) {
                PushQuad(mesh.get(), Vec3(xa, ya, zInner), Vec3(xSideStart, ya, zOuter), Vec3(xSideStart, yb, zOuter), Vec3(xb, yb, zInner), col);
            } else {
                PushQuad(mesh.get(), Vec3(xSideStart, ya, zOuter), Vec3(xa, ya, zInner), Vec3(xb, yb, zInner), Vec3(xSideStart, yb, zOuter), col);
            }
        }

        // Bumper side apron panel terminating strictly ahead of front wheel well [xSideStart .. bumperEndX]
        float zBox0 = rightSide ? (cabHW - 0.06f) : -cabHW;
        float zBox1 = rightSide ? cabHW : -(cabHW - 0.06f);
        RunningGear::AppendBox(mesh.get(), Vec3(xSideStart, bumperBotY, zBox0), Vec3(bumperEndX, cabClearY + 0.04f, zBox1), olivePaint);
        // Upper cab A-pillar cheek [xSideStart .. doorX0, cabClearY .. windBotY]
        RunningGear::AppendBox(mesh.get(), Vec3(xSideStart, cabClearY + 0.02f, zBox0), Vec3(doorX0 + 0.04f, windBotY, zBox1), olivePaint);
        // Subtle interior backing behind front grille bay so cabin stays sealed
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.18f, bumperBotY + 0.06f, -cabHW + 0.08f), Vec3(xFront + 0.34f, windBotY - 0.02f, cabHW - 0.08f), frameDark);
    }

    // Painted Olive Wheel-Arch Brow Flares + Semicircular Front Wheel-Well Cutouts & Entry Steps
    for (int side : {-1, 1}) {
        bool rightSide = (side > 0);
        float sideZ = rightSide ? cabHW : -cabHW;
        CabComponents::AppendSemicircularWheelArchSkirt(
            mesh.get(), frontAxleX, wheelR, wheelR + 0.068f, archHalfLen,
            bumperBotY, cabClearY + 0.04f, sideZ, rightSide, 0.045f, olivePaint, glossBlack
        );
        CabComponents::AppendWheelArchFender(
            mesh.get(), frontAxleX, wheelR, wheelR + 0.072f, sideZ, rightSide, 0.048f, olivePaint
        );
    }
    const float stepX0 = frontAxleX + archHalfLen;
    const float stepX1 = cabBackX - 0.04f;
    AppendCabEntrySteps(mesh.get(), stepX0, std::min(stepX1, doorX1), bumperBotY + 0.06f, cabClearY + 0.02f, cabHW, glossBlack, satinSilver);
    if (cabBackX > doorX1 + 0.08f) {
        RunningGear::AppendBox(mesh.get(), Vec3(doorX1, bumperBotY + 0.06f, -cabHW), Vec3(cabBackX, cabClearY + 0.04f, -cabHW + 0.08f), olivePaint);
        RunningGear::AppendBox(mesh.get(), Vec3(doorX1, bumperBotY + 0.06f,  cabHW - 0.08f), Vec3(cabBackX, cabClearY + 0.04f,  cabHW), olivePaint);
    }

    // 4. Lower Brake Cooling Ducts (with Silver Trim), Radar Plate & 4-Slat Tapered Grille (0.30m .. 1.07m)
    AppendFlushFrontStripX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, 0.305f, 0.425f, -0.23f, 0.23f, 0.004f, 0.012f, 4, glossBlack);
    AppendFlushFrontStripX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, 0.420f, 0.432f, -0.24f, 0.24f, 0.006f, 0.008f, 4, satinSilver);
    AppendFlushFrontStripX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, 0.300f, 0.312f, -0.24f, 0.24f, 0.006f, 0.008f, 4, satinSilver);
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zIn = s * 0.27f, zOut = s * 0.59f;
        float zMin = std::min(zIn, zOut), zMax = std::max(zIn, zOut);
        AppendFlushFrontStripX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, 0.305f, 0.318f, zMin, zMax, 0.006f, 0.008f, 3, satinSilver);
        AppendFlushFrontStripX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, 0.420f, 0.432f, zMin + 0.03f, zMax - 0.03f, 0.006f, 0.008f, 3, satinSilver);
        AppendFlushSlantedLineX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, 0.31f, s * 0.59f, 0.425f, s * 0.56f, 0.007f, 0.006f, 0.010f, satinSilver, 2);
        AppendFlushSlantedLineX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, 0.31f, s * 0.27f, 0.425f, s * 0.30f, 0.007f, 0.006f, 0.010f, satinSilver, 2);
    }

    AppendFlushTaperedGrille(
        mesh.get(), xFront, cabHW, splitterBotY, windBotY,
        lowGrBot + 0.01f, lowGrTop - 0.01f, lowGrHWBot, lowGrHWTop,
        4, glossBlack, slatHighlight, darkTrim
    );

    // 5. Upper 5-Slat Tapered Grille (1.14m .. 2.03m) + Centered Iron Mark + "800" Badge
    AppendFlushTaperedGrille(
        mesh.get(), xFront, cabHW, splitterBotY, windBotY,
        upGrBot + 0.01f, upGrTop - 0.01f, upGrHWBot, upGrHWTop,
        5, glossBlack, slatHighlight, darkTrim
    );

    // Centered Volvo Iron Mark Circular Badge
    const float badgeY = 1.59f;
    float badgeX = lowerSurf(badgeY, 0.0f);
    AppendFrontDiscX(mesh.get(), badgeX - 0.008f, badgeY, 0.0f, 0.118f, 0.016f, 24, glossBlack, chromeCol);
    AppendFrontDiscX(mesh.get(), badgeX - 0.014f, badgeY, 0.0f, 0.095f, 0.010f, 24, darkBacking, chromeCol);
    AppendFlushSlantedLineX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, badgeY - 0.105f, -0.105f, badgeY + 0.115f, 0.115f, 0.014f, 0.022f, 0.014f, chromeCol, 3);
    AppendFlushFrontStripX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, badgeY - 0.022f, badgeY + 0.022f, -0.112f, 0.112f, 0.024f, 0.012f, 2, chromeCol);
    AppendFlushFrontStripX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, badgeY - 0.009f, badgeY + 0.009f, -0.090f, 0.090f, 0.026f, 0.008f, 2, glossBlack);

    // 3D Chrome "800" Badge on upper +Z (viewer right) top slat of upper grille
    AppendEmblemDigit(mesh.get(), '8', xFront, cabHW, splitterBotY, windBotY, 1.95f, 0.48f, 0.064f, 0.042f, 0.011f, 0.016f, chromeCol);
    AppendEmblemDigit(mesh.get(), '0', xFront, cabHW, splitterBotY, windBotY, 1.95f, 0.54f, 0.064f, 0.042f, 0.011f, 0.016f, chromeCol);
    AppendEmblemDigit(mesh.get(), '0', xFront, cabHW, splitterBotY, windBotY, 1.95f, 0.60f, 0.064f, 0.042f, 0.011f, 0.016f, chromeCol);

    // 6. Flush Curved Corner LED Headlamp Assemblies (Upper circular projector pod + 7-segment DRL blade)
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float podY = 0.85f;
        float podZ = s * 0.82f;
        float podX = lowerSurf(podY, podZ) - 0.005f;
        AppendCircularLampPodX(mesh.get(), podX, podY, podZ, 0.082f, 0.052f, 18, glossBlack, chromeCol, ledWhite);

        // Tall vertical 7-segment beaded LED DRL blade descending along outer corner
        for (int b = 0; b < 7; ++b) {
            float tb = static_cast<float>(b) / 6.0f;
            float by0 = 0.74f - tb * 0.39f;
            float bzCenter = s * (0.90f - tb * 0.14f);
            AppendFlushFrontStripX(
                mesh.get(), xFront, cabHW, splitterBotY, windBotY,
                by0 - 0.019f, by0 + 0.019f,
                bzCenter - 0.026f, bzCenter + 0.026f,
                0.008f, 0.014f, 2, drlIce
            );
        }
        // Subtle silver lower outer trim line hugging lamp bottom corner + amber outer marker
        AppendFlushSlantedLineX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, 0.33f, s * 0.72f, 0.46f, s * 1.02f, 0.007f, 0.006f, 0.010f, satinSilver, 3);
        float ambZ0 = (side > 0) ? 0.96f : -1.03f;
        float ambZ1 = (side > 0) ? 1.03f : -0.96f;
        AppendFlushFrontStripX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, 0.56f, 0.63f, ambZ0, ambZ1, 0.007f, 0.012f, 2, amberLed);
    }

    // Dual parked wipers right on slim black cowl strip [2.24 .. 2.34]
    AppendFlushFrontStripX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, windBotY - 0.025f, windBotY + 0.010f, -0.92f, -0.06f, 0.012f, 0.018f, 4, glossBlack);
    AppendFlushFrontStripX(mesh.get(), xFront, cabHW, splitterBotY, windBotY, windBotY - 0.025f, windBotY + 0.010f,  0.06f,  0.92f, 0.012f, 0.018f, 4, glossBlack);

    // 7. Sleek Curved A-Pillars, Solid Flush Sunvisor & Airfoil Globetrotter XL Roof Dome
    const int pSegs = 6;
    const float pillarInZ = cabHW - 0.095f;
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        bool posSide = (side > 0);
        float zIn = s * pillarInZ;
        float zOut = s * cabHW;
        float zA = std::min(zIn, zOut), zB = std::max(zIn, zOut);
        for (int i = 0; i < pSegs; ++i) {
            float t0 = static_cast<float>(i) / static_cast<float>(pSegs);
            float t1 = static_cast<float>(i + 1) / static_cast<float>(pSegs);
            float y0 = (windBotY - 0.01f) + ((windTopY + 0.02f) - (windBotY - 0.01f)) * t0;
            float y1 = (windBotY - 0.01f) + ((windTopY + 0.02f) - (windBotY - 0.01f)) * t1;
            float xF0 = AeroLowerFrontX(windBotY, zIn, xFront, cabHW, splitterBotY, windBotY) * (1.0f - t0) + (windBrowX + AeroFrontLatSweep(zIn, cabHW, 0.092f)) * t0;
            float xF1 = AeroLowerFrontX(windBotY, zIn, xFront, cabHW, splitterBotY, windBotY) * (1.0f - t1) + (windBrowX + AeroFrontLatSweep(zIn, cabHW, 0.092f)) * t1;
            float xOut0 = xF0 + 0.055f, xOut1 = xF1 + 0.055f;
            float xB0 = xF0 + 0.24f, xB1 = xF1 + 0.24f;
            float xFL0 = posSide ? xF0 : xOut0, xFR0 = posSide ? xOut0 : xF0;
            float xFL1 = posSide ? xF1 : xOut1, xFR1 = posSide ? xOut1 : xF1;
            Vec3 f0(xFL0, y0, zA), f1(xFR0, y0, zB), f2(xFR1, y1, zB), f3(xFL1, y1, zA);
            Vec3 b0(xB0,  y0, zA), b1(xB0,  y0, zB), b2(xB1,  y1, zB), b3(xB1,  y1, zA);
            PushQuad(mesh.get(), f0, f1, f2, f3, glossBlack);
            PushQuad(mesh.get(), b1, b0, b3, b2, glossBlack);
            PushQuad(mesh.get(), b0, f0, f3, b3, glossBlack);
            PushQuad(mesh.get(), f1, b1, b2, f2, glossBlack);
        }
    }

    // Flush Curved Solid Gloss-Black Sunvisor Band [windTopY - 0.01 .. windTopY + 0.12]
    const int visSegs = 14;
    for (int iz = 0; iz < visSegs; ++iz) {
        float tz0 = static_cast<float>(iz) / static_cast<float>(visSegs);
        float tz1 = static_cast<float>(iz + 1) / static_cast<float>(visSegs);
        float za = (-cabHW + 0.06f) + 2.0f * (cabHW - 0.06f) * tz0;
        float zb = (-cabHW + 0.06f) + 2.0f * (cabHW - 0.06f) * tz1;
        float xBotF_a = windBrowX + AeroFrontLatSweep(za, cabHW, 0.092f) - 0.024f;
        float xBotF_b = windBrowX + AeroFrontLatSweep(zb, cabHW, 0.092f) - 0.024f;
        float xTopF_a = windBrowX + 0.014f + AeroFrontLatSweep(za, cabHW, 0.092f) - 0.014f;
        float xTopF_b = windBrowX + 0.014f + AeroFrontLatSweep(zb, cabHW, 0.092f) - 0.014f;
        float xBack_a = windBrowX + 0.045f + AeroFrontLatSweep(za, cabHW, 0.092f);
        float xBack_b = windBrowX + 0.045f + AeroFrontLatSweep(zb, cabHW, 0.092f);
        float yb0 = windTopY - 0.01f, yb1 = windTopY + 0.12f;
        PushQuad(mesh.get(), Vec3(xBotF_a, yb0, za), Vec3(xBotF_b, yb0, zb), Vec3(xTopF_b, yb1, zb), Vec3(xTopF_a, yb1, za), glossBlack);
        PushQuad(mesh.get(), Vec3(xTopF_a, yb1, za), Vec3(xTopF_b, yb1, zb), Vec3(xBack_b, yb1, zb), Vec3(xBack_a, yb1, za), glossBlack);
        PushQuad(mesh.get(), Vec3(xBack_a, yb0, za), Vec3(xBack_b, yb0, zb), Vec3(xBotF_b, yb0, zb), Vec3(xBotF_a, yb0, za), glossBlack);
    }
    // Wrap sunvisor outer tips cleanly into side cab header
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(
            mesh.get(),
            Vec3(windBrowX + 0.05f, windTopY - 0.01f, s > 0 ? cabHW - 0.06f : -cabHW - 0.005f),
            Vec3(windBrowX + 0.24f, windTopY + 0.12f, s > 0 ? cabHW + 0.005f : -cabHW + 0.06f),
            glossBlack
        );
    }

    // Olive Side Header Strips closing above doorTopY [xFront + 0.62f .. cabBackX - 0.04f]
    RunningGear::AppendBox(mesh.get(), Vec3(windBrowX + 0.22f, doorTopY - 0.01f, -cabHW), Vec3(cabBackX - 0.04f, windTopY + 0.12f, -cabHW + 0.08f), olivePaint);
    RunningGear::AppendBox(mesh.get(), Vec3(windBrowX + 0.22f, doorTopY - 0.01f,  cabHW - 0.08f), Vec3(cabBackX - 0.04f, windTopY + 0.12f,  cabHW), olivePaint);

    // Smooth Airfoil Globetrotter XL Roof Brow Dome [windTopY + 0.11 .. cabTopY]
    AppendCurvedSurfPatchX(
        mesh.get(), windTopY + 0.11f, cabTopY, -cabHW + 0.10f, cabHW - 0.10f,
        7, 14, 0.0f, roofSurf, olivePaint
    );
    // Smooth corner wrap from roof brow front edges back into cab sidewalls
    for (int side : {-1, 1}) {
        bool rightSide = (side > 0);
        float s = static_cast<float>(side);
        float zInner = s * (cabHW - 0.10f);
        float zOuter = s * cabHW;
        const int rySegs = 6;
        for (int iy = 0; iy < rySegs; ++iy) {
            float ya = (windTopY + 0.11f) + (cabTopY - (windTopY + 0.11f)) * (static_cast<float>(iy) / static_cast<float>(rySegs));
            float yb = (windTopY + 0.11f) + (cabTopY - (windTopY + 0.11f)) * (static_cast<float>(iy + 1) / static_cast<float>(rySegs));
            float xa = roofSurf(ya, zInner);
            float xb = roofSurf(yb, zInner);
            float xSideA = xa + 0.14f, xSideB = xb + 0.14f;
            if (rightSide) {
                PushQuad(mesh.get(), Vec3(xa, ya, zInner), Vec3(xSideA, ya, zOuter), Vec3(xSideB, yb, zOuter), Vec3(xb, yb, zInner), olivePaint);
            } else {
                PushQuad(mesh.get(), Vec3(xSideA, ya, zOuter), Vec3(xa, ya, zInner), Vec3(xb, yb, zInner), Vec3(xSideB, yb, zOuter), olivePaint);
            }
            float zWall0 = rightSide ? (cabHW - 0.06f) : -cabHW;
            float zWall1 = rightSide ? cabHW : -(cabHW - 0.06f);
            RunningGear::AppendBox(mesh.get(), Vec3(xSideB, ya, zWall0), Vec3(cabBackX - 0.04f, yb + 0.01f, zWall1), olivePaint);
        }
    }

    // Sleek Subtle V-Crease Shadow Groove across front olive brow (hugging roofSurf 6mm proud)
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        const int vSegs = 6;
        for (int i = 0; i < vSegs; ++i) {
            float t0 = static_cast<float>(i) / static_cast<float>(vSegs);
            float t1 = static_cast<float>(i + 1) / static_cast<float>(vSegs);
            float za = s * (cabHW - 0.12f) * t0;
            float zb = s * (cabHW - 0.12f) * t1;
            float ya = windTopY + 0.25f + 0.16f * (t0 * t0 * 0.4f + t0 * 0.6f);
            float yb = windTopY + 0.25f + 0.16f * (t1 * t1 * 0.4f + t1 * 0.6f);
            float xa = roofSurf(ya, za) - 0.006f;
            float xb = roofSurf(yb, zb) - 0.006f;
            float zL = std::min(za, zb), zR = std::max(za, zb);
            float xL = (za <= zb) ? xa : xb, xR = (za <= zb) ? xb : xa;
            float yL = (za <= zb) ? ya : yb, yR = (za <= zb) ? yb : ya;
            PushQuad(mesh.get(), Vec3(xL, yL - 0.007f, zL), Vec3(xR, yR - 0.007f, zR), Vec3(xR, yR + 0.007f, zR), Vec3(xL, yL + 0.007f, zL), glossBlack);
        }
    }

    // Flush Curved Roof Top Deck: Olive Outer Side Shoulders flanking Gloss-Black Center Roof Scoop
    // Leading edge tracks roofSurf(cabTopY, z) smoothly across Z so corners never overhang!
    const float roofEndX = cabBackX - 0.06f;
    const float roofCenterZ = 0.64f;
    const int rxSegs = 5;
    const int rzSegs = 12;
    for (int iz = 0; iz < rzSegs; ++iz) {
        float tz0 = static_cast<float>(iz) / static_cast<float>(rzSegs);
        float tz1 = static_cast<float>(iz + 1) / static_cast<float>(rzSegs);
        float z0 = -(cabHW - 0.08f) + 2.0f * (cabHW - 0.08f) * tz0;
        float z1 = -(cabHW - 0.08f) + 2.0f * (cabHW - 0.08f) * tz1;
        float zMid = 0.5f * (z0 + z1);
        Vec4 deckCol = (std::abs(zMid) <= roofCenterZ) ? glossBlack : olivePaint;
        float xLead0 = roofSurf(cabTopY, z0);
        float xLead1 = roofSurf(cabTopY, z1);
        float yLift  = (std::abs(zMid) <= roofCenterZ) ? 0.0f : 0.012f;
        for (int ix = 0; ix < rxSegs; ++ix) {
            float tx0 = static_cast<float>(ix) / static_cast<float>(rxSegs);
            float tx1 = static_cast<float>(ix + 1) / static_cast<float>(rxSegs);
            float x0a = xLead0 + (roofEndX - xLead0) * tx0;
            float x0b = xLead1 + (roofEndX - xLead1) * tx0;
            float x1a = xLead0 + (roofEndX - xLead0) * tx1;
            float x1b = xLead1 + (roofEndX - xLead1) * tx1;
            float archY0 = cabTopY + yLift + 0.028f * std::sin(tx0 * 3.14159f);
            float archY1 = cabTopY + yLift + 0.028f * std::sin(tx1 * 3.14159f);
            PushQuad(mesh.get(), Vec3(x0a, archY0, z0), Vec3(x0b, archY0, z1), Vec3(x1b, archY1, z1), Vec3(x1a, archY1, z0), deckCol);
        }
    }

    // Swept Gloss-Black Rear Corner Roof Aero Wings (ramped wedge on ±Z only, clear center) + Twin Whip Antennas
    const float wingStartX = cabBackX - 0.86f;
    const float wingEndX   = cabBackX - 0.06f;
    const float wingTopY   = std::min(H, cabTopY + 0.095f);
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zA = (side > 0) ? 0.54f : -(cabHW - 0.09f);
        float zB = (side > 0) ? (cabHW - 0.09f) : -0.54f;
        float yBase = cabTopY - 0.01f;
        float yFrontTop = cabTopY + 0.025f;
        Vec3 fBL(wingStartX, yBase, zA), fBR(wingStartX, yBase, zB);
        Vec3 fTR(wingStartX, yFrontTop, zB), fTL(wingStartX, yFrontTop, zA);
        Vec3 bBL(wingEndX,   yBase, zA), bBR(wingEndX,   yBase, zB);
        Vec3 bTR(wingEndX,   wingTopY, zB), bTL(wingEndX,   wingTopY, zA);
        PushQuad(mesh.get(), fBL, fBR, fTR, fTL, glossBlack); // -X front
        PushQuad(mesh.get(), bBR, bBL, bTL, bTR, glossBlack); // +X rear
        PushQuad(mesh.get(), fTL, fTR, bTR, bTL, glossBlack); // +Y top
        PushQuad(mesh.get(), bBL, fBL, fTL, bTL, glossBlack); // -Z left
        PushQuad(mesh.get(), fBR, bBR, bTR, fTR, glossBlack); // +Z right

        // Twin slim antennas rising from front black roof shoulders
        float antX = roofSurf(cabTopY, s * 0.62f) + 0.14f;
        ChassisComponents::AppendClosedCylinderY(mesh.get(), antX, s * 0.62f, cabTopY - 0.01f, cabTopY + 0.34f, 0.010f, 8, glossBlack);
    }

    // Carved rounded rear sleeper walls behind door [doorX1 .. cabBackX]
    std::vector<HullStationX> rearSleeper = {
        {doorX1,           cabClearY, cabTopY - 0.04f, cabHW,         0.10f, 0.12f, 0.0f, 0.0f},
        {cabBackX - 0.12f, cabClearY, cabTopY - 0.04f, cabHW,         0.10f, 0.14f, 0.0f, 0.0f},
        {cabBackX,         cabClearY + 0.04f, cabTopY - 0.04f, cabHW - 0.06f, 0.12f, 0.16f, 0.0f, 0.0f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), rearSleeper, olivePaint, 5);

    // Cab side rear extender blades & black accent molding behind door
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.04f, cabClearY + 0.04f, s > 0 ? cabHW - 0.05f : -cabHW), Vec3(cabBackX + 0.24f, windTopY + 0.02f, s > 0 ? cabHW : -cabHW + 0.05f), olivePaint);
        RunningGear::AppendBox(mesh.get(), Vec3(doorX1 + 0.03f, cabClearY + 0.50f, s > 0 ? cabHW - 0.01f : -cabHW - 0.012f), Vec3(cabBackX - 0.06f, cabClearY + 0.56f, s > 0 ? cabHW + 0.012f : -cabHW + 0.01f), glossBlack);
    }

    // 8. Kerb-Dipped FH Doors + Slim Twin-Arm Aero Wing Mirrors
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.16f, false, false, olivePaint, glossBlack, sideGlass, doorBeltY, def.is_rhd, true);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.16f, false, false, olivePaint, glossBlack, sideGlass, doorBeltY, !def.is_rhd, true);
    AppendVolvoWingMirrors(mesh.get(), xFront + 0.52f, windBotY + 0.30f, windTopY - 0.02f, cabHW, glossBlack, satinSilver, chromeCol);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.38f, windTopY - 0.10f, -cabHW, glossBlack);
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.46f, windTopY + 0.06f, cabHW * 0.85f, def.headlights_on);

    // 9. Flush Olive Aero Side Skirts [cabBackX + 0.10f .. driveCenterX - archHalfLen] + Rear Wheel Eyebrows
    float skirtStart = cabBackX + 0.12f;
    float skirtEnd   = std::max(skirtStart + 0.55f, driveCenterX - archHalfLen - 0.06f);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, bumperBotY + 0.05f, -hw + 0.03f), Vec3(skirtEnd, frameTopY + 0.04f, -hw + 0.10f), olivePaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, bumperBotY + 0.05f,  hw - 0.10f), Vec3(skirtEnd, frameTopY + 0.04f,  hw - 0.03f), olivePaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, bumperBotY + 0.03f, -hw + 0.025f), Vec3(skirtEnd, bumperBotY + 0.09f, -hw + 0.05f), glossBlack);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, bumperBotY + 0.03f,  hw - 0.05f),  Vec3(skirtEnd, bumperBotY + 0.09f,  hw - 0.025f), glossBlack);
    CabComponents::AppendWheelArchFender(mesh.get(), driveCenterX, wheelR, wheelR + 0.08f, -hw, false, tireW * 1.15f, olivePaint);
    CabComponents::AppendWheelArchFender(mesh.get(), driveCenterX, wheelR, wheelR + 0.08f,  hw, true,  tireW * 1.15f, olivePaint);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.10f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.66f, frameTopY + 0.04f, 0.45f), satinSilver);

    // 10. Translucent Curved Flush Glass Pass (matching AeroLowerFrontX & sunvisor brow seamlessly!)
    const int gSegs = 12;
    const float gHW = cabHW - 0.095f;
    for (int i = 0; i < gSegs; ++i) {
        float za = -gHW + (2.0f * gHW) * (static_cast<float>(i) / static_cast<float>(gSegs));
        float zb = -gHW + (2.0f * gHW) * (static_cast<float>(i + 1) / static_cast<float>(gSegs));
        float xBotA = lowerSurf(windBotY, za) + 0.008f;
        float xBotB = lowerSurf(windBotY, zb) + 0.008f;
        float xTopA = windBrowX + AeroFrontLatSweep(za, cabHW, 0.092f) + 0.006f;
        float xTopB = windBrowX + AeroFrontLatSweep(zb, cabHW, 0.092f) + 0.006f;
        PushQuad(mesh.get(), Vec3(xBotA, windBotY - 0.01f, za), Vec3(xBotB, windBotY - 0.01f, zb), Vec3(xTopB, windTopY + 0.01f, zb), Vec3(xTopA, windTopY + 0.01f, za), windshieldGlass);
    }
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.16f, false, true, olivePaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.16f, false, true, olivePaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
