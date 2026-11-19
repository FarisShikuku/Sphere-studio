#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace sphere::vehicles::trucks {

namespace {

inline float Fh780LatSweep(float z, float cabHW, float maxSweepX) {
    float nz = std::clamp(std::abs(z) / std::max(0.25f, cabHW), 0.0f, 1.0f);
    return maxSweepX * (0.26f * nz + 0.74f * nz * nz);
}

inline float Fh780LowerFrontX(float y, float z, float xFront, float cabHW, float splitterBotY, float windBotY) {
    float ty = std::clamp((y - splitterBotY) / std::max(0.2f, windBotY - splitterBotY), 0.0f, 1.0f);
    // Volvo FH16 780 Aero elongated nose raking smoothly into windshield cowl
    return xFront + 0.145f * ty + Fh780LatSweep(z, cabHW, 0.115f);
}

inline float Fh780RoofBrowX(float y, float z, float windBrowX, float windTopY, float cabTopY, float cabHW) {
    float tr = std::clamp((y - windTopY) / std::max(0.2f, cabTopY - windTopY), 0.0f, 1.0f);
    float archBack = 0.045f * tr + 0.44f * tr * tr * tr;
    return windBrowX + archBack + Fh780LatSweep(z, cabHW, 0.105f) * (1.0f - 0.28f * tr * tr);
}

template <typename SurfFn>
void AppendCurvedPatchX(
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
void AppendFlushBoxOnSurfX(
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
void AppendFlushLineOnSurfX(
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

// 3D Micro-font character strokes flush on curved surface surfX for "FH16 780" / "VOLVO"
template <typename SurfFn>
void AppendSurfGlyph(
    Mesh* mesh, char ch,
    float yBot, float zLeft, float h, float w, float t, float proudX,
    SurfFn&& surfX, const Vec4& col
) {
    auto box = [&](float yn0, float yn1, float zn0, float zn1) {
        AppendFlushBoxOnSurfX(
            mesh, yBot + h * yn0, yBot + h * yn1,
            zLeft + w * zn0, zLeft + w * zn1,
            proudX, 0.010f, 1, surfX, col
        );
    };
    (void)t;
    switch (ch) {
        case 'F': box(0.0f, 1.0f, 0.0f, 0.24f); box(0.78f, 1.0f, 0.24f, 1.0f); box(0.42f, 0.62f, 0.24f, 0.85f); break;
        case 'H': box(0.0f, 1.0f, 0.0f, 0.24f); box(0.0f, 1.0f, 0.76f, 1.0f); box(0.40f, 0.60f, 0.24f, 0.76f); break;
        case '1': box(0.0f, 1.0f, 0.36f, 0.64f); box(0.72f, 0.92f, 0.14f, 0.40f); box(0.0f, 0.18f, 0.14f, 0.86f); break;
        case '6': box(0.0f, 1.0f, 0.0f, 0.24f); box(0.78f, 1.0f, 0.24f, 1.0f); box(0.42f, 0.62f, 0.24f, 1.0f); box(0.0f, 0.22f, 0.24f, 1.0f); box(0.22f, 0.42f, 0.76f, 1.0f); break;
        case '7': box(0.78f, 1.0f, 0.0f, 1.0f); box(0.0f, 0.78f, 0.70f, 0.96f); break;
        case '8': box(0.0f, 1.0f, 0.0f, 0.24f); box(0.0f, 1.0f, 0.76f, 1.0f); box(0.78f, 1.0f, 0.24f, 0.76f); box(0.40f, 0.60f, 0.24f, 0.76f); box(0.0f, 0.22f, 0.24f, 0.76f); break;
        case '0': box(0.0f, 1.0f, 0.0f, 0.24f); box(0.0f, 1.0f, 0.76f, 1.0f); box(0.78f, 1.0f, 0.24f, 0.76f); box(0.0f, 0.22f, 0.24f, 0.76f); break;
        case 'V': box(0.22f, 1.0f, 0.0f, 0.24f); box(0.22f, 1.0f, 0.76f, 1.0f); box(0.0f, 0.26f, 0.28f, 0.72f); break;
        case 'O': box(0.0f, 1.0f, 0.0f, 0.24f); box(0.0f, 1.0f, 0.76f, 1.0f); box(0.78f, 1.0f, 0.24f, 0.76f); box(0.0f, 0.22f, 0.24f, 0.76f); break;
        case 'L': box(0.0f, 1.0f, 0.0f, 0.26f); box(0.0f, 0.22f, 0.26f, 1.0f); break;
        default: break;
    }
}

void AppendChromeAlcoaHub(
    Mesh* mesh, float axleX, float wheelR, float tireW, float halfTrack,
    float steerAngleDeg, bool isDualRear, const Vec4& chromeCol, const Vec4& darkRing
) {
    float steerRad = steerAngleDeg * (3.14159265f / 180.0f);
    float cS = std::cos(steerRad), sS = std::sin(steerRad);
    float outerOff = isDualRear ? (tireW * 0.56f) : 0.0f;
    float wheelHW = tireW * 0.5f;
    float dishZ = wheelHW * 0.58f, hubZ = wheelHW * 0.80f;
    float dishR = wheelR * 0.48f, hubR = wheelR * 0.21f;
    int segs = 18;
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
                PushTri(mesh, cHub, h0, h1, chromeCol);
                PushQuad(mesh, h0, d0, d1, h1, (i % 2 == 0) ? chromeCol : darkRing);
            } else {
                PushTri(mesh, cHub, h1, h0, chromeCol);
                PushQuad(mesh, h1, d1, d0, h0, (i % 2 == 0) ? chromeCol : darkRing);
            }
        }
    }
}

// Painted Emerald Green MirrorCam Camera Arms (Camera Monitor System matching Photo 1)
void AppendGreenMirrorCams(
    Mesh* mesh,
    float anchorX, float mountY, float cabHW,
    const Vec4& greenPaint, const Vec4& blackLens, const Vec4& silverAccent
) {
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zIn  = s * (cabHW - 0.02f);
        float zMid = s * (cabHW + 0.18f);
        float zOut = s * (cabHW + 0.34f);
        float armZ0 = std::min(zIn, zOut), armZ1 = std::max(zIn, zOut);
        float midZ0 = std::min(zMid, zOut), midZ1 = std::max(zMid, zOut);
        // Sculpted green streamlined aerodynamic MirrorCam wing arm
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.045f, mountY, armZ0), Vec3(anchorX + 0.055f, mountY + 0.052f, armZ1), greenPaint);
        // Underside articulated gloss-black dual-lens camera pod at mid-wing
        RunningGear::AppendBox(mesh, Vec3(anchorX - 0.030f, mountY - 0.135f, midZ0), Vec3(anchorX + 0.065f, mountY + 0.005f, midZ1 - (s > 0 ? 0.05f : 0.0f)), blackLens);
        RunningGear::AppendBox(mesh, Vec3(anchorX + 0.058f, mountY - 0.115f, midZ0 + 0.02f), Vec3(anchorX + 0.072f, mountY - 0.045f, midZ1 - 0.06f), silverAccent);
    }
}

} // namespace

