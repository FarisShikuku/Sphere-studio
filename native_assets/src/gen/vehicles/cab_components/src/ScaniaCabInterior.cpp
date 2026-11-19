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

inline void PushTriDirected(
    Mesh* mesh,
    const Vec3& p0, const Vec3& p1, const Vec3& p2,
    const Vec3& expectedDir,
    const Vec4& col
) {
    Vec3 e1 = p1 - p0;
    Vec3 e2 = p2 - p0;
    Vec3 n = Vec3::Cross(e1, e2);
    bool flip = (Vec3::Dot(n, expectedDir) < 0.0f);

    uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
    Vertex v0, v1, v2;
    v0.position = p0; v0.color = col;
    v1.position = flip ? p2 : p1; v1.color = col;
    v2.position = flip ? p1 : p2; v2.color = col;
    mesh->vertices.push_back(v0);
    mesh->vertices.push_back(v1);
    mesh->vertices.push_back(v2);
    mesh->indices.push_back(base + 0);
    mesh->indices.push_back(base + 1);
    mesh->indices.push_back(base + 2);
}

inline void PushQuadDirected(
    Mesh* mesh,
    const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d,
    const Vec3& expectedDir,
    const Vec4& col
) {
    PushTriDirected(mesh, a, b, c, expectedDir, col);
    PushTriDirected(mesh, a, c, d, expectedDir, col);
}

