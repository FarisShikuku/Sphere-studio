#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace sphere::vehicles::trucks {

namespace {

inline float XxlLatSweep(float z, float cabHW, float maxSweepX) {
    float nz = std::clamp(std::abs(z) / std::max(0.25f, cabHW), 0.0f, 1.0f);
    return maxSweepX * (0.24f * nz + 0.76f * nz * nz);
}

inline float XxlLowerFrontX(float y, float z, float xFront, float cabHW, float splitterBotY, float windBotY) {
    float ty = std::clamp((y - splitterBotY) / std::max(0.2f, windBotY - splitterBotY), 0.0f, 1.0f);
    return xFront + 0.130f * ty + XxlLatSweep(z, cabHW, 0.095f);
}

inline float XxlRoofBrowX(float y, float z, float windBrowX, float windTopY, float cabTopY, float cabHW) {
    float tr = std::clamp((y - windTopY) / std::max(0.2f, cabTopY - windTopY), 0.0f, 1.0f);
    float archBack = 0.035f * tr + 0.42f * tr * tr * tr;
    return windBrowX + archBack + XxlLatSweep(z, cabHW, 0.095f) * (1.0f - 0.28f * tr * tr);
}

template <typename SurfFn>
void AppendXxlPatchX(
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
            PushQuad(
                mesh,
                Vec3(surfX(ya, za) + depthOffsetX, ya, za),
                Vec3(surfX(ya, zb) + depthOffsetX, ya, zb),
                Vec3(surfX(yb, zb) + depthOffsetX, yb, zb),
                Vec3(surfX(yb, za) + depthOffsetX, yb, za),
                col
            );
        }
    }
}

template <typename SurfFn>
void AppendXxlTaperedPatchX(
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
            PushQuad(
                mesh,
                Vec3(surfX(ya, za0) + depthOffsetX, ya, za0),
                Vec3(surfX(ya, za1) + depthOffsetX, ya, za1),
                Vec3(surfX(yb, zb1) + depthOffsetX, yb, zb1),
                Vec3(surfX(yb, zb0) + depthOffsetX, yb, zb0),
                col
            );
        }
    }
}

template <typename SurfFn>
void AppendXxlStripX(
    Mesh* mesh,
    float yBot, float yTop,
    float zLeft, float zRight,
    float proudFrontX, float depthBackX,
    int segsZ,
    SurfFn&& surfX,
    const Vec4& col
) {
    if (!mesh || zRight <= zLeft + 1e-3f || yTop <= yBot + 1e-4f) return;
    int n = std::max(1, segsZ);
    for (int i = 0; i < n; ++i) {
        float t0 = static_cast<float>(i) / static_cast<float>(n);
        float t1 = static_cast<float>(i + 1) / static_cast<float>(n);
        float z0 = zLeft + (zRight - zLeft) * t0;
        float z1 = zLeft + (zRight - zLeft) * t1;
        float xF00 = surfX(yBot, z0) - proudFrontX;
        float xF01 = surfX(yBot, z1) - proudFrontX;
        float xF11 = surfX(yTop, z1) - proudFrontX;
        float xF10 = surfX(yTop, z0) - proudFrontX;
        Vec3 fBL(xF00, yBot, z0), fBR(xF01, yBot, z1), fTR(xF11, yTop, z1), fTL(xF10, yTop, z0);
        Vec3 bBL(xF00 + depthBackX, yBot, z0), bBR(xF01 + depthBackX, yBot, z1);
        Vec3 bTR(xF11 + depthBackX, yTop, z1), bTL(xF10 + depthBackX, yTop, z0);
        PushQuad(mesh, fBL, fBR, fTR, fTL, col);
        PushQuad(mesh, fTL, fTR, bTR, bTL, col);
        PushQuad(mesh, bBL, bBR, fBR, fBL, col);
        if (i == 0)     PushQuad(mesh, bBL, fBL, fTL, bTL, col);
        if (i == n - 1) PushQuad(mesh, fBR, bBR, bTR, fTR, col);
    }
}