// Photo-matched procedural C++20 3D generator for Volvo FH16 780 Aero 4x2 (Emerald Racing Green):
// - Extended smooth Aero nose w/ gloss-black windshield collar + VOLVO trim
// - Clean painted green upper shield w/ circular (L) low-noise badge & horizontal brushed-silver mesh vent
// - Signature lower trapezoidal satin-silver Aero grille frame + fine mesh + diagonal Volvo Iron Mark sash
// - Corner V-shaped Thor's Hammer LED DRLs + twin horizontal lower fog lamps + Green MirrorCam arms
// - Illuminated FH16 780 lightbox header + curved tubular chrome roof bar w/ dual chrome air horns + rear spoiler
std::shared_ptr<Mesh> GenerateVolvoFh16780Mesh(const VehicleDef& def) {
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

    const float splitterBotY = groundClear - 0.02f;  // 0.22m
    const float bumperBotY   = groundClear + 0.02f;  // 0.26m
    const float cabBaseY     = frameBotY - 0.04f;    // 0.86m
    const float cabClearY    = std::max(cabBaseY + 0.24f, archTopY + 0.02f); // 1.20m
    const float cabHW        = hw - 0.03f;           // 1.23m
    const float cabBackX     = xFront + 2.56f;
    const float cabTopY      = std::clamp(def.cab_height_m, 3.45f, 3.98f);

    const float frontAxleX   = xFront + 1.44f;
    const float driveCenterX = std::min(xRear - 0.90f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX  = std::clamp(def.fifth_wheel_offset_m, 0.80f, xRear - 0.88f);

    const Vec4 greenPaint  = def.base_color;
    const Vec4 silverTrim  = def.accent_color;
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 darkPocket(0.035f, 0.040f, 0.048f, 1.0f);
    const Vec4 meshBack(0.11f, 0.13f, 0.16f, 1.0f);
    const Vec4 chromeCol(0.92f, 0.95f, 0.99f, 1.0f);
    const Vec4 frameDark(0.12f, 0.14f, 0.17f, 1.0f);
    const Vec4 linerDark(0.045f, 0.05f, 0.06f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.14f, 1.0f);
    const Vec4 interiorSeat(0.16f, 0.20f, 0.22f, 1.0f);
    const Vec4 cabinGlow(0.22f, 0.88f, 0.48f, 1.0f);
    const Vec4 windshieldGlass(0.26f, 0.46f, 0.58f, 0.26f);
    const Vec4 sideGlass(0.18f, 0.34f, 0.48f, 0.42f);
    const Vec4 amberLed(0.98f, 0.64f, 0.10f, 1.0f);
    const Vec4 whiteHeader(0.95f, 0.97f, 0.99f, 1.0f);
    const Vec4 ledWhite = def.headlights_on ? Vec4(0.97f, 0.99f, 1.00f, 1.0f) : Vec4(0.74f, 0.80f, 0.86f, 1.0f);

    // 1. Chassis Rails, 4x2 Axles, Green Painted Drive Wheel Arches & Polished Chrome Alcoa Wheels
    AppendTractorChassisBase(
        mesh.get(), def, xFront, xRear, frontAxleX, driveCenterX, fifthWheelX,
        wheelR, tireW, hw, cabHW, frameBotY, frameTopY, archTopY, archHalfLen,
        frameDark, glossBlack, linerDark, silverTrim
    );
    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = hw - tireW * 1.06f - 0.025f;
    AppendChromeAlcoaHub(mesh.get(), frontAxleX, wheelR, tireW, frontHalfTrack, def.steer_angle_deg, false, chromeCol, frameDark);
    AppendChromeAlcoaHub(mesh.get(), driveCenterX, wheelR, tireW, rearHalfTrack, 0.0f, true, chromeCol, frameDark);

    // Panoramic Windshield & Kerb-Dipped Doors
    const float windBrowX  = xFront + 0.42f;
    const float windBotY   = 2.34f;
    const float windTopY   = std::min(cabTopY - 0.58f, windBotY + 0.82f); // 3.16m
    const float doorX0     = xFront + 0.48f;
    const float doorX1     = cabBackX - 0.72f;
    const float doorTopY   = windTopY + 0.04f;
    const float doorBeltY  = windBotY - 0.02f;

    auto lowerSurf = [&](float y, float z) {
        return Fh780LowerFrontX(y, z, xFront, cabHW, splitterBotY, windBotY);
    };
    auto roofSurf = [&](float y, float z) {
        return Fh780RoofBrowX(y, z, windBrowX, windTopY + 0.11f, cabTopY, cabHW);
    };

    // 2. Full LHD / RHD Volvo Cockpit Interior
    CabComponents::AppendVolvoCabInterior(
        mesh.get(), xFront + 0.34f, cabBackX - 0.08f,
        cabClearY + 0.04f, windBotY, cabTopY - 0.16f,
        cabHW, true, def.is_rhd, def.steer_angle_deg, def.headlights_on,
        dashDark, interiorSeat, chromeCol, cabinGlow
    );

    // 3. Continuous Smooth Aero Front Fascia (Photo 1 Exact Layout)
    const float bumperEndX = frontAxleX - archHalfLen + 0.02f;
    const float meshGrBot  = 0.42f, meshGrTop  = 1.15f;
    const float meshHWBot  = 0.50f, meshHWTop  = 0.62f;
    const float midVentBot = 1.38f, midVentTop = 1.62f;

    // 3A. Gloss-Black Chin Spoiler (0.22..0.255m) + Green Lower Bumper Lip (0.255..0.42m) + Radar/Plate Bar
    AppendFlushBoxOnSurfX(mesh.get(), splitterBotY, bumperBotY, -cabHW + 0.04f, cabHW - 0.04f, 0.012f, 0.16f, 12, lowerSurf, glossBlack);
    AppendCurvedPatchX(mesh.get(), bumperBotY, 0.42f, -0.62f, 0.62f, 2, 10, 0.0f, lowerSurf, greenPaint);
    AppendFlushBoxOnSurfX(mesh.get(), bumperBotY + 0.02f, bumperBotY + 0.11f, -0.25f, 0.25f, 0.008f, 0.014f, 3, lowerSurf, glossBlack);
    AppendFlushBoxOnSurfX(mesh.get(), bumperBotY + 0.035f, bumperBotY + 0.095f, -0.23f, 0.23f, 0.011f, 0.008f, 3, lowerSurf, silverTrim);
    AppendFlushBoxOnSurfX(mesh.get(), bumperBotY + 0.040f, bumperBotY + 0.090f, -0.14f, 0.14f, 0.013f, 0.008f, 2, lowerSurf, glossBlack);

    // 3B. Lower Recessed Corner Thor's-Hammer Lamp Bays & Lower Trapezoidal Grille Pocket
    AppendCurvedPatchX(mesh.get(), meshGrBot, meshGrTop, -meshHWTop - 0.02f, meshHWTop + 0.02f, 4, 12, 0.010f, lowerSurf, meshBack);
    for (int side : {-1, 1}) {
        float z0 = (side > 0) ? 0.56f : -1.06f;
        float z1 = (side > 0) ? 1.06f : -0.56f;
        AppendCurvedPatchX(mesh.get(), 0.34f, 1.15f, z0, z1, 3, 4, 0.008f, lowerSurf, darkPocket);
    }

    // 3C. Tapered Emerald Green Front Cheeks Hugging Lower Trapezoidal Grille & Headlamps
    AppendCurvedPatchX(mesh.get(), bumperBotY, 1.16f, -cabHW + 0.08f, -1.04f, 4, 2, 0.0f, lowerSurf, greenPaint);
    AppendCurvedPatchX(mesh.get(), bumperBotY, 1.16f,  1.04f,  cabHW - 0.08f, 4, 2, 0.0f, lowerSurf, greenPaint);
    // Clean green cheek pillars between trapezoidal grille & corner headlamps [0.42..1.16]
    AppendCurvedTaperedPatchX(mesh.get(), meshGrBot, meshGrTop, -0.66f, -0.72f, -meshHWBot, -meshHWTop, 4, 2, 0.0f, lowerSurf, greenPaint);
    AppendCurvedTaperedPatchX(mesh.get(), meshGrBot, meshGrTop,  meshHWBot,  meshHWTop,  0.66f,  0.72f, 4, 2, 0.0f, lowerSurf, greenPaint);

    // 3D. Painted Green Smooth Upper Hood Panel [1.15 .. 2.16m] w/ Recessed Center Perforated Vent [1.38..1.62m]
    AppendCurvedPatchX(mesh.get(), 1.15f, midVentBot, -cabHW + 0.08f, cabHW - 0.08f, 2, 14, 0.0f, lowerSurf, greenPaint);
    AppendCurvedPatchX(mesh.get(), midVentBot, midVentTop, -cabHW + 0.08f, -0.56f, 2, 4, 0.0f, lowerSurf, greenPaint);
    AppendCurvedPatchX(mesh.get(), midVentBot, midVentTop,  0.56f,  cabHW - 0.08f, 2, 4, 0.0f, lowerSurf, greenPaint);
    AppendCurvedPatchX(mesh.get(), midVentTop, 2.16f, -cabHW + 0.08f, cabHW - 0.08f, 4, 14, 0.0f, lowerSurf, greenPaint);

    // Horizontal brushed-silver perforated intake grille inset at [1.38..1.62m, ±0.56m]
    AppendCurvedPatchX(mesh.get(), midVentBot, midVentTop, -0.57f, 0.57f, 2, 8, 0.008f, lowerSurf, darkPocket);
    AppendFlushBoxOnSurfX(mesh.get(), midVentTop - 0.020f, midVentTop + 0.004f, -0.56f, 0.56f, 0.004f, 0.012f, 8, lowerSurf, silverTrim);
    AppendFlushBoxOnSurfX(mesh.get(), midVentBot - 0.004f, midVentBot + 0.020f, -0.56f, 0.56f, 0.004f, 0.012f, 8, lowerSurf, silverTrim);
    for (int r = 0; r < 4; ++r) {
        float yBar = midVentBot + 0.042f + static_cast<float>(r) * 0.048f;
        AppendFlushBoxOnSurfX(mesh.get(), yBar - 0.010f, yBar + 0.010f, -0.54f, 0.54f, 0.002f, 0.010f, 8, lowerSurf, silverTrim);
    }

    // Green circular "(L)" Low-Noise Euro Disc Badge on viewer left (-Z) upper hood
    float lBadgeX = lowerSurf(1.76f, -0.76f);
    AppendFrontDiscX(mesh.get(), lBadgeX - 0.006f, 1.76f, -0.76f, 0.052f, 0.008f, 16, Vec4(0.06f, 0.44f, 0.22f, 1.0f), whiteHeader);
    AppendSurfGlyph(mesh.get(), 'L', 1.732f, -0.785f, 0.056f, 0.044f, 0.012f, 0.014f, lowerSurf, whiteHeader);

    // Gloss-black under-windshield collar [2.16..2.34m] + Spaced Silver VOLVO Letters
    AppendCurvedPatchX(mesh.get(), 2.16f, windBotY, -cabHW + 0.08f, cabHW - 0.08f, 2, 14, -0.002f, lowerSurf, glossBlack);
    AppendFlushBoxOnSurfX(mesh.get(), 2.25f, 2.264f, -cabHW + 0.14f, cabHW - 0.14f, 0.006f, 0.008f, 12, lowerSurf, darkPocket);
    const char volvoWord[] = {'V', 'O', 'L', 'V', 'O'};
    for (int i = 0; i < 5; ++i) {
        AppendSurfGlyph(mesh.get(), volvoWord[i], 2.185f, -0.34f + static_cast<float>(i) * 0.145f, 0.048f, 0.068f, 0.014f, 0.009f, lowerSurf, silverTrim);
    }
    // Dual parked wipers
    AppendFlushBoxOnSurfX(mesh.get(), windBotY - 0.025f, windBotY + 0.010f, -0.92f, -0.05f, 0.012f, 0.016f, 4, lowerSurf, glossBlack);
    AppendFlushBoxOnSurfX(mesh.get(), windBotY - 0.025f, windBotY + 0.010f,  0.05f,  0.92f, 0.012f, 0.016f, 4, lowerSurf, glossBlack);

    // 3E. Signature Satin-Silver Trapezoidal Lower Mesh Grille + Diagonal Volvo Iron Mark Sash
    AppendFlushBoxOnSurfX(mesh.get(), meshGrTop - 0.032f, meshGrTop + 0.012f, -meshHWTop - 0.01f, meshHWTop + 0.01f, 0.009f, 0.016f, 10, lowerSurf, silverTrim);
    AppendFlushBoxOnSurfX(mesh.get(), meshGrBot - 0.010f, meshGrBot + 0.024f, -meshHWBot, meshHWBot, 0.009f, 0.016f, 8, lowerSurf, silverTrim);
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        AppendFlushLineOnSurfX(mesh.get(), meshGrBot, s * meshHWBot, meshGrTop, s * meshHWTop, 0.022f, 0.009f, 0.016f, lowerSurf, silverTrim, 5);
    }
    for (int r = 0; r < 6; ++r) {
        float t = (static_cast<float>(r) + 0.5f) / 6.0f;
        float ym = meshGrBot + (meshGrTop - meshGrBot) * t;
        float w = meshHWBot + (meshHWTop - meshHWBot) * t - 0.028f;
        AppendFlushBoxOnSurfX(mesh.get(), ym - 0.012f, ym + 0.012f, -w, w, 0.002f, 0.012f, 8, lowerSurf, silverTrim);
    }
    for (int c = -4; c <= 4; ++c) {
        float zc = static_cast<float>(c) * 0.115f;
        AppendFlushBoxOnSurfX(mesh.get(), meshGrBot + 0.02f, meshGrTop - 0.02f, zc - 0.008f, zc + 0.008f, 0.001f, 0.010f, 1, lowerSurf, silverTrim);
    }
    // Prominent diagonal satin-silver Volvo Iron Mark sash & circular black/chrome medallion
    const float badgeY = 0.78f;
    AppendFlushLineOnSurfX(mesh.get(), meshGrBot + 0.04f, -meshHWBot + 0.06f, meshGrTop - 0.04f, meshHWTop - 0.06f, 0.018f, 0.014f, 0.012f, lowerSurf, silverTrim, 6);
    float bX = lowerSurf(badgeY, 0.0f);
    AppendFrontDiscX(mesh.get(), bX - 0.012f, badgeY, 0.0f, 0.105f, 0.014f, 22, glossBlack, chromeCol);
    AppendFlushBoxOnSurfX(mesh.get(), badgeY - 0.018f, badgeY + 0.018f, -0.095f, 0.095f, 0.026f, 0.010f, 2, lowerSurf, chromeCol);
    AppendFlushBoxOnSurfX(mesh.get(), badgeY - 0.007f, badgeY + 0.007f, -0.078f, 0.078f, 0.028f, 0.008f, 2, lowerSurf, glossBlack);

    // 3F. Corner V-Shaped LED "Thor's Hammer" Headlamps + Twin Lower Fog Blades
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        // Bright swept vertical-angled LED DRL blade descending inward toward lower grille
        AppendFlushLineOnSurfX(mesh.get(), 0.56f, s * 0.64f, 1.04f, s * 0.94f, 0.018f, 0.007f, 0.014f, lowerSurf, ledWhite, 5);
        AppendFlushLineOnSurfX(mesh.get(), 0.50f, s * 0.66f, 0.58f, s * 0.98f, 0.014f, 0.007f, 0.012f, lowerSurf, ledWhite, 3);
        // Inner circular projector lens inside upper headlamp housing
        float lx = lowerSurf(0.84f, s * 0.78f) - 0.003f;
        AppendCircularLampPodX(mesh.get(), lx, 0.84f, s * 0.78f, 0.068f, 0.044f, 16, glossBlack, chromeCol, ledWhite);
        // Horizontal bottom fog light strip at bumper apron [0.35..0.41m]
        float fz0 = (side > 0) ? 0.66f : -0.98f;
        float fz1 = (side > 0) ? 0.98f : -0.66f;
        AppendFlushBoxOnSurfX(mesh.get(), 0.35f, 0.405f, fz0, fz1, 0.006f, 0.012f, 2, lowerSurf, ledWhite);
    }

    // 4. Curved Corner Wrap-Around Side Aprons, Front Wheel Arches & Entry Steps
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        bool rightSide = (side > 0);
        float zInner = s * (cabHW - 0.08f);
        float zOuter = s * cabHW;
        float xSideStart = xFront + 0.28f;
        const int cySegs = 6;
        for (int iy = 0; iy < cySegs; ++iy) {
            float ya = bumperBotY + (windBotY - bumperBotY) * (static_cast<float>(iy) / static_cast<float>(cySegs));
            float yb = bumperBotY + (windBotY - bumperBotY) * (static_cast<float>(iy + 1) / static_cast<float>(cySegs));
            Vec4 col = (yb > 2.16f) ? glossBlack : greenPaint;
            float xa = lowerSurf(ya, zInner);
            float xb = lowerSurf(yb, zInner);
            if (rightSide) {
                PushQuad(mesh.get(), Vec3(xa, ya, zInner), Vec3(xSideStart, ya, zOuter), Vec3(xSideStart, yb, zOuter), Vec3(xb, yb, zInner), col);
            } else {
                PushQuad(mesh.get(), Vec3(xSideStart, ya, zOuter), Vec3(xa, ya, zInner), Vec3(xb, yb, zInner), Vec3(xSideStart, yb, zOuter), col);
            }
        }
        float zBox0 = rightSide ? (cabHW - 0.06f) : -cabHW;
        float zBox1 = rightSide ? cabHW : -(cabHW - 0.06f);
        RunningGear::AppendBox(mesh.get(), Vec3(xSideStart, bumperBotY, zBox0), Vec3(bumperEndX, cabClearY + 0.04f, zBox1), greenPaint);
        RunningGear::AppendBox(mesh.get(), Vec3(xSideStart, cabClearY + 0.02f, zBox0), Vec3(doorX0 + 0.04f, windBotY - 0.16f, zBox1), greenPaint);
        RunningGear::AppendBox(mesh.get(), Vec3(xSideStart, windBotY - 0.16f, zBox0), Vec3(doorX0 + 0.04f, windBotY, zBox1), glossBlack);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.22f, bumperBotY + 0.06f, -cabHW + 0.08f), Vec3(xFront + 0.36f, windBotY - 0.02f, cabHW - 0.08f), frameDark);

        CabComponents::AppendSemicircularWheelArchSkirt(
            mesh.get(), frontAxleX, wheelR, wheelR + 0.068f, archHalfLen,
            bumperBotY, cabClearY + 0.04f, rightSide ? cabHW : -cabHW, rightSide, 0.045f, greenPaint, glossBlack
        );
        CabComponents::AppendWheelArchFender(
            mesh.get(), frontAxleX, wheelR, wheelR + 0.072f, rightSide ? cabHW : -cabHW, rightSide, 0.048f, greenPaint
        );
    }
    AppendCabEntrySteps(mesh.get(), frontAxleX + archHalfLen, std::min(cabBackX - 0.04f, doorX1), bumperBotY + 0.05f, cabClearY + 0.02f, cabHW, glossBlack, silverTrim);
    if (cabBackX > doorX1 + 0.08f) {
        RunningGear::AppendBox(mesh.get(), Vec3(doorX1, bumperBotY + 0.05f, -cabHW), Vec3(cabBackX, cabClearY + 0.04f, -cabHW + 0.08f), greenPaint);
        RunningGear::AppendBox(mesh.get(), Vec3(doorX1, bumperBotY + 0.05f,  cabHW - 0.08f), Vec3(cabBackX, cabClearY + 0.04f,  cabHW), greenPaint);
    }

    // 5. Sleek A-Pillars, Flush Sunvisor, Illuminated "FH16 780" Lightbox Header & Chrome Hadley Horn Bar
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
            float xF0 = Fh780LowerFrontX(windBotY, zIn, xFront, cabHW, splitterBotY, windBotY) * (1.0f - t0) + (windBrowX + Fh780LatSweep(zIn, cabHW, 0.105f)) * t0;
            float xF1 = Fh780LowerFrontX(windBotY, zIn, xFront, cabHW, splitterBotY, windBotY) * (1.0f - t1) + (windBrowX + Fh780LatSweep(zIn, cabHW, 0.105f)) * t1;
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
    for (int iz = 0; iz < 14; ++iz) {
        float tz0 = static_cast<float>(iz) / 14.0f, tz1 = static_cast<float>(iz + 1) / 14.0f;
        float za = (-cabHW + 0.06f) + 2.0f * (cabHW - 0.06f) * tz0;
        float zb = (-cabHW + 0.06f) + 2.0f * (cabHW - 0.06f) * tz1;
        float xBotF_a = windBrowX + Fh780LatSweep(za, cabHW, 0.105f) - 0.024f;
        float xBotF_b = windBrowX + Fh780LatSweep(zb, cabHW, 0.105f) - 0.024f;
        float xTopF_a = windBrowX + 0.014f + Fh780LatSweep(za, cabHW, 0.105f) - 0.014f;
        float xTopF_b = windBrowX + 0.014f + Fh780LatSweep(zb, cabHW, 0.105f) - 0.014f;
        float xBack_a = windBrowX + 0.045f + Fh780LatSweep(za, cabHW, 0.105f);
        float xBack_b = windBrowX + 0.045f + Fh780LatSweep(zb, cabHW, 0.105f);
        float yb0 = windTopY - 0.01f, yb1 = windTopY + 0.12f;
        PushQuad(mesh.get(), Vec3(xBotF_a, yb0, za), Vec3(xBotF_b, yb0, zb), Vec3(xTopF_b, yb1, zb), Vec3(xTopF_a, yb1, za), glossBlack);
        PushQuad(mesh.get(), Vec3(xTopF_a, yb1, za), Vec3(xTopF_b, yb1, zb), Vec3(xBack_b, yb1, zb), Vec3(xBack_a, yb1, za), glossBlack);
        PushQuad(mesh.get(), Vec3(xBack_a, yb0, za), Vec3(xBack_b, yb0, zb), Vec3(xBotF_b, yb0, zb), Vec3(xBotF_a, yb0, za), glossBlack);
    }
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(windBrowX + 0.05f, windTopY - 0.01f, s > 0 ? cabHW - 0.06f : -cabHW - 0.005f), Vec3(windBrowX + 0.24f, windTopY + 0.12f, s > 0 ? cabHW + 0.005f : -cabHW + 0.06f), glossBlack);
    }

    // Illuminated Black Globetrotter "FH16 780" Lightbox Header [windTopY + 0.11 .. cabTopY]
    const float hdrBotY = windTopY + 0.11f;
    const float hdrTopY = hdrBotY + 0.24f; // ~3.51m
    AppendCurvedPatchX(mesh.get(), hdrBotY, hdrTopY, -cabHW + 0.10f, cabHW - 0.10f, 3, 14, 0.0f, roofSurf, glossBlack);
    AppendCurvedPatchX(mesh.get(), hdrTopY - 0.02f, cabTopY, -cabHW + 0.10f, cabHW - 0.10f, 5, 14, 0.0f, roofSurf, greenPaint);
    // White illuminated FH16 780 header letters & subtle cyan/blue square accent around "16" + corner clearance LEDs
    const char fhChars[] = {'F', 'H', '1', '6'};
    for (int i = 0; i < 4; ++i) {
        AppendSurfGlyph(mesh.get(), fhChars[i], hdrBotY + 0.055f, -0.56f + static_cast<float>(i) * 0.11f, 0.125f, 0.088f, 0.02f, 0.008f, roofSurf, whiteHeader);
    }
    const char n780Chars[] = {'7', '8', '0'};
    for (int i = 0; i < 3; ++i) {
        AppendSurfGlyph(mesh.get(), n780Chars[i], hdrBotY + 0.055f, 0.02f + static_cast<float>(i) * 0.135f, 0.125f, 0.105f, 0.02f, 0.008f, roofSurf, whiteHeader);
    }
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zc0 = (side > 0) ? 0.90f : -0.98f;
        float zc1 = (side > 0) ? 0.98f : -0.90f;
        AppendFlushBoxOnSurfX(mesh.get(), hdrBotY + 0.06f, hdrTopY - 0.05f, zc0, zc1, 0.008f, 0.012f, 1, roofSurf, amberLed);
        // Tubular Polished Chrome Roof Horn Bar Rails arcing over brow + Chrome Hadley Air Horns
        float zRail = s * 0.62f;
        for (int k = 0; k < 5; ++k) {
            float tk0 = static_cast<float>(k) / 5.0f, tk1 = static_cast<float>(k + 1) / 5.0f;
            float yk0 = hdrBotY + 0.06f + (cabTopY + 0.04f - (hdrBotY + 0.06f)) * tk0;
            float yk1 = hdrBotY + 0.06f + (cabTopY + 0.04f - (hdrBotY + 0.06f)) * tk1;
            float xk0 = roofSurf(yk0, zRail) - 0.040f;
            float xk1 = roofSurf(yk1, zRail) - 0.040f;
            RunningGear::AppendBox(mesh.get(), Vec3(xk0 - 0.024f, yk0, zRail - 0.022f), Vec3(xk1 + 0.024f, yk1 + 0.022f, zRail + 0.022f), chromeCol);
        }
        // Chrome Hadley Air Horn trumpet mounted near top corner of roof bar
        float hornX = roofSurf(cabTopY - 0.02f, s * 0.72f);
        ChassisComponents::AppendClosedCylinderX(mesh.get(), hornX - 0.16f, hornX + 0.24f, cabTopY + 0.02f, s * 0.72f, 0.036f, 12, chromeCol, chromeCol);
        AppendFrontDiscX(mesh.get(), hornX - 0.16f, cabTopY + 0.02f, s * 0.72f, 0.054f, 0.012f, 14, glossBlack, chromeCol);
    }
    // Horizontal chrome roof crossbar joining the twin chrome rails
    float barY = cabTopY - 0.05f;
    float barX = roofSurf(barY, 0.0f) - 0.042f;
    RunningGear::AppendBox(mesh.get(), Vec3(barX - 0.022f, barY, -0.64f), Vec3(barX + 0.026f, barY + 0.042f, 0.64f), chromeCol);

    // Smooth corner wrap from roof brow front edges back into cab sidewalls + header strip above doors
    RunningGear::AppendBox(mesh.get(), Vec3(windBrowX + 0.22f, doorTopY - 0.01f, -cabHW), Vec3(cabBackX - 0.04f, hdrBotY + 0.02f, -cabHW + 0.08f), greenPaint);
    RunningGear::AppendBox(mesh.get(), Vec3(windBrowX + 0.22f, doorTopY - 0.01f,  cabHW - 0.08f), Vec3(cabBackX - 0.04f, hdrBotY + 0.02f,  cabHW), greenPaint);
    for (int side : {-1, 1}) {
        bool rightSide = (side > 0);
        float s = static_cast<float>(side);
        float zInner = s * (cabHW - 0.10f), zOuter = s * cabHW;
        for (int iy = 0; iy < 6; ++iy) {
            float ya = hdrBotY + (cabTopY - hdrBotY) * (static_cast<float>(iy) / 6.0f);
            float yb = hdrBotY + (cabTopY - hdrBotY) * (static_cast<float>(iy + 1) / 6.0f);
            Vec4 col = (yb <= hdrTopY) ? glossBlack : greenPaint;
            float xa = roofSurf(ya, zInner), xb = roofSurf(yb, zInner);
            float xSideA = xa + 0.14f, xSideB = xb + 0.14f;
            if (rightSide) {
                PushQuad(mesh.get(), Vec3(xa, ya, zInner), Vec3(xSideA, ya, zOuter), Vec3(xSideB, yb, zOuter), Vec3(xb, yb, zInner), col);
            } else {
                PushQuad(mesh.get(), Vec3(xSideA, ya, zOuter), Vec3(xa, ya, zInner), Vec3(xb, yb, zInner), Vec3(xSideB, yb, zOuter), col);
            }
            float zWall0 = rightSide ? (cabHW - 0.06f) : -cabHW;
            float zWall1 = rightSide ? cabHW : -(cabHW - 0.06f);
            RunningGear::AppendBox(mesh.get(), Vec3(xSideB, ya, zWall0), Vec3(cabBackX - 0.04f, yb + 0.01f, zWall1), greenPaint);
        }
    }

    // Flush Curved Roof Top Deck (Green outer crown + gloss-black center sunroof insert) + Rear Spoiler
    const float roofEndX = cabBackX - 0.04f;
    for (int iz = 0; iz < 12; ++iz) {
        float tz0 = static_cast<float>(iz) / 12.0f, tz1 = static_cast<float>(iz + 1) / 12.0f;
        float z0 = -(cabHW - 0.08f) + 2.0f * (cabHW - 0.08f) * tz0;
        float z1 = -(cabHW - 0.08f) + 2.0f * (cabHW - 0.08f) * tz1;
        Vec4 deckCol = (std::abs(0.5f * (z0 + z1)) <= 0.56f) ? glossBlack : greenPaint;
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
    // Painted green aerodynamic upper roof spoiler blade at rear of cab [cabBackX - 0.58 .. cabBackX + 0.06]
    {
        float xS0 = cabBackX - 0.58f, xS1 = cabBackX + 0.06f;
        float yS0 = cabTopY, yS1 = std::min(H + 0.04f, cabTopY + 0.14f);
        float zW = cabHW - 0.08f;
        Vec3 fBL(xS0, cabTopY - 0.01f, -zW), fBR(xS0, cabTopY - 0.01f, zW), fTR(xS0, yS0 + 0.02f, zW), fTL(xS0, yS0 + 0.02f, -zW);
        Vec3 bBL(xS1, cabTopY - 0.01f, -zW), bBR(xS1, cabTopY - 0.01f, zW), bTR(xS1, yS1, zW), bTL(xS1, yS1, -zW);
        PushQuad(mesh.get(), fBL, fBR, fTR, fTL, greenPaint);
        PushQuad(mesh.get(), bBR, bBL, bTL, bTR, greenPaint);
        PushQuad(mesh.get(), fTL, fTR, bTR, bTL, greenPaint);
        PushQuad(mesh.get(), bBL, fBL, fTL, bTL, greenPaint);
        PushQuad(mesh.get(), fBR, bBR, bTR, fTR, greenPaint);
    }

    // Carved Rear Sleeper Walls + Trapezoidal Quarter Sleeper Windows + Aero Side Extender Panels
    std::vector<HullStationX> rearSleeper = {
        {doorX1,           cabClearY, cabTopY - 0.04f, cabHW,         0.10f, 0.12f, 0.0f, 0.0f},
        {cabBackX - 0.12f, cabClearY, cabTopY - 0.04f, cabHW,         0.10f, 0.14f, 0.0f, 0.0f},
        {cabBackX,         cabClearY + 0.04f, cabTopY - 0.04f, cabHW - 0.06f, 0.12f, 0.16f, 0.0f, 0.0f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), rearSleeper, greenPaint, 5);
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        // Extended aerodynamic green rear corner fairing [cabBackX - 0.04 .. cabBackX + 0.25]
        RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.04f, cabClearY + 0.04f, s > 0 ? cabHW - 0.05f : -cabHW), Vec3(cabBackX + 0.25f, cabTopY - 0.06f, s > 0 ? cabHW : -cabHW + 0.05f), greenPaint);
        // Rear sleeper dark tinted trapezoidal side window + black molding
        float zWin = s * (cabHW + 0.008f);
        RunningGear::AppendBox(mesh.get(), Vec3(doorX1 + 0.14f, doorBeltY + 0.06f, std::min(s * cabHW, zWin)), Vec3(cabBackX - 0.22f, doorTopY - 0.08f, std::max(s * cabHW, zWin)), glossBlack);
        // Silver horizontal pinstripe crease along sleeper flank ("780")
        AppendSideRibbonZ(mesh.get(), s * cabHW, 0.010f, side > 0, doorX1 + 0.04f, cabClearY + 0.50f, cabBackX - 0.08f, cabClearY + 0.62f, 0.014f, silverTrim);
    }

    // 6. Kerb-Dipped Doors + Green MirrorCam Pods + Chrome Door Handles
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.16f, false, false, greenPaint, glossBlack, sideGlass, doorBeltY, def.is_rhd, true);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.16f, false, false, greenPaint, glossBlack, sideGlass, doorBeltY, !def.is_rhd, true);
    AppendGreenMirrorCams(mesh.get(), xFront + 0.58f, doorTopY + 0.02f, cabHW, greenPaint, glossBlack, silverTrim);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.36f, windTopY - 0.12f, -cabHW, glossBlack);
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.48f, windTopY + 0.06f, cabHW * 0.85f, def.headlights_on);

    // 7. Green Painted Full Side Skirts + Painted Rear Drive Wheel Arches + Amber Side Markers
    float skirtStart = cabBackX + 0.10f;
    float skirtEnd   = std::max(skirtStart + 0.55f, driveCenterX - archHalfLen - 0.04f);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, bumperBotY + 0.04f, -hw + 0.03f), Vec3(skirtEnd, frameTopY + 0.04f, -hw + 0.10f), greenPaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, bumperBotY + 0.04f,  hw - 0.10f), Vec3(skirtEnd, frameTopY + 0.04f,  hw - 0.03f), greenPaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.18f, bumperBotY + 0.22f, -hw + 0.02f), Vec3(skirtStart + 0.30f, bumperBotY + 0.28f, -hw + 0.04f), amberLed);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.18f, bumperBotY + 0.22f,  hw - 0.04f), Vec3(skirtStart + 0.30f, bumperBotY + 0.28f,  hw - 0.02f), amberLed);
    CabComponents::AppendWheelArchFender(mesh.get(), driveCenterX, wheelR, wheelR + 0.08f, -hw, false, tireW * 1.15f, greenPaint);
    CabComponents::AppendWheelArchFender(mesh.get(), driveCenterX, wheelR, wheelR + 0.08f,  hw, true,  tireW * 1.15f, greenPaint);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.10f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.66f, frameTopY + 0.04f, 0.45f), silverTrim);

    // 8. Translucent Flush Curved Windshield & Door Glass Pass
    const float gHW = cabHW - 0.095f;
    for (int i = 0; i < 12; ++i) {
        float za = -gHW + (2.0f * gHW) * (static_cast<float>(i) / 12.0f);
        float zb = -gHW + (2.0f * gHW) * (static_cast<float>(i + 1) / 12.0f);
        float xBotA = lowerSurf(windBotY, za) + 0.008f;
        float xBotB = lowerSurf(windBotY, zb) + 0.008f;
        float xTopA = windBrowX + Fh780LatSweep(za, cabHW, 0.105f) + 0.006f;
        float xTopB = windBrowX + Fh780LatSweep(zb, cabHW, 0.105f) + 0.006f;
        PushQuad(mesh.get(), Vec3(xBotA, windBotY - 0.01f, za), Vec3(xBotB, windBotY - 0.01f, zb), Vec3(xTopB, windTopY + 0.01f, zb), Vec3(xTopA, windTopY + 0.01f, za), windshieldGlass);
    }
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.16f, false, true, greenPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.16f, false, true, greenPaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