// Rigid box rotated around Y around (pivotX, pivotZ) maintaining strict outward-CCW winding
void AppendBoxYaw(
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

// Rigid box rotated around Z in X-Y plane (positive angle tips top toward -X forward, bottom +X)
void AppendBoxPitchZ(
    Mesh* mesh,
    const Vec3& minP, const Vec3& maxP,
    float pivotX, float pivotY,
    float angleRad,
    const Vec4& color
) {
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    auto xf = [&](float x, float y, float z) -> Vec3 {
        float dx = x - pivotX;
        float dy = y - pivotY;
        return Vec3(pivotX + dx * c - dy * s, pivotY + dx * s + dy * c, z);
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

// Combined yaw + pitch box in driver space for mounted tablet / tilted console pod
void AppendOrientedPod(
    Mesh* mesh,
    float halfX, float halfY, float halfZ,
    const Vec3& center,
    float pitchRad,
    float yawRad,
    const Vec4& color
) {
    float cp = std::cos(pitchRad), sp = std::sin(pitchRad);
    float cy = std::cos(yawRad), sy = std::sin(yawRad);
    auto xf = [&](float lx, float ly, float lz) -> Vec3 {
        // First pitch around Z in X-Y
        float px = lx * cp - ly * sp;
        float py = lx * sp + ly * cp;
        float pz = lz;
        // Then yaw around Y
        float rx = px * cy - pz * sy;
        float rz = px * sy + pz * cy;
        return Vec3(center.x + rx, center.y + py, center.z + rz);
    };
    uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
    Vec3 corners[8] = {
        xf(-halfX, -halfY,  halfZ), xf( halfX, -halfY,  halfZ),
        xf( halfX,  halfY,  halfZ), xf(-halfX,  halfY,  halfZ),
        xf(-halfX, -halfY, -halfZ), xf( halfX, -halfY, -halfZ),
        xf( halfX,  halfY, -halfZ), xf(-halfX,  halfY, -halfZ)
    };
    for (int i = 0; i < 8; ++i) {
        Vertex v;
        v.position = corners[i];
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

void AppendVerticalDiscPosX(
    Mesh* mesh,
    float faceX, float cy, float cz,
    float radius, float thickX,
    int segs, const Vec4& faceCol, const Vec4& rimCol
) {
    float xFront = faceX + thickX * 0.5f; // Facing +X toward driver
    float xBack  = faceX - thickX * 0.5f;
    Vec3 cF(xFront, cy, cz);
    Vec3 cB(xBack, cy, cz);
    Vec3 dirF(1.0f, 0.0f, 0.0f);
    Vec3 dirB(-1.0f, 0.0f, 0.0f);

    for (int i = 0; i < segs; ++i) {
        float a0 = (static_cast<float>(i) / static_cast<float>(segs)) * 2.0f * kPi;
        float a1 = (static_cast<float>(i + 1) / static_cast<float>(segs)) * 2.0f * kPi;
        Vec3 f0(xFront, cy + radius * std::sin(a0), cz + radius * std::cos(a0));
        Vec3 f1(xFront, cy + radius * std::sin(a1), cz + radius * std::cos(a1));
        Vec3 b0(xBack,  cy + radius * std::sin(a0), cz + radius * std::cos(a0));
        Vec3 b1(xBack,  cy + radius * std::sin(a1), cz + radius * std::cos(a1));
        Vec3 outDir(0.0f, std::sin((a0 + a1) * 0.5f), std::cos((a0 + a1) * 0.5f));
        PushTriDirected(mesh, cF, f0, f1, dirF, faceCol);
        PushTriDirected(mesh, cB, b0, b1, dirB, rimCol);
        PushQuadDirected(mesh, f0, f1, b1, b0, outDir, rimCol);
    }
}

} // namespace

void CabComponents::AppendScaniaCabInterior(
    Mesh* mesh,
    float xDashFront, float xCabRear,
    float floorY, float windBotY, float ceilingY,
    float cabHW,
    bool hasSleeperBunk,
    bool isRhd,
    float steerAngleDeg,
    const Vec4& dashColor,
    const Vec4& seatColor,
    const Vec4& accentColor
) {
    if (!mesh || xCabRear <= xDashFront + 0.7f) return;

    // Driver-relative Lateral Axis Convention:
    // Facing forward (-X), Up (+Y) in right-handed space -> Truck Left is +Z, Truck Right is -Z.
    // LHD (isRhd = false): Driver sits on Left (+Z, dSign = +1.0f), projects LEFT in Inside Cabin View.
    // RHD (isRhd = true) : Driver sits on Right (-Z, dSign = -1.0f), projects RIGHT in Inside Cabin View.
    const float dSign = isRhd ? -1.0f : 1.0f;
    const float inHW = cabHW - 0.055f;
    const float dashTopY = (windBotY > floorY + 0.42f) ? (windBotY - 0.015f) : (floorY + 0.90f);
    const float ceilY = std::max(dashTopY + 0.78f, ceilingY);

    auto MZ = [&](float u) -> float { return dSign * u; };
    auto MBox = [&](float x0, float y0, float u0, float x1, float y1, float u1, const Vec4& col) {
        float xMin = std::min(x0, x1), xMax = std::max(x0, x1);
        float yMin = std::min(y0, y1), yMax = std::max(y0, y1);
        float zMin = std::min(MZ(u0), MZ(u1)), zMax = std::max(MZ(u0), MZ(u1));
        if (xMax - xMin < 1e-4f || yMax - yMin < 1e-4f || zMax - zMin < 1e-4f) return;
        RunningGear::AppendBox(mesh, Vec3(xMin, yMin, zMin), Vec3(xMax, yMax, zMax), col);
    };

    // Rich Scania Next-Gen Interior Palette matching reference photo (cab-computer-top.png)
    const Vec4 dashCharcoal(
        std::clamp(dashColor.x * 1.15f, 0.13f, 0.19f),
        std::clamp(dashColor.y * 1.15f, 0.14f, 0.20f),
        std::clamp(dashColor.z * 1.15f, 0.16f, 0.23f),
        1.0f
    );
    const Vec4 dashUpperSlate(0.22f, 0.24f, 0.27f, 1.0f);
    const Vec4 dashLowerTaupe(0.32f, 0.34f, 0.36f, 1.0f);
    const Vec4 leatherBlack(0.11f, 0.12f, 0.14f, 1.0f);
    const Vec4 matRubberDark(0.08f, 0.09f, 0.10f, 1.0f);
    const Vec4 satinSilver(0.74f, 0.77f, 0.82f, 1.0f);
    const Vec4 stainlessSteel(0.68f, 0.71f, 0.75f, 1.0f);
    const Vec4 scaniaRed(0.84f, 0.15f, 0.17f, 1.0f);
    const Vec4 scaniaOrange(0.95f, 0.42f, 0.10f, 1.0f);
    const Vec4 headlinerGrey(0.76f, 0.78f, 0.82f, 1.0f);
    const Vec4 displayDark(0.06f, 0.07f, 0.09f, 1.0f);
    const Vec4 uiCyan(0.20f, 0.78f, 0.96f, 1.0f);
    const Vec4 uiAmber(0.94f, 0.72f, 0.22f, 1.0f);
    const Vec4 uiGreen(0.38f, 0.86f, 0.46f, 1.0f);
    const Vec4 leatherTan(0.58f, 0.31f, 0.16f, 1.0f);
    const Vec4 bottleGold(0.84f, 0.72f, 0.16f, 1.0f);
    const Vec4 seatFabric(
        std::clamp(seatColor.x, 0.14f, 0.26f),
        std::clamp(seatColor.y, 0.15f, 0.27f),
        std::clamp(seatColor.z, 0.17f, 0.30f),
        1.0f
    );

    const float uDriver = inHW * 0.50f; // ~0.585m from cab centerline

    // =========================================================================
    // 1. FLOORPAN, RIBBED FOOTWELL MAT, ENGINE TUNNEL & STAINLESS PEDALS
    // =========================================================================
    MBox(xDashFront - 0.10f, floorY - 0.045f, -inHW, xCabRear, floorY, inHW, matRubberDark);
    MBox(xDashFront + 0.10f, floorY, -0.22f, xDashFront + 1.05f, floorY + 0.16f, 0.22f, dashCharcoal);
    // Ribbed driver rubber mat + stainless footrest
    MBox(xDashFront + 0.42f, floorY + 0.004f, uDriver - 0.34f, xDashFront + 1.10f, floorY + 0.022f, uDriver + 0.32f, dashCharcoal);
    for (int rib = 0; rib < 6; ++rib) {
        float rx = xDashFront + 0.48f + static_cast<float>(rib) * 0.09f;
        MBox(rx, floorY + 0.022f, uDriver - 0.28f, rx + 0.028f, floorY + 0.032f, uDriver + 0.26f, matRubberDark);
    }
    // Tilted brake & throttle pedals
    AppendBoxPitchZ(mesh, Vec3(xDashFront + 0.34f, floorY + 0.11f, std::min(MZ(uDriver - 0.04f), MZ(uDriver + 0.09f))),
                    Vec3(xDashFront + 0.38f, floorY + 0.22f, std::max(MZ(uDriver - 0.04f), MZ(uDriver + 0.09f))),
                    xDashFront + 0.36f, floorY + 0.16f, 0.45f, satinSilver);
    AppendBoxPitchZ(mesh, Vec3(xDashFront + 0.31f, floorY + 0.09f, std::min(MZ(uDriver - 0.18f), MZ(uDriver - 0.11f))),
                    Vec3(xDashFront + 0.35f, floorY + 0.25f, std::max(MZ(uDriver - 0.18f), MZ(uDriver - 0.11f))),
                    xDashFront + 0.33f, floorY + 0.16f, 0.52f, satinSilver);

    // =========================================================================
    // 2. FULL WRAP-AROUND SCANIA DASHBOARD BASE (seals exterior lower nose cleanly!)
    // =========================================================================
    const float xPadFront = xDashFront - 0.14f; // Tucks right under windshield bottom
    const float xPadBack  = xDashFront + 0.38f;
    const float xFaceBack = xDashFront + 0.43f;

    // Upper full-width soft slate cowl deck + front defroster vent strip
    MBox(xPadFront, dashTopY - 0.09f, -inHW, xPadBack, dashTopY, inHW, dashUpperSlate);
    MBox(xPadFront + 0.04f, dashTopY - 0.005f, -0.75f, xPadFront + 0.14f, dashTopY + 0.012f, 0.75f, matRubberDark);
    // Windshield-base GPS/ADAS sensor pod (visible just right of dash center in photo)
    MBox(xPadFront + 0.04f, dashTopY, -0.14f, xPadFront + 0.13f, dashTopY + 0.07f, -0.04f, leatherBlack);

    // Lower full-width dash bulkhead + lighter grey lower glovebox/passenger knee trim
    MBox(xPadFront, floorY + 0.24f, -inHW, xPadBack - 0.02f, dashTopY - 0.08f, inHW, dashCharcoal);
    MBox(xPadBack - 0.05f, floorY + 0.26f, -inHW, xFaceBack, dashTopY - 0.32f, -0.26f, dashLowerTaupe);

    // Passenger-side sculpted upper glovebox bar, silver handle & dual vents
    MBox(xPadBack - 0.02f, dashTopY - 0.28f, -inHW + 0.04f, xFaceBack + 0.02f, dashTopY - 0.06f, -0.24f, dashCharcoal);
    MBox(xFaceBack, dashTopY - 0.18f, -0.82f, xFaceBack + 0.032f, dashTopY - 0.135f, -0.32f, satinSilver);
    // Passenger corner & mid satin-ringed vents
    for (float uVent : {-inHW + 0.14f, -0.36f}) {
        MBox(xFaceBack, dashTopY - 0.24f, uVent - 0.08f, xFaceBack + 0.03f, dashTopY - 0.11f, uVent + 0.08f, satinSilver);
        MBox(xFaceBack + 0.008f, dashTopY - 0.225f, uVent - 0.065f, xFaceBack + 0.036f, dashTopY - 0.125f, uVent + 0.065f, leatherBlack);
    }

    // =========================================================================
    // 3. DRIVER HOODED INSTRUMENT CLUSTER BINNACLE & OUTER VENT/SWITCH POD
    // =========================================================================
    const float yClustBot = dashTopY - 0.23f;
    const float yClustTop = dashTopY + 0.045f;
    // Sculpted overhanging binnacle brow hood
    MBox(xPadBack - 0.06f, yClustTop - 0.035f, uDriver - 0.27f, xFaceBack + 0.09f, yClustTop + 0.025f, uDriver + 0.27f, dashUpperSlate);
    MBox(xPadBack - 0.06f, yClustBot, uDriver - 0.27f, xFaceBack + 0.06f, yClustTop, uDriver - 0.23f, dashCharcoal);
    MBox(xPadBack - 0.06f, yClustBot, uDriver + 0.23f, xFaceBack + 0.06f, yClustTop, uDriver + 0.27f, dashCharcoal);

    // Matte black instrument cluster housing
    MBox(xPadBack - 0.02f, yClustBot + 0.01f, uDriver - 0.235f, xFaceBack + 0.015f, yClustTop - 0.02f, uDriver + 0.235f, displayDark);

    // Twin Scania circular dials (Speedometer on driver outer +u, Tachometer inner +u)
    const float yDial = (yClustBot + yClustTop) * 0.5f;
    const float xDialFace = xFaceBack + 0.020f;
    AppendVerticalDiscPosX(mesh, xDialFace, yDial, MZ(uDriver + 0.130f), 0.072f, 0.014f, 20, Vec4(0.14f, 0.16f, 0.20f, 1.0f), satinSilver);
    AppendVerticalDiscPosX(mesh, xDialFace + 0.005f, yDial, MZ(uDriver + 0.130f), 0.056f, 0.010f, 18, displayDark, Vec4(0.92f, 0.94f, 0.98f, 1.0f));
    AppendVerticalDiscPosX(mesh, xDialFace, yDial, MZ(uDriver - 0.130f), 0.072f, 0.014f, 20, Vec4(0.14f, 0.16f, 0.20f, 1.0f), satinSilver);
    AppendVerticalDiscPosX(mesh, xDialFace + 0.005f, yDial, MZ(uDriver - 0.130f), 0.056f, 0.010f, 18, displayDark, uiGreen);
    // Crisp glowing orange needles & dial ticks
    MBox(xDialFace + 0.006f, yDial - 0.006f, uDriver + 0.122f, xDialFace + 0.015f, yDial + 0.042f, uDriver + 0.138f, scaniaOrange);
    MBox(xDialFace + 0.006f, yDial - 0.006f, uDriver - 0.138f, xDialFace + 0.015f, yDial + 0.036f, uDriver - 0.100f, scaniaOrange);
    // Central Multi-Function TFT Color Driver Display
    MBox(xDialFace, yDial - 0.055f, uDriver - 0.068f, xDialFace + 0.014f, yDial + 0.055f, uDriver + 0.068f, Vec4(0.12f, 0.15f, 0.20f, 1.0f));
    MBox(xDialFace + 0.004f, yDial + 0.018f, uDriver - 0.052f, xDialFace + 0.018f, yDial + 0.038f, uDriver + 0.052f, uiAmber);
    MBox(xDialFace + 0.004f, yDial - 0.025f, uDriver - 0.048f, xDialFace + 0.018f, yDial - 0.008f, uDriver + 0.035f, uiCyan);

    // Outer driver air vent + light selector knob & M1/M2/EXT rocker switch bank
    MBox(xFaceBack, yClustBot - 0.06f, uDriver + 0.28f, xFaceBack + 0.035f, yClustTop - 0.06f, uDriver + 0.40f, satinSilver);
    MBox(xFaceBack + 0.01f, yClustBot - 0.05f, uDriver + 0.29f, xFaceBack + 0.042f, yClustTop - 0.07f, uDriver + 0.39f, leatherBlack);
    MBox(xFaceBack, yClustBot - 0.22f, uDriver + 0.24f, xFaceBack + 0.035f, yClustBot - 0.09f, uDriver + 0.44f, dashCharcoal);
    MBox(xFaceBack + 0.012f, yClustBot - 0.19f, uDriver + 0.28f, xFaceBack + 0.045f, yClustBot - 0.14f, uDriver + 0.40f, satinSilver);

    // =========================================================================
    // 4. FLAT-BOTTOMED 3-SPOKE SCANIA LEATHER STEERING WHEEL, GRIFFIN BADGE & STALKS
    // =========================================================================
    const float wheelCenterY = dashTopY - 0.145f;
    const float wheelCenterX = xFaceBack + 0.225f;
    const float wheelCenterZ = MZ(uDriver);

    // Sculpted steering column shroud & Opticruise right + indicator left stalks
    AppendBoxPitchZ(
        mesh,
        Vec3(xFaceBack - 0.04f, wheelCenterY - 0.055f, std::min(MZ(uDriver - 0.065f), MZ(uDriver + 0.065f))),
        Vec3(wheelCenterX - 0.04f, wheelCenterY + 0.055f, std::max(MZ(uDriver - 0.065f), MZ(uDriver + 0.065f))),
        xFaceBack + 0.10f, wheelCenterY - 0.03f, 0.28f, leatherBlack
    );
    // Column pedestal down to floor
    MBox(xFaceBack + 0.03f, floorY + 0.02f, uDriver - 0.055f, xFaceBack + 0.13f, wheelCenterY - 0.05f, uDriver + 0.055f, matRubberDark);
    // Column stalks (Opticruise selector toward center -u, lighting/wiper stalk toward +u)
    MBox(wheelCenterX - 0.075f, wheelCenterY + 0.025f, uDriver - 0.19f, wheelCenterX - 0.045f, wheelCenterY + 0.055f, uDriver - 0.05f, dashCharcoal);
    MBox(wheelCenterX - 0.075f, wheelCenterY + 0.025f, uDriver + 0.05f, wheelCenterX - 0.045f, wheelCenterY + 0.055f, uDriver + 0.18f, dashCharcoal);

    // 3D Steer Wheel tilted ~22 deg toward driver, rotated in wheel plane by steerAngleDeg * 3.5
    const float wheelTilt = 0.36f;
    const float ct = std::cos(wheelTilt), st = std::sin(wheelTilt);
    const float wheelRoll = dSign * (steerAngleDeg * 3.5f * kPi / 180.0f);
    const float cr = std::cos(wheelRoll), sr = std::sin(wheelRoll);
    const float wheelR = 0.205f;
    const float flatCutY = -wheelR * 0.78f;

    auto wheelPt = [&](float ryPreRoll, float ruPreRoll, float nxOffset) -> Vec3 {
        // Apply steering rotation in wheel plane
        float ry = ryPreRoll * cr - ruPreRoll * sr;
        float ru = ryPreRoll * sr + ruPreRoll * cr;
        // Tilt back around Z axis so normal aims up slightly toward driver eye
        float wx = wheelCenterX + nxOffset * ct - ry * st;
        float wy = wheelCenterY + nxOffset * st + ry * ct;
        float wz = wheelCenterZ + dSign * ru;
        return Vec3(wx, wy, wz);
    };

    // Wheel rim cross-section ring (24 segments with iconic Scania flat bottom)
    const int rimSegs = 24;
    const float tubeR = 0.019f;
    auto evalRimCenter = [&](float theta) -> std::pair<float, float> {
        float y0 = wheelR * std::sin(theta);
        float u0 = wheelR * std::cos(theta);
        if (y0 < flatCutY) {
            y0 = flatCutY;
        }
        return {y0, u0};
    };

    for (int i = 0; i < rimSegs; ++i) {
        float a0 = (static_cast<float>(i) / static_cast<float>(rimSegs)) * 2.0f * kPi;
        float a1 = (static_cast<float>(i + 1) / static_cast<float>(rimSegs)) * 2.0f * kPi;
        auto [yA, uA] = evalRimCenter(a0);
        auto [yB, uB] = evalRimCenter(a1);
        float xm = (a0 + a1) * 0.5f;
        float cA = std::cos(a0), sA = std::sin(a0);
        float cB = std::cos(a1), sB = std::sin(a1);

        // 4-sided chamfered leather grip profile around rim centerline
        Vec3 r0O = wheelPt(yA + tubeR * sA, uA + tubeR * cA,  0.0f);
        Vec3 r1O = wheelPt(yB + tubeR * sB, uB + tubeR * cB,  0.0f);
        Vec3 r0I = wheelPt(yA - tubeR * sA, uA - tubeR * cA,  0.0f);
        Vec3 r1I = wheelPt(yB - tubeR * sB, uB - tubeR * cB,  0.0f);
        Vec3 r0F = wheelPt(yA, uA,  tubeR);
        Vec3 r1F = wheelPt(yB, uB,  tubeR);
        Vec3 r0B = wheelPt(yA, uA, -tubeR);
        Vec3 r1B = wheelPt(yB, uB, -tubeR);

        Vec3 dOut = wheelPt(std::sin(xm), std::cos(xm), 0.0f) - Vec3(wheelCenterX, wheelCenterY, wheelCenterZ);
        Vec3 dIn  = Vec3(wheelCenterX, wheelCenterY, wheelCenterZ) - wheelPt(std::sin(xm), std::cos(xm), 0.0f);
        Vec3 dFront = wheelPt(0.0f, 0.0f, 1.0f) - Vec3(wheelCenterX, wheelCenterY, wheelCenterZ);

        PushQuadDirected(mesh, r0B, r0O, r1O, r1B, dOut - dFront, leatherBlack);
        PushQuadDirected(mesh, r0O, r0F, r1F, r1O, dOut + dFront, leatherBlack);
        PushQuadDirected(mesh, r0F, r0I, r1I, r1F, dIn + dFront,  leatherBlack);
        PushQuadDirected(mesh, r0I, r0B, r1B, r1I, dIn - dFront,  leatherBlack);
    }

    auto emitWheelBlock = [&](float yMin, float yMax, float uMin, float uMax, float xMin, float xMax, const Vec4& col) {
        Vec3 c000 = wheelPt(yMin, uMin, xMin), c100 = wheelPt(yMin, uMin, xMax);
        Vec3 c010 = wheelPt(yMax, uMin, xMin), c110 = wheelPt(yMax, uMin, xMax);
        Vec3 c001 = wheelPt(yMin, uMax, xMin), c101 = wheelPt(yMin, uMax, xMax);
        Vec3 c011 = wheelPt(yMax, uMax, xMin), c111 = wheelPt(yMax, uMax, xMax);
        Vec3 ctr = wheelPt((yMin + yMax) * 0.5f, (uMin + uMax) * 0.5f, (xMin + xMax) * 0.5f);
        auto faceOut = [&](const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d) {
            Vec3 fc = (a + b + c + d) * 0.25f;
            PushQuadDirected(mesh, a, b, c, d, fc - ctr, col);
        };
        faceOut(c100, c101, c111, c110); // Front (+driver face)
        faceOut(c001, c000, c010, c011); // Back
        faceOut(c010, c110, c111, c011); // Top
        faceOut(c000, c001, c101, c100); // Bottom
        faceOut(c000, c100, c110, c010); // Outer/Inner side A
        faceOut(c001, c011, c111, c101); // Outer/Inner side B
    };

    // Satin-silver 3-spoke Y-frame & lower V spoke bars
    emitWheelBlock(-0.025f, 0.028f, -wheelR + 0.015f, -0.055f, -0.016f, 0.016f, dashCharcoal);
    emitWheelBlock(-0.025f, 0.028f,  0.055f, wheelR - 0.015f, -0.016f, 0.016f, dashCharcoal);
    emitWheelBlock(-0.022f, 0.018f, -0.175f, -0.068f, 0.010f, 0.022f, satinSilver);
    emitWheelBlock(-0.022f, 0.018f,  0.068f,  0.175f, 0.010f, 0.022f, satinSilver);
    // Satin silver twin V-spokes descending to flat-bottom spoke
    emitWheelBlock(flatCutY + 0.010f, -0.030f, -0.072f, -0.044f, -0.010f, 0.022f, satinSilver);
    emitWheelBlock(flatCutY + 0.010f, -0.030f,  0.044f,  0.072f, -0.010f, 0.022f, satinSilver);
    emitWheelBlock(flatCutY + 0.005f, flatCutY + 0.042f, -0.095f, 0.095f, -0.012f, 0.018f, satinSilver);
    // Multi-function thumb switch clusters on left/right spokes
    emitWheelBlock(-0.020f, 0.022f, -0.155f, -0.088f, 0.016f, 0.028f, leatherBlack);
    emitWheelBlock(-0.020f, 0.022f,  0.088f,  0.155f, 0.016f, 0.028f, leatherBlack);

    // Central Trapezoid Airbag Pad + Iconic Red & Chrome Scania Griffin Emblem
    emitWheelBlock(-0.075f, 0.065f, -0.078f, 0.078f, -0.020f, 0.024f, leatherBlack);
    Vec3 padFrontDir = wheelPt(0.0f, 0.0f, 1.0f) - Vec3(wheelCenterX, wheelCenterY, wheelCenterZ);
    const int griffinSegs = 16;
    Vec3 gCenterChrome = wheelPt(0.012f, 0.0f, 0.026f);
    Vec3 gCenterRed    = wheelPt(0.012f, 0.0f, 0.028f);
    for (int i = 0; i < griffinSegs; ++i) {
        float a0 = (static_cast<float>(i) / static_cast<float>(griffinSegs)) * 2.0f * kPi;
        float a1 = (static_cast<float>(i + 1) / static_cast<float>(griffinSegs)) * 2.0f * kPi;
        PushTriDirected(mesh, gCenterChrome,
                        wheelPt(0.012f + 0.030f * std::sin(a0), 0.030f * std::cos(a0), 0.026f),
                        wheelPt(0.012f + 0.030f * std::sin(a1), 0.030f * std::cos(a1), 0.026f),
                        padFrontDir, satinSilver);
        PushTriDirected(mesh, gCenterRed,
                        wheelPt(0.012f + 0.023f * std::sin(a0), 0.023f * std::cos(a0), 0.028f),
                        wheelPt(0.012f + 0.023f * std::sin(a1), 0.023f * std::cos(a1), 0.028f),
                        padFrontDir, scaniaRed);
    }

    // =========================================================================
    // 5. ANGLED WRAP-AROUND CENTER STACK (INFOTAINMENT, PARK BRAKE, 21C AC, TOGGLES)
    // =========================================================================
    const float stackPivotX = xFaceBack + 0.02f;
    const float stackPivotU = -0.02f;
    const float stackPivotZ = MZ(stackPivotU);
    const float stackYaw = dSign * 0.28f; // ~16 deg turned smoothly toward driver

    auto StackBox = [&](float x0, float y0, float u0, float x1, float y1, float u1, const Vec4& col) {
        float xMin = std::min(x0, x1), xMax = std::max(x0, x1);
        float yMin = std::min(y0, y1), yMax = std::max(y0, y1);
        float zMin = std::min(MZ(u0), MZ(u1)), zMax = std::max(MZ(u0), MZ(u1));
        AppendBoxYaw(mesh, Vec3(xMin, yMin, zMin), Vec3(xMax, yMax, zMax), stackPivotX, stackPivotZ, stackYaw, col);
    };

    // Main angled center stack fascia housing (replaces flat block with contoured cockpit surround)
    const float yStackBot = floorY + 0.30f;
    const float yStackTop = dashTopY - 0.01f;
    StackBox(stackPivotX - 0.14f, yStackBot, -0.26f, stackPivotX + 0.045f, yStackTop, 0.26f, dashCharcoal);
    // Satin silver outer trim surround around top infotainment zone
    StackBox(stackPivotX + 0.038f, dashTopY - 0.27f, -0.22f, stackPivotX + 0.052f, dashTopY - 0.035f, 0.16f, satinSilver);

    // Trailer / Tractor Park Brake Lever (`PUSH TO APPLY (P)`) on driver side of stack
    StackBox(stackPivotX + 0.038f, dashTopY - 0.24f, 0.165f, stackPivotX + 0.068f, dashTopY - 0.045f, 0.235f, leatherBlack);
    StackBox(stackPivotX + 0.060f, dashTopY - 0.15f, 0.182f, stackPivotX + 0.115f, dashTopY - 0.075f, 0.218f, satinSilver);
    StackBox(stackPivotX + 0.095f, dashTopY - 0.09f, 0.176f, stackPivotX + 0.128f, dashTopY - 0.035f, 0.224f, leatherBlack);
    // Red Hazard Warning Triangle button above park brake!
    StackBox(stackPivotX + 0.040f, dashTopY - 0.055f, 0.242f, stackPivotX + 0.065f, dashTopY - 0.012f, 0.295f, scaniaRed);

    // Scania Infotainment Screen & Flanking Tactile Button Banks
    StackBox(stackPivotX + 0.042f, dashTopY - 0.255f, -0.135f, stackPivotX + 0.056f, dashTopY - 0.050f, 0.148f, displayDark);
    // Glowing Infotainment Media UI (BANDIT header, emerald status ribbon & amber EQ bars)
    StackBox(stackPivotX + 0.048f, dashTopY - 0.225f, -0.092f, stackPivotX + 0.060f, dashTopY - 0.078f, 0.106f, Vec4(0.08f, 0.11f, 0.14f, 1.0f));
    StackBox(stackPivotX + 0.052f, dashTopY - 0.105f, -0.084f, stackPivotX + 0.064f, dashTopY - 0.084f, 0.098f, uiGreen);
    StackBox(stackPivotX + 0.052f, dashTopY - 0.165f, -0.078f, stackPivotX + 0.064f, dashTopY - 0.132f, 0.092f, uiAmber);
    StackBox(stackPivotX + 0.052f, dashTopY - 0.205f, -0.078f, stackPivotX + 0.064f, dashTopY - 0.180f, 0.045f, satinSilver);
    // Dual rotary infotainment knobs + vertical AC vent to passenger side
    StackBox(stackPivotX + 0.048f, dashTopY - 0.245f, 0.112f, stackPivotX + 0.074f, dashTopY - 0.212f, 0.142f, satinSilver);
    StackBox(stackPivotX + 0.048f, dashTopY - 0.245f, -0.132f, stackPivotX + 0.074f, dashTopY - 0.212f, -0.102f, satinSilver);
    StackBox(stackPivotX + 0.038f, dashTopY - 0.265f, -0.218f, stackPivotX + 0.055f, dashTopY - 0.055f, -0.148f, leatherBlack);

    // Automatic Climate Control Panel (`21.0°C` digital strip) & Rocker Switch Bank
    const float yAcTop = dashTopY - 0.285f;
    const float yAcBot = yAcTop - 0.120f;
    StackBox(stackPivotX + 0.038f, yAcBot, -0.18f, stackPivotX + 0.058f, yAcTop, 0.18f, displayDark);
    StackBox(stackPivotX + 0.046f, yAcTop - 0.042f, -0.09f, stackPivotX + 0.062f, yAcTop - 0.015f, 0.09f, Vec4(0.90f, 0.94f, 0.98f, 1.0f)); // 21.0 C readout
    StackBox(stackPivotX + 0.048f, yAcBot + 0.018f, 0.125f, stackPivotX + 0.074f, yAcTop - 0.025f, 0.165f, satinSilver);
    StackBox(stackPivotX + 0.048f, yAcBot + 0.018f, -0.165f, stackPivotX + 0.074f, yAcTop - 0.025f, -0.125f, satinSilver);
    // Lower traction/suspension/axle-lock toggle row
    const float ySwBot = yAcBot - 0.105f;
    for (int sw = 0; sw < 5; ++sw) {
        float uSw = -0.13f + static_cast<float>(sw) * 0.065f;
        StackBox(stackPivotX + 0.042f, ySwBot + 0.012f, uSw - 0.022f, stackPivotX + 0.062f, yAcBot - 0.015f, uSw + 0.022f,
                 (sw == 2) ? uiAmber : dashUpperSlate);
    }

    // =========================================================================
    // 6. SIGNATURE PROTRUDING WORKSTATION TRAY: TRAVEL MUG, CAB TABLET, POUCH, NOTEBOOK & BOTTLE
    // =========================================================================
    const float trayPivotX = stackPivotX + 0.16f;
    const float trayPivotU = -0.10f;
    const float trayPivotZ = MZ(trayPivotU);
    const float trayYaw    = dSign * 0.22f;
    const float yTrayTop   = std::max(floorY + 0.24f, ySwBot - 0.045f);
    const float yTrayBot   = yTrayTop - 0.095f;

    auto TrayBox = [&](float x0, float y0, float u0, float x1, float y1, float u1, const Vec4& col) {
        float xMin = std::min(x0, x1), xMax = std::max(x0, x1);
        float yMin = std::min(y0, y1), yMax = std::max(y0, y1);
        float zMin = std::min(MZ(u0), MZ(u1)), zMax = std::max(MZ(u0), MZ(u1));
        AppendBoxYaw(mesh, Vec3(xMin, yMin, zMin), Vec3(xMax, yMax, zMax), trayPivotX, trayPivotZ, trayYaw, col);
    };

    // Protruding contoured workstation console body + recessed grip mat + lower silver pull drawer
    TrayBox(stackPivotX - 0.02f, yTrayBot, -0.34f, trayPivotX + 0.19f, yTrayTop, 0.20f, dashUpperSlate);
    TrayBox(stackPivotX + 0.02f, yTrayBot - 0.06f, -0.26f, trayPivotX + 0.14f, yTrayBot, 0.14f, dashLowerTaupe);
    TrayBox(trayPivotX + 0.02f, yTrayTop - 0.005f, -0.31f, trayPivotX + 0.17f, yTrayTop + 0.012f, 0.17f, matRubberDark);

    // 6a. Stainless Steel Travel Mug with Black Lid & Handle (center-left of tray in photo)
    const float mugX = trayPivotX + 0.10f;
    const float mugZ = MZ(0.015f);
    ChassisComponents::AppendClosedCylinderY(mesh, mugX, mugZ, yTrayTop + 0.005f, yTrayTop + 0.135f, 0.046f, 14, stainlessSteel);
    ChassisComponents::AppendClosedCylinderY(mesh, mugX, mugZ, yTrayTop + 0.132f, yTrayTop + 0.165f, 0.048f, 14, leatherBlack);
    TrayBox(mugX + 0.032f, yTrayTop + 0.04f, 0.002f, mugX + 0.068f, yTrayTop + 0.115f, 0.028f, leatherBlack);

    // 6b. Mounted Cab Computer / Tablet (iconic "cab-computer-top.png" tablet on bracket facing driver!)
    const Vec3 tabPos(trayPivotX + 0.055f, yTrayTop + 0.165f, MZ(-0.145f));
    // Clamp bracket arm rising from tray edge
    TrayBox(trayPivotX - 0.03f, yTrayTop, -0.165f, trayPivotX + 0.04f, yTrayTop + 0.17f, -0.125f, dashCharcoal);
    // Tablet slate bezel tilted toward driver eye
    AppendOrientedPod(mesh, 0.010f, 0.112f, 0.082f, tabPos, 0.34f, dSign * 0.26f, leatherBlack);
    // Vivid Cyan/Sky Lock/Home Wallpaper behind app icons
    AppendOrientedPod(mesh, 0.004f, 0.098f, 0.072f, Vec3(tabPos.x + 0.008f, tabPos.y, tabPos.z), 0.34f, dSign * 0.26f, uiCyan);
    // Row of 4x3 colorful cab-computer app icons + bottom dock bar!
    const Vec4 appCols[4] = {uiGreen, uiAmber, scaniaRed, Vec4(0.95f, 0.96f, 0.98f, 1.0f)};
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            float ry = 0.052f - static_cast<float>(row) * 0.038f;
            float rz = -0.042f + static_cast<float>(col) * 0.042f;
            Vec3 iPos(tabPos.x + 0.011f + ry * 0.30f, tabPos.y + ry, tabPos.z + dSign * rz);
            AppendOrientedPod(mesh, 0.003f, 0.011f, 0.011f, iPos, 0.34f, dSign * 0.26f, appCols[(row + col) % 4]);
        }
    }

    // 6c. Tan Leather Pouch + Black Notebook + Bright Orange Pen on lower right tray
    TrayBox(trayPivotX + 0.05f, yTrayTop + 0.010f, -0.175f, trayPivotX + 0.14f, yTrayTop + 0.036f, -0.055f, leatherTan);
    TrayBox(trayPivotX + 0.04f, yTrayTop + 0.010f, -0.295f, trayPivotX + 0.15f, yTrayTop + 0.028f, -0.185f, leatherBlack);
    TrayBox(trayPivotX + 0.04f, yTrayTop + 0.014f, -0.245f, trayPivotX + 0.085f, yTrayTop + 0.030f, -0.188f, scaniaOrange);
    TrayBox(trayPivotX + 0.07f, yTrayTop + 0.028f, -0.265f, trayPivotX + 0.135f, yTrayTop + 0.038f, -0.205f, scaniaOrange);

    // 6d. Golden Yellow Beverage Bottle w/ White Cap at outer passenger tray corner
    const float botX = stackPivotX + 0.09f;
    const float botZ = MZ(-0.275f);
    ChassisComponents::AppendClosedCylinderY(mesh, botX, botZ, yTrayTop + 0.010f, yTrayTop + 0.145f, 0.036f, 12, bottleGold);
    ChassisComponents::AppendClosedCylinderY(mesh, botX, botZ, yTrayTop + 0.142f, yTrayTop + 0.175f, 0.018f, 10, Vec4(0.95f, 0.96f, 0.98f, 1.0f));

    // =========================================================================
    // 7. A-PILLAR GRAB HANDLES, OVERHEAD STORAGE LOCKERS, HEADLINER & REAR SLEEPER
    // =========================================================================
    for (int side : {-1, 1}) {
        float uSide = static_cast<float>(side) * (inHW - 0.045f);
        // Inner sculpted A-pillar trim
        MBox(xDashFront - 0.12f, dashTopY - 0.02f, uSide - 0.03f, xDashFront + 0.22f, ceilY - 0.08f, uSide + 0.04f, dashCharcoal);
        // Signature vertical curved A-pillar cab entry grab handle
        ChassisComponents::AppendClosedCylinderY(
            mesh, xDashFront + 0.20f, MZ(static_cast<float>(side) * (inHW - 0.105f)),
            dashTopY + 0.08f, dashTopY + 0.48f, 0.018f, 10, dashUpperSlate
        );
    }

    // Overhead Front Windshield Storage Lockers + Interior Mirror + Dome Lamp
    const float yOverBot = (windBotY > floorY + 0.4f) ? (windBotY + 0.80f) : (ceilY - 0.26f);
    const float yHeaderBot = std::max(dashTopY + 0.72f, std::min(ceilY - 0.16f, yOverBot));
    MBox(xDashFront + 0.05f, yHeaderBot, -inHW + 0.06f, xDashFront + 0.52f, ceilY, inHW - 0.06f, dashUpperSlate);
    for (float uLock : {-0.58f, 0.0f, 0.58f}) {
        MBox(xDashFront + 0.49f, yHeaderBot + 0.02f, uLock - 0.24f, xDashFront + 0.535f, ceilY - 0.03f, uLock + 0.24f, headlinerGrey);
        MBox(xDashFront + 0.525f, yHeaderBot + 0.045f, uLock - 0.05f, xDashFront + 0.545f, yHeaderBot + 0.075f, uLock + 0.05f, dashCharcoal);
    }
    // Interior cab roof headliner & acoustic rear cab bulkhead
    MBox(xDashFront + 0.05f, ceilY, -inHW, xCabRear, ceilY + 0.045f, inHW, headlinerGrey);
    MBox(xCabRear - 0.055f, floorY, -inHW, xCabRear, ceilY, inHW, headlinerGrey);

    // =========================================================================
    // 8. CONTOURED SCANIA AIR-RIDE SEATS W/ ORANGE PIPING & SLEEPER BUNK
    // =========================================================================
    const float seatX = std::clamp(xDashFront + 1.24f, xDashFront + 1.08f, xCabRear - 0.34f);
    for (int side : {-1, 1}) {
        float uS = static_cast<float>(side) * uDriver;
        AppendSeat(mesh, Vec3(seatX, floorY, MZ(uS)), -1.0f, 0.54f, 1.06f, seatFabric, leatherBlack, dashCharcoal);
        // Scania orange contrast stitching / piping accents along seat cushion & backrest shoulders
        MBox(seatX - 0.20f, floorY + 0.52f, uS - 0.26f, seatX + 0.14f, floorY + 0.545f, uS - 0.23f, scaniaOrange);
        MBox(seatX - 0.20f, floorY + 0.52f, uS + 0.23f, seatX + 0.14f, floorY + 0.545f, uS + 0.26f, scaniaOrange);
        MBox(seatX + 0.11f, floorY + 0.55f, uS - 0.24f, seatX + 0.19f, floorY + 0.92f,  uS - 0.21f, scaniaOrange);
        MBox(seatX + 0.11f, floorY + 0.55f, uS + 0.21f, seatX + 0.19f, floorY + 0.92f,  uS + 0.24f, scaniaOrange);
        // Inboard folding armrest
        float uArm = (side > 0) ? (uS - 0.31f) : (uS + 0.31f);
        MBox(seatX - 0.16f, floorY + 0.62f, uArm - 0.04f, seatX + 0.12f, floorY + 0.69f, uArm + 0.04f, dashUpperSlate);
    }

    // Sleeper berth bunk behind seats if sleeper cab (R / S series) + enclosed lower side trim for 360 head turns
    if (hasSleeperBunk && (xCabRear - seatX) > 0.55f) {
        const float bunkX0 = seatX + 0.30f;
        MBox(bunkX0, floorY, -inHW, xCabRear - 0.05f, floorY + 0.36f, inHW, dashCharcoal);
        MBox(bunkX0 + 0.02f, floorY + 0.36f, -inHW + 0.04f, xCabRear - 0.06f, floorY + 0.50f, inHW - 0.04f, Vec4(0.84f, 0.86f, 0.90f, 1.0f));
        MBox(bunkX0 + 0.08f, floorY + 0.50f, uDriver - 0.25f, bunkX0 + 0.40f, floorY + 0.58f, uDriver + 0.25f, accentColor);
        // Inner sleeper side walls up to window sill + rear C-pillars so turning 135-180 deg sees trimmed interior
        float sleepWallX0 = std::max(seatX + 0.14f, xCabRear - 0.82f);
        if (xCabRear - sleepWallX0 > 0.08f) {
            MBox(sleepWallX0, floorY + 0.34f, -inHW - 0.025f, xCabRear - 0.04f, dashTopY + 0.08f, -inHW + 0.015f, dashCharcoal);
            MBox(sleepWallX0, floorY + 0.34f,  inHW - 0.015f, xCabRear - 0.04f, dashTopY + 0.08f,  inHW + 0.025f, dashCharcoal);
            MBox(xCabRear - 0.22f, dashTopY + 0.08f, -inHW - 0.025f, xCabRear - 0.04f, ceilY, -inHW + 0.015f, headlinerGrey);
            MBox(xCabRear - 0.22f, dashTopY + 0.08f,  inHW - 0.015f, xCabRear - 0.04f, ceilY,  inHW + 0.025f, headlinerGrey);
        }
    }

    // =========================================================================
    // 9. ANCHOR EXACT DRIVER INTERIOR EYE CAMERA ON MESH FOR 360 HEAD ROTATION
    // =========================================================================
    mesh->hasInteriorEye = true;
    mesh->interiorEyePos = Vec3(
        xDashFront + 1.00f,      // Neck pivot behind wheel & ahead of backrest for natural 360 head turns
        dashTopY + 0.22f,        // Seated driver eye line clearing seat shoulders cleanly when looking back
        MZ(uDriver * 0.72f)      // Centered in driver zone with full cockpit & door switch pod visibility
    );
}

} // namespace sphere::vehicles::cab