template <typename SurfFn>
void AppendXxlLineX(
    Mesh* mesh,
    float y0, float z0,
    float y1, float z1,
    float halfThick, float proudFrontX, float depthBackX,
    SurfFn&& surfX,
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
        float xFa = surfX(ya, za) - proudFrontX;
        float xFb = surfX(yb, zb) - proudFrontX;
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

template <typename SurfFn>
void AppendVolvoBrowGlyph(
    Mesh* mesh, char ch,
    float yBot, float zLeft, float h, float w, float proudX,
    SurfFn&& surfX, const Vec4& col
) {
    auto box = [&](float yn0, float yn1, float zn0, float zn1) {
        AppendXxlStripX(mesh, yBot + h * yn0, yBot + h * yn1, zLeft + w * zn0, zLeft + w * zn1, proudX, 0.014f, 1, surfX, col);
    };
    switch (ch) {
        case 'V': box(0.22f, 1.0f, 0.0f, 0.25f); box(0.22f, 1.0f, 0.75f, 1.0f); box(0.0f, 0.26f, 0.26f, 0.74f); break;
        case 'O': box(0.0f, 1.0f, 0.0f, 0.24f); box(0.0f, 1.0f, 0.76f, 1.0f); box(0.76f, 1.0f, 0.24f, 0.76f); box(0.0f, 0.24f, 0.24f, 0.76f); break;
        case 'L': box(0.0f, 1.0f, 0.0f, 0.26f); box(0.0f, 0.24f, 0.26f, 1.0f); break;
        default: break;
    }
}

// All-black multi-spoke performance wheel hubs matching Photo 2
void AppendBlackPerformanceHub(
    Mesh* mesh, float axleX, float wheelR, float tireW, float halfTrack,
    float steerAngleDeg, bool isDualRear, const Vec4& glossBlack, const Vec4& gunmetalRing
) {
    float steerRad = steerAngleDeg * (3.14159265f / 180.0f);
    float cS = std::cos(steerRad), sS = std::sin(steerRad);
    float outerOff = isDualRear ? (tireW * 0.56f) : 0.0f;
    float wheelHW = tireW * 0.5f;
    float dishZ = wheelHW * 0.58f, hubZ = wheelHW * 0.80f;
    float dishR = wheelR * 0.49f, hubR = wheelR * 0.19f;
    int segs = 20;
    for (int side : {-1, 1}) {
        float sideSign = static_cast<float>(side);
        float cz = sideSign * (halfTrack + outerOff);
        auto rotPt = [&](float lx, float ly, float lz) -> Vec3 {
            return Vec3(axleX + lx * cS - lz * sS, wheelR + ly, cz + lx * sS + lz * cS);
        };
        Vec3 cHub = rotPt(0.0f, 0.0f, sideSign * hubZ + sideSign * 0.004f);
        for (int i = 0; i < segs; ++i) {
            float a0 = (static_cast<float>(i) / static_cast<float>(segs)) * 6.2831853f;
            float a1 = (static_cast<float>(i + 1) / static_cast<float>(segs)) * 6.2831853f;
            Vec3 h0 = rotPt(hubR * std::cos(a0), hubR * std::sin(a0), sideSign * hubZ);
            Vec3 h1 = rotPt(hubR * std::cos(a1), hubR * std::sin(a1), sideSign * hubZ);
            Vec3 d0 = rotPt(dishR * std::cos(a0), dishR * std::sin(a0), sideSign * dishZ);
            Vec3 d1 = rotPt(dishR * std::cos(a1), dishR * std::sin(a1), sideSign * dishZ);
            if (side > 0) {
                PushTri(mesh, cHub, h0, h1, glossBlack);
                PushQuad(mesh, h0, d0, d1, h1, (i % 2 == 0) ? glossBlack : gunmetalRing);
            } else {
                PushTri(mesh, cHub, h1, h0, glossBlack);
                PushQuad(mesh, h1, d1, d0, h0, (i % 2 == 0) ? glossBlack : gunmetalRing);
            }
        }
    }
}

void AppendGunmetalWingMirrors(
    Mesh* mesh, float anchorX, float yBot, float yTop, float cabHW,
    const Vec4& gunmetalCol, const Vec4& blackCol, const Vec4& yellowLine
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
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.06f, yHighArm - 0.020f, armZ0), Vec3(anchorX + 0.02f, yHighArm + 0.020f, armZ1), blackCol);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.03f, yLowArm - 0.018f,  armZ0), Vec3(anchorX + 0.04f, yLowArm + 0.018f,  armZ1), blackCol);
        float shZ0 = std::min(zShellIn, zShellOut), shZ1 = std::max(zShellIn, zShellOut);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.085f, yBot, shZ0), Vec3(anchorX + 0.055f, yTop, shZ1), gunmetalCol);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.070f, yTop, shZ0 + 0.012f), Vec3(anchorX + 0.045f, yTop + 0.042f, shZ1 - 0.012f), blackCol);
        RunningGear::AppendBox(mesh, Vec3(anchorX + 0.050f, yBot + 0.035f, shZ0 + 0.016f), Vec3(anchorX + 0.064f, yTop - 0.035f, shZ1 - 0.016f), blackCol);
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.090f, yTop - 0.012f, shZ0 + 0.014f), Vec3(anchorX - 0.078f, yTop + 0.008f, shZ1 - 0.014f), yellowLine);
    }
}

} // namespace

// Photo-matched procedural C++20 3D generator for Volvo FH16 750 XXL 6x4 (Gunmetal Nardo Grey + Racing Yellow):
// - Extended Globetrotter XXL 6x4 tandem drive chassis w/ all-black performance wheels & yellow-accented side skirts
// - Custom aggressive lower aero chin splitter w/ 5-LED center rally auxiliary pod & yellow chevron trims
// - Bold 3-slat sculpted black lower grille + upper black mesh bay featuring 3D yellow winged crest emblem
// - Diagonal yellow hood slash + Volvo circular emblem, tall vertical yellow DRL corner light blades
// - Yellow VOLVO forehead wordmark, black corner brow vents, & 7-lamp circular roof LED spotlight bar
std::shared_ptr<Mesh> GenerateVolvoFh16750XxlMesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 6.2f, 9.2f);
    const float W = std::clamp(def.dimensions.y, 2.40f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.45f, 4.15f);
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

    const float splitterBotY = groundClear - 0.02f;  // 0.22m
    const float bumperBotY   = groundClear + 0.04f;  // 0.28m
    const float cabBaseY     = frameBotY - 0.04f;    // 0.86m
    const float cabClearY    = std::max(cabBaseY + 0.24f, archTopY + 0.02f); // 1.20m
    const float cabHW        = hw - 0.03f;           // 1.24m
    const float cabBackX     = xFront + 2.82f;       // +0.28m EXTENDED GLOBETROTTER XXL SLEEPER
    const float cabTopY      = std::clamp(def.cab_height_m, 3.50f, 4.02f);

    const float frontAxleX   = xFront + 1.44f;
    const float driveCenterX = std::min(xRear - 1.15f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX  = std::clamp(def.fifth_wheel_offset_m, 1.10f, xRear - 0.95f);

    const Vec4 greyPaint   = def.base_color;
    const Vec4 neonYellow  = def.accent_color;
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 darkPocket(0.030f, 0.035f, 0.042f, 1.0f);
    const Vec4 anthracite(0.14f, 0.16f, 0.19f, 1.0f);
    const Vec4 chromeCol(0.88f, 0.91f, 0.96f, 1.0f);
    const Vec4 frameDark(0.10f, 0.12f, 0.14f, 1.0f);
    const Vec4 linerDark(0.04f, 0.045f, 0.055f, 1.0f);
    const Vec4 dashDark(0.10f, 0.11f, 0.13f, 1.0f);
    const Vec4 interiorSeat(0.14f, 0.16f, 0.19f, 1.0f);
    const Vec4 windshieldGlass(0.20f, 0.34f, 0.44f, 0.30f);
    const Vec4 sideGlass(0.16f, 0.28f, 0.38f, 0.46f);
    const Vec4 ledWhite = def.headlights_on ? Vec4(0.97f, 0.99f, 1.00f, 1.0f) : Vec4(0.74f, 0.80f, 0.86f, 1.0f);

    // 1. 6x4 Tandem Chassis + Black Multi-Spoke Hubs on Steer & Tandem Drive Axles
    AppendTractorChassisBase(
        mesh.get(), def, xFront, xRear, frontAxleX, driveCenterX, fifthWheelX,
        wheelR, tireW, hw, cabHW, frameBotY, frameTopY, archTopY, archHalfLen,
        frameDark, glossBlack, linerDark, anthracite
    );
    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = hw - tireW * 1.06f - 0.025f;
    AppendBlackPerformanceHub(mesh.get(), frontAxleX, wheelR, tireW, frontHalfTrack, def.steer_angle_deg, false, glossBlack, anthracite);
    float d0 = driveCenterX - 0.68f, d1 = driveCenterX + 0.68f;
    AppendBlackPerformanceHub(mesh.get(), d0, wheelR, tireW, rearHalfTrack, 0.0f, true, glossBlack, anthracite);
    AppendBlackPerformanceHub(mesh.get(), d1, wheelR, tireW, rearHalfTrack, 0.0f, true, glossBlack, anthracite);

    // Panoramic Windshield & Kerb-Dipped Doors
    const float windBrowX  = xFront + 0.40f;
    const float windBotY   = 2.34f;
    const float windTopY   = std::min(cabTopY - 0.60f, windBotY + 0.80f);
    const float doorX0     = xFront + 0.46f;
    const float doorX1     = xFront + 1.82f;
    const float doorTopY   = windTopY + 0.04f;
    const float doorBeltY  = windBotY - 0.02f;

    auto lowerSurf = [&](float y, float z) {
        return XxlLowerFrontX(y, z, xFront, cabHW, splitterBotY, windBotY);
    };
    auto roofSurf = [&](float y, float z) {
        return XxlRoofBrowX(y, z, windBrowX, windTopY + 0.12f, cabTopY, cabHW);
    };

    // 2. Full LHD / RHD Volvo Cockpit Interior w/ subtle racing-yellow ambient halo
    CabComponents::AppendVolvoCabInterior(
        mesh.get(), xFront + 0.34f, cabBackX - 0.08f,
        cabClearY + 0.04f, windBotY, cabTopY - 0.16f,
        cabHW, true, def.is_rhd, def.steer_angle_deg, def.headlights_on,
        dashDark, interiorSeat, chromeCol, neonYellow
    );

    // 3. Smooth Curved Front Lower Aero Bumper, Yellow Accents & Two-Zone Grille
    const float bumperEndX = frontAxleX - archHalfLen + 0.02f;
    const float lowGrBot = 0.48f, lowGrTop = 1.18f;
    const float upBayBot = 1.22f, upBayTop = 1.72f;
    const float grHWBot  = 0.62f, grHWTop  = 0.78f;

    // 3A. Bold Aero Chin Splitter [0.22..0.28m] + Racing Yellow Lip Pinstripe + Center 5-LED Rally Pod
    AppendXxlStripX(mesh.get(), splitterBotY, bumperBotY, -cabHW + 0.04f, cabHW - 0.04f, 0.012f, 0.18f, 12, lowerSurf, glossBlack);
    AppendXxlStripX(mesh.get(), splitterBotY + 0.008f, splitterBotY + 0.028f, -cabHW + 0.08f, cabHW - 0.08f, 0.014f, 0.04f, 12, lowerSurf, neonYellow);
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zA = s * (cabHW - 0.10f), zB = s * (cabHW + 0.020f);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.10f, splitterBotY, std::min(zA, zB)), Vec3(bumperEndX - 0.03f, bumperBotY + 0.06f, std::max(zA, zB)), glossBlack);
        AppendXxlStripX(mesh.get(), bumperBotY + 0.01f, bumperBotY + 0.03f, std::min(zA, zB), std::max(zA, zB), 0.014f, 0.03f, 2, lowerSurf, neonYellow);
    }

    // Center 5-LED Auxiliary Rally Light Box [0.29..0.41m, ±0.20m] + Yellow Chevron Wings
    AppendXxlPatchX(mesh.get(), bumperBotY, 0.48f, -0.64f, 0.64f, 2, 10, 0.006f, lowerSurf, darkPocket);
    AppendXxlStripX(mesh.get(), 0.295f, 0.415f, -0.21f, 0.21f, 0.006f, 0.014f, 4, lowerSurf, glossBlack);
    AppendXxlStripX(mesh.get(), 0.405f, 0.422f, -0.22f, 0.22f, 0.008f, 0.010f, 4, lowerSurf, chromeCol);
    AppendXxlStripX(mesh.get(), 0.292f, 0.308f, -0.22f, 0.22f, 0.008f, 0.010f, 4, lowerSurf, chromeCol);
    for (int i = -2; i <= 2; ++i) {
        float zLed = static_cast<float>(i) * 0.068f;
        float lX = lowerSurf(0.355f, zLed) - 0.008f;
        AppendFrontDiscX(mesh.get(), lX, 0.355f, zLed, 0.022f, 0.008f, 10, ledWhite, chromeCol);
    }
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        AppendXxlLineX(mesh.get(), 0.30f, s * 0.26f, 0.44f, s * 0.40f, 0.018f, 0.008f, 0.012f, lowerSurf, neonYellow, 3);
        AppendXxlLineX(mesh.get(), 0.44f, s * 0.40f, 0.32f, s * 0.64f, 0.014f, 0.008f, 0.012f, lowerSurf, neonYellow, 3);
    }

    // 3B. Dark Recessed Grille & Corner Headlamp Pockets
    AppendXxlPatchX(mesh.get(), lowGrBot, lowGrTop, -0.78f, 0.78f, 4, 12, 0.010f, lowerSurf, darkPocket);
    AppendXxlPatchX(mesh.get(), upBayBot, upBayTop, -0.80f, 0.80f, 4, 12, 0.010f, lowerSurf, darkPocket);
    for (int side : {-1, 1}) {
        float z0 = (side > 0) ? 0.66f : -1.08f;
        float z1 = (side > 0) ? 1.08f : -0.66f;
        AppendXxlPatchX(mesh.get(), 0.32f, 1.18f, z0, z1, 4, 4, 0.009f, lowerSurf, darkPocket);
    }

    // 3C. Tapered Gunmetal Nardo Grey Front Cheeks + Hood [1.72..2.22m] + Wiper Cowl [2.22..2.34m]
    AppendXxlPatchX(mesh.get(), bumperBotY, 1.18f, -cabHW + 0.08f, -1.04f, 4, 2, 0.0f, lowerSurf, greyPaint);
    AppendXxlPatchX(mesh.get(), bumperBotY, 1.18f,  1.04f,  cabHW - 0.08f, 4, 2, 0.0f, lowerSurf, greyPaint);
    AppendXxlTaperedPatchX(mesh.get(), 0.46f, 1.18f, -0.78f, -0.78f, -grHWBot, -grHWTop + 0.06f, 4, 2, 0.0f, lowerSurf, greyPaint);
    AppendXxlTaperedPatchX(mesh.get(), 0.46f, 1.18f,  grHWBot,  grHWTop - 0.06f, 0.78f, 0.78f, 4, 2, 0.0f, lowerSurf, greyPaint);
    AppendXxlPatchX(mesh.get(), 1.16f, 1.74f, -cabHW + 0.08f, -0.78f, 3, 4, 0.0f, lowerSurf, greyPaint);
    AppendXxlPatchX(mesh.get(), 1.16f, 1.74f,  0.78f,  cabHW - 0.08f, 3, 4, 0.0f, lowerSurf, greyPaint);
    AppendXxlPatchX(mesh.get(), upBayTop, 2.22f, -cabHW + 0.08f, cabHW - 0.08f, 3, 14, 0.0f, lowerSurf, greyPaint);
    AppendXxlPatchX(mesh.get(), 2.22f, windBotY, -cabHW + 0.08f, cabHW - 0.08f, 2, 14, 0.002f, lowerSurf, glossBlack);

    // 3D. Bold 3-Slat Sculpted Black Lower Grille + Upper Black Mesh Bay w/ Yellow Winged Eagle Emblem
    for (int sIdx = 0; sIdx < 3; ++sIdx) {
        float t = (static_cast<float>(sIdx) + 0.5f) / 3.0f;
        float ym = lowGrBot + (lowGrTop - lowGrBot) * t;
        float w  = grHWBot + (grHWTop - grHWBot) * t - 0.04f;
        AppendXxlStripX(mesh.get(), ym - 0.055f, ym + 0.045f, -w, w, 0.008f, 0.020f, 10, lowerSurf, glossBlack);
        AppendXxlStripX(mesh.get(), ym + 0.045f, ym + 0.062f, -w, w, 0.010f, 0.012f, 10, lowerSurf, anthracite);
    }
    AppendXxlStripX(mesh.get(), lowGrBot, lowGrTop, -0.76f, -0.72f, 0.006f, 0.012f, 1, lowerSurf, anthracite);
    AppendXxlStripX(mesh.get(), lowGrBot, lowGrTop,  0.72f,  0.76f, 0.006f, 0.012f, 1, lowerSurf, anthracite);

    // Fine honeycomb mesh look inside Upper Intake Bay [1.22..1.72m]
    for (int r = 0; r < 6; ++r) {
        float yr = upBayBot + 0.04f + static_cast<float>(r) * 0.075f;
        AppendXxlStripX(mesh.get(), yr - 0.009f, yr + 0.009f, -0.74f, 0.74f, 0.002f, 0.010f, 10, lowerSurf, anthracite);
    }
    AppendXxlStripX(mesh.get(), upBayTop - 0.025f, upBayTop + 0.008f, -0.78f, 0.78f, 0.005f, 0.014f, 10, lowerSurf, glossBlack);
    AppendXxlStripX(mesh.get(), upBayBot - 0.008f, upBayBot + 0.025f, -0.78f, 0.78f, 0.005f, 0.014f, 10, lowerSurf, glossBlack);

    // Iconic 3D Bright Yellow Winged Eagle Emblem centered in the Upper Mesh Bay
    const float wY = 1.48f;
    AppendXxlStripX(mesh.get(), wY - 0.14f, wY + 0.09f, -0.055f, 0.055f, 0.018f, 0.014f, 2, lowerSurf, neonYellow);
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        // Sweeping outer primary wing feather + 2 tiered lower feathers
        AppendXxlLineX(mesh.get(), wY + 0.04f, s * 0.04f, wY + 0.12f, s * 0.46f, 0.028f, 0.016f, 0.014f, lowerSurf, neonYellow, 4);
        AppendXxlLineX(mesh.get(), wY - 0.02f, s * 0.04f, wY + 0.05f, s * 0.34f, 0.022f, 0.015f, 0.012f, lowerSurf, neonYellow, 4);
        AppendXxlLineX(mesh.get(), wY - 0.07f, s * 0.03f, wY - 0.01f, s * 0.22f, 0.016f, 0.014f, 0.012f, lowerSurf, neonYellow, 3);
    }

    // 3E. Gunmetal Upper Hood Yellow Racing Slash + Yellow Volvo Badge + Wipers
    AppendXxlLineX(mesh.get(), 1.84f, -cabHW + 0.12f, 2.04f, -0.02f, 0.018f, 0.010f, 0.012f, lowerSurf, neonYellow, 8);
    AppendXxlLineX(mesh.get(), 1.84f,  cabHW - 0.12f, 1.98f,  0.04f, 0.018f, 0.010f, 0.012f, lowerSurf, neonYellow, 8);
    AppendXxlLineX(mesh.get(), 1.88f, -0.24f, 2.12f, 0.24f, 0.014f, 0.014f, 0.012f, lowerSurf, neonYellow, 4);
    float bX = lowerSurf(1.98f, 0.0f);
    AppendFrontDiscX(mesh.get(), bX - 0.010f, 1.98f, 0.0f, 0.088f, 0.014f, 20, glossBlack, neonYellow);
    AppendXxlStripX(mesh.get(), 1.965f, 1.995f, -0.080f, 0.080f, 0.022f, 0.010f, 2, lowerSurf, neonYellow);
    AppendXxlStripX(mesh.get(), windBotY - 0.025f, windBotY + 0.010f, -0.92f, -0.05f, 0.012f, 0.016f, 4, lowerSurf, glossBlack);
    AppendXxlStripX(mesh.get(), windBotY - 0.025f, windBotY + 0.010f,  0.05f,  0.92f, 0.012f, 0.016f, 4, lowerSurf, glossBlack);

    // 3F. Corner Headlamps: Tall Glowing Yellow Vertical DRL Blade + Stacked White LED Projectors
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        // Bright tall vertical racing-yellow outer LED blade
        AppendXxlLineX(mesh.get(), 0.48f, s * 0.90f, 1.14f, s * 1.00f, 0.022f, 0.010f, 0.014f, lowerSurf, neonYellow, 5);
        AppendXxlLineX(mesh.get(), 1.14f, s * 1.00f, 1.18f, s * 0.80f, 0.018f, 0.010f, 0.014f, lowerSurf, neonYellow, 3);
        // Stacked inner crisp white LED projector modules
        for (int p = 0; p < 4; ++p) {
            float py = 0.60f + static_cast<float>(p) * 0.14f;
            float pz = s * (0.80f + static_cast<float>(p) * 0.02f);
            AppendXxlStripX(mesh.get(), py - 0.032f, py + 0.032f, std::min(pz - 0.03f, pz + 0.03f), std::max(pz - 0.03f, pz + 0.03f), 0.008f, 0.012f, 1, lowerSurf, ledWhite);
        }
    }

    // 4. Curved Corner Wrap-Around Side Aprons, Front Wheel Arches & Entry Steps
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        bool rightSide = (side > 0);
        float zInner = s * (cabHW - 0.08f), zOuter = s * cabHW;
        float xSideStart = xFront + 0.24f;
        for (int iy = 0; iy < 6; ++iy) {
            float ya = bumperBotY + (windBotY - bumperBotY) * (static_cast<float>(iy) / 6.0f);
            float yb = bumperBotY + (windBotY - bumperBotY) * (static_cast<float>(iy + 1) / 6.0f);
            Vec4 col = (yb > 2.22f) ? glossBlack : greyPaint;
            float xa = lowerSurf(ya, zInner), xb = lowerSurf(yb, zInner);
            if (rightSide) {
                PushQuad(mesh.get(), Vec3(xa, ya, zInner), Vec3(xSideStart, ya, zOuter), Vec3(xSideStart, yb, zOuter), Vec3(xb, yb, zInner), col);
            } else {
                PushQuad(mesh.get(), Vec3(xSideStart, ya, zOuter), Vec3(xa, ya, zInner), Vec3(xb, yb, zInner), Vec3(xSideStart, yb, zOuter), col);
            }
        }
        float zBox0 = rightSide ? (cabHW - 0.06f) : -cabHW;
        float zBox1 = rightSide ? cabHW : -(cabHW - 0.06f);
        RunningGear::AppendBox(mesh.get(), Vec3(xSideStart, bumperBotY, zBox0), Vec3(bumperEndX, cabClearY + 0.04f, zBox1), greyPaint);
        RunningGear::AppendBox(mesh.get(), Vec3(xSideStart, cabClearY + 0.02f, zBox0), Vec3(doorX0 + 0.04f, windBotY - 0.12f, zBox1), greyPaint);
        RunningGear::AppendBox(mesh.get(), Vec3(xSideStart, windBotY - 0.12f, zBox0), Vec3(doorX0 + 0.04f, windBotY, zBox1), glossBlack);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.18f, bumperBotY + 0.06f, -cabHW + 0.08f), Vec3(xFront + 0.34f, windBotY - 0.02f, cabHW - 0.08f), frameDark);

        CabComponents::AppendSemicircularWheelArchSkirt(
            mesh.get(), frontAxleX, wheelR, wheelR + 0.068f, archHalfLen,
            bumperBotY, cabClearY + 0.04f, rightSide ? cabHW : -cabHW, rightSide, 0.045f, greyPaint, glossBlack
        );
        CabComponents::AppendWheelArchFender(
            mesh.get(), frontAxleX, wheelR, wheelR + 0.072f, rightSide ? cabHW : -cabHW, rightSide, 0.048f, glossBlack
        );
    }
    AppendCabEntrySteps(mesh.get(), frontAxleX + archHalfLen, std::min(cabBackX - 0.04f, doorX1), bumperBotY + 0.05f, cabClearY + 0.02f, cabHW, glossBlack, neonYellow);
    if (cabBackX > doorX1 + 0.08f) {
        RunningGear::AppendBox(mesh.get(), Vec3(doorX1, bumperBotY + 0.05f, -cabHW), Vec3(cabBackX, cabClearY + 0.04f, -cabHW + 0.08f), greyPaint);
        RunningGear::AppendBox(mesh.get(), Vec3(doorX1, bumperBotY + 0.05f,  cabHW - 0.08f), Vec3(cabBackX, cabClearY + 0.04f,  cabHW), greyPaint);
    }

    // 5. A-Pillars, Flush Black Sunvisor, Yellow VOLVO Brow Wordmark, Black Pockets & 7-Lamp Roof Bar
    const float pillarInZ = cabHW - 0.095f;
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        bool posSide = (side > 0);
        float zIn = s * pillarInZ, zOut = s * cabHW;
        float zA = std::min(zIn, zOut), zB = std::max(zIn, zOut);
        for (int i = 0; i < 6; ++i) {
            float t0 = static_cast<float>(i) / 6.0f, t1 = static_cast<float>(i + 1) / 6.0f;
            float y0 = (windBotY - 0.01f) + ((windTopY + 0.02f) - (windBotY - 0.01f)) * t0;
            float y1 = (windBotY - 0.01f) + ((windTopY + 0.02f) - (windBotY - 0.01f)) * t1;
            float xF0 = XxlLowerFrontX(windBotY, zIn, xFront, cabHW, splitterBotY, windBotY) * (1.0f - t0) + (windBrowX + XxlLatSweep(zIn, cabHW, 0.095f)) * t0;
            float xF1 = XxlLowerFrontX(windBotY, zIn, xFront, cabHW, splitterBotY, windBotY) * (1.0f - t1) + (windBrowX + XxlLatSweep(zIn, cabHW, 0.095f)) * t1;
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

    // Flush Curved Gloss-Black Sunvisor Band [windTopY - 0.01 .. windTopY + 0.13]
    for (int iz = 0; iz < 14; ++iz) {
        float tz0 = static_cast<float>(iz) / 14.0f, tz1 = static_cast<float>(iz + 1) / 14.0f;
        float za = (-cabHW + 0.06f) + 2.0f * (cabHW - 0.06f) * tz0;
        float zb = (-cabHW + 0.06f) + 2.0f * (cabHW - 0.06f) * tz1;
        float xBotF_a = windBrowX + XxlLatSweep(za, cabHW, 0.095f) - 0.025f;
        float xBotF_b = windBrowX + XxlLatSweep(zb, cabHW, 0.095f) - 0.025f;
        float xTopF_a = windBrowX + 0.014f + XxlLatSweep(za, cabHW, 0.095f) - 0.014f;
        float xTopF_b = windBrowX + 0.014f + XxlLatSweep(zb, cabHW, 0.095f) - 0.014f;
        float xBack_a = windBrowX + 0.045f + XxlLatSweep(za, cabHW, 0.095f);
        float xBack_b = windBrowX + 0.045f + XxlLatSweep(zb, cabHW, 0.095f);
        float yb0 = windTopY - 0.01f, yb1 = windTopY + 0.13f;
        PushQuad(mesh.get(), Vec3(xBotF_a, yb0, za), Vec3(xBotF_b, yb0, zb), Vec3(xTopF_b, yb1, zb), Vec3(xTopF_a, yb1, za), glossBlack);
        PushQuad(mesh.get(), Vec3(xTopF_a, yb1, za), Vec3(xTopF_b, yb1, zb), Vec3(xBack_b, yb1, zb), Vec3(xBack_a, yb1, za), glossBlack);
        PushQuad(mesh.get(), Vec3(xBack_a, yb0, za), Vec3(xBack_b, yb0, zb), Vec3(xBotF_b, yb0, zb), Vec3(xBotF_a, yb0, za), glossBlack);
    }
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(windBrowX + 0.05f, windTopY - 0.01f, s > 0 ? cabHW - 0.06f : -cabHW - 0.005f), Vec3(windBrowX + 0.24f, windTopY + 0.13f, s > 0 ? cabHW + 0.005f : -cabHW + 0.06f), glossBlack);
    }

    // Olive Side Header Strips closing above doorTopY [xFront + 0.62f .. cabBackX - 0.04f]
    RunningGear::AppendBox(mesh.get(), Vec3(windBrowX + 0.22f, doorTopY - 0.01f, -cabHW), Vec3(cabBackX - 0.04f, windTopY + 0.14f, -cabHW + 0.08f), greyPaint);
    RunningGear::AppendBox(mesh.get(), Vec3(windBrowX + 0.22f, doorTopY - 0.01f,  cabHW - 0.08f), Vec3(cabBackX - 0.04f, windTopY + 0.14f,  cabHW), greyPaint);

    // Smooth Gunmetal XXL Forehead Brow + Black Corner Intake Pockets + Yellow VOLVO Lettering
    const float browBotY = windTopY + 0.12f;
    AppendXxlPatchX(mesh.get(), browBotY, cabTopY, -cabHW + 0.10f, cabHW - 0.10f, 7, 14, 0.0f, roofSurf, greyPaint);
    // Gloss-black recessed corner vents at upper outer brow corners [browBotY + 0.16 .. cabTopY - 0.06]
    AppendXxlPatchX(mesh.get(), browBotY + 0.16f, cabTopY - 0.06f, -cabHW + 0.14f, -0.76f, 3, 3, -0.005f, roofSurf, darkPocket);
    AppendXxlPatchX(mesh.get(), browBotY + 0.16f, cabTopY - 0.06f,  0.76f,  cabHW - 0.14f, 3, 3, -0.005f, roofSurf, darkPocket);

    // Bold Bright Yellow V O L V O wordmark + yellow eyebrow underline on gunmetal brow
    const char volvoBrow[] = {'V', 'O', 'L', 'V', 'O'};
    for (int i = 0; i < 5; ++i) {
        AppendVolvoBrowGlyph(mesh.get(), volvoBrow[i], browBotY + 0.19f, -0.44f + static_cast<float>(i) * 0.185f, 0.125f, 0.140f, 0.010f, roofSurf, neonYellow);
    }
    AppendXxlLineX(mesh.get(), browBotY + 0.10f, 0.0f, browBotY + 0.18f, -0.76f, 0.010f, 0.007f, 0.010f, roofSurf, neonYellow, 4);
    AppendXxlLineX(mesh.get(), browBotY + 0.10f, 0.0f, browBotY + 0.18f,  0.76f, 0.010f, 0.007f, 0.010f, roofSurf, neonYellow, 4);

    // 7-Lamp High-Output LED Roof Lightbar across upper forehead [cabTopY + 0.02f]
    float lbY = cabTopY + 0.015f;
    float lbX = roofSurf(cabTopY - 0.02f, 0.0f) - 0.04f;
    RunningGear::AppendBox(mesh.get(), Vec3(lbX - 0.04f, lbY - 0.02f, -0.78f), Vec3(lbX + 0.08f, lbY + 0.025f, 0.78f), glossBlack);
    for (int i = -3; i <= 3; ++i) {
        float lz = static_cast<float>(i) * 0.21f;
        float lxOff = lbX + 0.02f * std::abs(static_cast<float>(i)) / 3.0f;
        ChassisComponents::AppendClosedCylinderX(mesh.get(), lxOff - 0.04f, lxOff + 0.11f, lbY + 0.095f, lz, 0.085f, 16, glossBlack, glossBlack);
        AppendFrontDiscX(mesh.get(), lxOff - 0.042f, lbY + 0.095f, lz, 0.072f, 0.010f, 16, ledWhite, chromeCol);
    }

    // Smooth corner wrap from roof brow front edges back into cab sidewalls + XXL roof crown
    for (int side : {-1, 1}) {
        bool rightSide = (side > 0);
        float s = static_cast<float>(side);
        float zInner = s * (cabHW - 0.10f), zOuter = s * cabHW;
        for (int iy = 0; iy < 6; ++iy) {
            float ya = browBotY + (cabTopY - browBotY) * (static_cast<float>(iy) / 6.0f);
            float yb = browBotY + (cabTopY - browBotY) * (static_cast<float>(iy + 1) / 6.0f);
            float xa = roofSurf(ya, zInner), xb = roofSurf(yb, zInner);
            float xSideA = xa + 0.14f, xSideB = xb + 0.14f;
            if (rightSide) {
                PushQuad(mesh.get(), Vec3(xa, ya, zInner), Vec3(xSideA, ya, zOuter), Vec3(xSideB, yb, zOuter), Vec3(xb, yb, zInner), greyPaint);
            } else {
                PushQuad(mesh.get(), Vec3(xSideA, ya, zOuter), Vec3(xa, ya, zInner), Vec3(xb, yb, zInner), Vec3(xSideB, yb, zOuter), greyPaint);
            }
            float zWall0 = rightSide ? (cabHW - 0.06f) : -cabHW;
            float zWall1 = rightSide ? cabHW : -(cabHW - 0.06f);
            RunningGear::AppendBox(mesh.get(), Vec3(xSideB, ya, zWall0), Vec3(cabBackX - 0.04f, yb + 0.01f, zWall1), greyPaint);
        }
    }
    const float roofEndX = cabBackX - 0.04f;
    for (int iz = 0; iz < 12; ++iz) {
        float tz0 = static_cast<float>(iz) / 12.0f, tz1 = static_cast<float>(iz + 1) / 12.0f;
        float z0 = -(cabHW - 0.08f) + 2.0f * (cabHW - 0.08f) * tz0;
        float z1 = -(cabHW - 0.08f) + 2.0f * (cabHW - 0.08f) * tz1;
        Vec4 deckCol = (std::abs(0.5f * (z0 + z1)) <= 0.60f) ? glossBlack : greyPaint;
        float xLead0 = roofSurf(cabTopY, z0), xLead1 = roofSurf(cabTopY, z1);
        for (int ix = 0; ix < 5; ++ix) {
            float tx0 = static_cast<float>(ix) / 5.0f, tx1 = static_cast<float>(ix + 1) / 5.0f;
            float x0a = xLead0 + (roofEndX - xLead0) * tx0, x0b = xLead1 + (roofEndX - xLead1) * tx0;
            float x1a = xLead0 + (roofEndX - xLead0) * tx1, x1b = xLead1 + (roofEndX - xLead1) * tx1;
            float ay0 = cabTopY + 0.025f * std::sin(tx0 * 3.14159f);
            float ay1 = cabTopY + 0.025f * std::sin(tx1 * 3.14159f);
            PushQuad(mesh.get(), Vec3(x0a, ay0, z0), Vec3(x0b, ay0, z1), Vec3(x1b, ay1, z1), Vec3(x1a, ay1, z0), deckCol);
        }
    }

    // Carved Extended Globetrotter XXL Sleeper Walls [doorX1 .. cabBackX] + Racing Yellow Graphic Ribbons
    std::vector<HullStationX> rearSleeper = {
        {doorX1,           cabClearY, cabTopY - 0.04f, cabHW,         0.10f, 0.12f, 0.0f, 0.0f},
        {cabBackX - 0.14f, cabClearY, cabTopY - 0.04f, cabHW,         0.10f, 0.14f, 0.0f, 0.0f},
        {cabBackX,         cabClearY + 0.04f, cabTopY - 0.04f, cabHW - 0.06f, 0.12f, 0.16f, 0.0f, 0.0f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), rearSleeper, greyPaint, 5);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        // Extended rear sleeper cab corner fairing
        RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.04f, cabClearY + 0.04f, s > 0 ? cabHW - 0.05f : -cabHW), Vec3(cabBackX + 0.22f, windTopY + 0.14f, s > 0 ? cabHW : -cabHW + 0.05f), greyPaint);
        // Dark XXL flush upper sleeper window panel
        float zW0 = s > 0 ? (cabHW - 0.01f) : (-cabHW - 0.012f);
        float zW1 = s > 0 ? (cabHW + 0.012f) : (-cabHW + 0.01f);
        RunningGear::AppendBox(mesh.get(), Vec3(doorX1 + 0.14f, cabTopY - 0.85f, zW0), Vec3(cabBackX - 0.24f, cabTopY - 0.30f, zW1), glossBlack);
        // Diagonal & sweeping racing-yellow graphics across door, sleeper & upper roof outline (Photo 2!)
        AppendSideRibbonZ(mesh.get(), s * cabHW, 0.014f, side > 0, xFront + 0.34f, cabClearY + 0.48f, cabBackX - 0.12f, cabClearY + 0.18f, 0.016f, neonYellow);
        AppendSideRibbonZ(mesh.get(), s * cabHW, 0.014f, side > 0, doorX1 + 0.02f, doorBeltY, cabBackX - 0.06f, cabTopY - 0.20f, 0.016f, neonYellow);
        AppendSideRibbonZ(mesh.get(), s * cabHW, 0.014f, side > 0, windBrowX + 0.32f, cabTopY - 0.12f, cabBackX - 0.10f, cabTopY - 0.10f, 0.015f, neonYellow);
    }

    // 6. Kerb-Dipped Doors + Standard Gunmetal Wing Mirrors + Front Roof Markers
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.16f, false, false, greyPaint, glossBlack, sideGlass, doorBeltY, def.is_rhd, true);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.16f, false, false, greyPaint, glossBlack, sideGlass, doorBeltY, !def.is_rhd, true);
    AppendGunmetalWingMirrors(mesh.get(), xFront + 0.50f, windBotY + 0.28f, windTopY - 0.02f, cabHW, greyPaint, glossBlack, neonYellow);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.36f, windTopY - 0.10f, -cabHW, glossBlack);
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.48f, windTopY + 0.06f, cabHW * 0.85f, def.headlights_on);

    // 7. 6x4 Gloss-Black / Gunmetal Aero Side Skirts w/ Yellow Striping + Tandem Rear Fender Arches
    float skirtStart = cabBackX + 0.08f;
    float skirtEnd   = std::max(skirtStart + 0.55f, (driveCenterX - 0.68f) - archHalfLen - 0.04f);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, bumperBotY + 0.04f, -hw + 0.03f), Vec3(skirtEnd, frameTopY + 0.04f, -hw + 0.10f), glossBlack);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, bumperBotY + 0.04f,  hw - 0.10f), Vec3(skirtEnd, frameTopY + 0.04f,  hw - 0.03f), glossBlack);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.03f, bumperBotY + 0.05f, -hw + 0.02f), Vec3(skirtEnd - 0.03f, bumperBotY + 0.085f, -hw + 0.04f), neonYellow);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.03f, bumperBotY + 0.05f,  hw - 0.04f), Vec3(skirtEnd - 0.03f, bumperBotY + 0.085f,  hw - 0.02f), neonYellow);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.04f, frameTopY, -hw + 0.02f), Vec3(skirtEnd - 0.04f, frameTopY + 0.03f, -hw + 0.04f), neonYellow);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.04f, frameTopY,  hw - 0.04f), Vec3(skirtEnd - 0.04f, frameTopY + 0.03f,  hw - 0.02f), neonYellow);
    for (float dx : {d0, d1}) {
        CabComponents::AppendWheelArchFender(mesh.get(), dx, wheelR, wheelR + 0.08f, -hw, false, tireW * 1.15f, greyPaint);
        CabComponents::AppendWheelArchFender(mesh.get(), dx, wheelR, wheelR + 0.08f,  hw, true,  tireW * 1.15f, greyPaint);
    }
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.08f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.68f, frameTopY + 0.04f, 0.45f), anthracite);

    // 8. Translucent Flush Curved Windshield & Door Glass Pass
    const float gHW = cabHW - 0.095f;
    for (int i = 0; i < 12; ++i) {
        float za = -gHW + (2.0f * gHW) * (static_cast<float>(i) / 12.0f);
        float zb = -gHW + (2.0f * gHW) * (static_cast<float>(i + 1) / 12.0f);
        float xBotA = lowerSurf(windBotY, za) + 0.008f;
        float xBotB = lowerSurf(windBotY, zb) + 0.008f;
        float xTopA = windBrowX + XxlLatSweep(za, cabHW, 0.095f) + 0.006f;
        float xTopB = windBrowX + XxlLatSweep(zb, cabHW, 0.095f) + 0.006f;
        PushQuad(mesh.get(), Vec3(xBotA, windBotY - 0.01f, za), Vec3(xBotB, windBotY - 0.01f, zb), Vec3(xTopB, windTopY + 0.01f, zb), Vec3(xTopA, windTopY + 0.01f, za), windshieldGlass);
    }
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.16f, false, true, greyPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.16f, false, true, greyPaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
