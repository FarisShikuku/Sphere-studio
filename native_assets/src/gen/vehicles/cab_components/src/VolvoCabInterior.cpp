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

void AppendVerticalDiscNegX(
    Mesh* mesh,
    float faceX, float cy, float cz,
    float radius, float thickX,
    int segs, const Vec4& faceCol, const Vec4& rimCol
) {
    float xFront = faceX - thickX * 0.5f; // Facing -X visible through windshield
    float xBack  = faceX + thickX * 0.5f;
    Vec3 cF(xFront, cy, cz);
    Vec3 cB(xBack, cy, cz);
    Vec3 dirF(-1.0f, 0.0f, 0.0f);
    Vec3 dirB(1.0f, 0.0f, 0.0f);

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

void CabComponents::AppendVolvoCabInterior(
    Mesh* mesh,
    float xDashFront, float xCabRear,
    float floorY, float windBotY, float ceilingY,
    float cabHW,
    bool hasSleeperBunk,
    bool isRhd,
    float steerAngleDeg,
    bool ambientGlowOn,
    const Vec4& dashColor,
    const Vec4& seatColor,
    const Vec4& accentColor,
    const Vec4& ambientColor
) {
    if (!mesh || xCabRear <= xDashFront + 0.7f) return;

    // Facing forward (-X), Up (+Y) -> Truck Left is +Z, Truck Right is -Z.
    // LHD (isRhd = false): driver on +Z (dSign = +1.0f).
    // RHD (isRhd = true) : driver on -Z (dSign = -1.0f).
    const float dSign = isRhd ? -1.0f : 1.0f;
    const float inHW = cabHW - 0.055f;
    const float dashTopY = (windBotY > floorY + 0.42f) ? (windBotY - 0.025f) : (floorY + 0.80f);
    // Euro COE FH high-cab floor sits realistically ~0.78m below windshield sill so wheel & seats align!
    const float effFloorY = (windBotY > floorY + 0.90f) ? (windBotY - 0.78f) : floorY;
    const float ceilY = std::max(dashTopY + 0.82f, ceilingY);

    auto MZ = [&](float u) -> float { return dSign * u; };
    auto MBox = [&](float x0, float y0, float u0, float x1, float y1, float u1, const Vec4& col) {
        float xMin = std::min(x0, x1), xMax = std::max(x0, x1);
        float yMin = std::min(y0, y1), yMax = std::max(y0, y1);
        float zMin = std::min(MZ(u0), MZ(u1)), zMax = std::max(MZ(u0), MZ(u1));
        if (xMax - xMin < 1e-4f || yMax - yMin < 1e-4f || zMax - zMin < 1e-4f) return;
        RunningGear::AppendBox(mesh, Vec3(xMin, yMin, zMin), Vec3(xMax, yMax, zMax), col);
    };

    const Vec4 dashCharcoal(
        std::clamp(dashColor.x * 1.15f, 0.12f, 0.18f),
        std::clamp(dashColor.y * 1.15f, 0.13f, 0.19f),
        std::clamp(dashColor.z * 1.15f, 0.15f, 0.22f),
        1.0f
    );
    const Vec4 dashUpperSlate(0.20f, 0.22f, 0.25f, 1.0f);
    const Vec4 leatherBlack(0.10f, 0.11f, 0.13f, 1.0f);
    const Vec4 matRubberDark(0.07f, 0.08f, 0.09f, 1.0f);
    const Vec4 satinSilver(0.78f, 0.81f, 0.86f, 1.0f);
    const Vec4 headlinerDark(0.24f, 0.26f, 0.28f, 1.0f);
    const Vec4 displayDark(0.06f, 0.07f, 0.09f, 1.0f);
    const Vec4 uiCyan(0.20f, 0.80f, 0.96f, 1.0f);
    const Vec4 glowStrip = ambientGlowOn ? ambientColor : Vec4(0.16f, 0.30f, 0.20f, 1.0f);
    const Vec4 seatQuilt(
        std::clamp(seatColor.x * 1.12f, 0.14f, 0.34f),
        std::clamp(seatColor.y * 1.15f, 0.16f, 0.42f),
        std::clamp(seatColor.z * 1.05f, 0.12f, 0.28f),
        1.0f
    );

    const float uDriver = inHW * 0.50f;

    // 1. Flat-Floor Cab Pan, Low Engine Tunnel & Stainless Pedals
    MBox(xDashFront - 0.10f, effFloorY - 0.045f, -inHW, xCabRear, effFloorY, inHW, matRubberDark);
    MBox(xDashFront + 0.12f, effFloorY, -0.22f, xDashFront + 1.05f, effFloorY + 0.09f, 0.22f, dashCharcoal);
    AppendBoxPitchZ(
        mesh,
        Vec3(xDashFront + 0.32f, effFloorY + 0.09f, std::min(MZ(uDriver - 0.06f), MZ(uDriver + 0.09f))),
        Vec3(xDashFront + 0.36f, effFloorY + 0.21f, std::max(MZ(uDriver - 0.06f), MZ(uDriver + 0.09f))),
        xDashFront + 0.34f, effFloorY + 0.15f, 0.46f, satinSilver
    );

    // 2. Sculpted Volvo FH Wrap-Around Dashboard & Metallic Accent Strip
    const float xPadFront = xDashFront - 0.14f;
    const float xPadBack  = xDashFront + 0.38f;
    const float xFaceBack = xDashFront + 0.43f;

    MBox(xPadFront, dashTopY - 0.08f, -inHW, xPadBack, dashTopY, inHW, dashUpperSlate);
    MBox(xPadFront, effFloorY + 0.22f, -inHW, xPadBack - 0.02f, dashTopY - 0.08f, inHW, dashCharcoal);
    MBox(xPadBack - 0.01f, dashTopY - 0.13f, -inHW + 0.06f, xFaceBack + 0.015f, dashTopY - 0.095f, -0.26f, satinSilver);
    if (ambientGlowOn) {
        MBox(xPadBack - 0.01f, effFloorY + 0.24f, -inHW + 0.10f, xFaceBack + 0.01f, effFloorY + 0.27f, inHW - 0.10f, glowStrip);
    }

    // 3. 12-inch Full Digital Volvo Instrument Display + Angled Side Infotainment
    const float yClustBot = dashTopY - 0.22f;
    const float yClustTop = dashTopY + 0.04f;
    MBox(xPadBack - 0.04f, yClustTop - 0.03f, uDriver - 0.25f, xFaceBack + 0.07f, yClustTop + 0.02f, uDriver + 0.25f, dashUpperSlate);
    MBox(xPadBack - 0.01f, yClustBot, uDriver - 0.23f, xFaceBack + 0.015f, yClustTop - 0.025f, uDriver + 0.23f, displayDark);
    MBox(xFaceBack + 0.014f, yClustBot + 0.02f, uDriver - 0.20f, xFaceBack + 0.024f, yClustTop - 0.04f, uDriver + 0.20f, uiCyan);
    MBox(xFaceBack + 0.018f, yClustBot + 0.04f, uDriver - 0.07f, xFaceBack + 0.028f, yClustTop - 0.06f, uDriver + 0.07f, glowStrip);

    const float stackPivotX = xFaceBack + 0.02f;
    const float stackPivotZ = MZ(-0.04f);
    const float stackYaw = dSign * 0.26f;
    auto StackBox = [&](float x0, float y0, float u0, float x1, float y1, float u1, const Vec4& col) {
        float xMin = std::min(x0, x1), xMax = std::max(x0, x1);
        float yMin = std::min(y0, y1), yMax = std::max(y0, y1);
        float zMin = std::min(MZ(u0), MZ(u1)), zMax = std::max(MZ(u0), MZ(u1));
        AppendBoxYaw(mesh, Vec3(xMin, yMin, zMin), Vec3(xMax, yMax, zMax), stackPivotX, stackPivotZ, stackYaw, col);
    };
    StackBox(stackPivotX - 0.12f, effFloorY + 0.28f, -0.24f, stackPivotX + 0.04f, dashTopY - 0.02f, 0.22f, dashCharcoal);
    StackBox(stackPivotX + 0.036f, dashTopY - 0.24f, -0.14f, stackPivotX + 0.056f, dashTopY - 0.05f, 0.14f, displayDark);
    StackBox(stackPivotX + 0.046f, dashTopY - 0.21f, -0.11f, stackPivotX + 0.062f, dashTopY - 0.075f, 0.11f, glowStrip);

    // 4. 3-Spoke Volvo FH Steering Wheel (Top rim peaks ~0.12m above windBotY, matching photo!)
    const float wheelCenterY = dashTopY - 0.045f; // Top of 0.205m tilted wheel reaches windBotY + 0.12m!
    const float wheelCenterX = xFaceBack + 0.22f;
    const float wheelCenterZ = MZ(uDriver);

    AppendBoxPitchZ(
        mesh,
        Vec3(xFaceBack - 0.04f, wheelCenterY - 0.055f, std::min(MZ(uDriver - 0.06f), MZ(uDriver + 0.06f))),
        Vec3(wheelCenterX - 0.04f, wheelCenterY + 0.055f, std::max(MZ(uDriver - 0.06f), MZ(uDriver + 0.06f))),
        xFaceBack + 0.10f, wheelCenterY - 0.03f, 0.28f, leatherBlack
    );
    MBox(xFaceBack + 0.03f, effFloorY + 0.02f, uDriver - 0.05f, xFaceBack + 0.13f, wheelCenterY - 0.05f, uDriver + 0.05f, matRubberDark);

    const float wheelTilt = 0.38f;
    const float ct = std::cos(wheelTilt), st = std::sin(wheelTilt);
    const float wheelRoll = dSign * (steerAngleDeg * 3.5f * kPi / 180.0f);
    const float cr = std::cos(wheelRoll), sr = std::sin(wheelRoll);
    const float wheelR = 0.205f;
    const float flatCutY = -wheelR * 0.86f;

    auto wheelPt = [&](float ryPreRoll, float ruPreRoll, float nxOffset) -> Vec3 {
        float ry = ryPreRoll * cr - ruPreRoll * sr;
        float ru = ryPreRoll * sr + ruPreRoll * cr;
        float wx = wheelCenterX + nxOffset * ct - ry * st;
        float wy = wheelCenterY + nxOffset * st + ry * ct;
        float wz = wheelCenterZ + dSign * ru;
        return Vec3(wx, wy, wz);
    };

    const int rimSegs = 24;
    const float tubeR = 0.020f;
    for (int i = 0; i < rimSegs; ++i) {
        float a0 = (static_cast<float>(i) / static_cast<float>(rimSegs)) * 2.0f * kPi;
        float a1 = (static_cast<float>(i + 1) / static_cast<float>(rimSegs)) * 2.0f * kPi;
        float yA = std::max(flatCutY, wheelR * std::sin(a0)), uA = wheelR * std::cos(a0);
        float yB = std::max(flatCutY, wheelR * std::sin(a1)), uB = wheelR * std::cos(a1);
        float xm = (a0 + a1) * 0.5f;
        float cA = std::cos(a0), sA = std::sin(a0);
        float cB = std::cos(a1), sB = std::sin(a1);

        Vec3 r0O = wheelPt(yA + tubeR * sA, uA + tubeR * cA, 0.0f);
        Vec3 r1O = wheelPt(yB + tubeR * sB, uB + tubeR * cB, 0.0f);
        Vec3 r0I = wheelPt(yA - tubeR * sA, uA - tubeR * cA, 0.0f);
        Vec3 r1I = wheelPt(yB - tubeR * sB, uB - tubeR * cB, 0.0f);
        Vec3 r0F = wheelPt(yA, uA,  tubeR), r1F = wheelPt(yB, uB,  tubeR);
        Vec3 r0B = wheelPt(yA, uA, -tubeR), r1B = wheelPt(yB, uB, -tubeR);

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
        faceOut(c100, c101, c111, c110);
        faceOut(c001, c000, c010, c011);
        faceOut(c010, c110, c111, c011);
        faceOut(c000, c001, c101, c100);
        faceOut(c000, c100, c110, c010);
        faceOut(c001, c011, c111, c101);
    };
    // Steering wheel spokes, central airbag hub & silver top stripe
    emitWheelBlock(-0.024f, 0.024f, -wheelR + 0.015f, -0.055f, -0.014f, 0.014f, dashCharcoal);
    emitWheelBlock(-0.024f, 0.024f,  0.055f, wheelR - 0.015f, -0.014f, 0.014f, dashCharcoal);
    emitWheelBlock(flatCutY + 0.010f, -0.032f, -0.040f, 0.040f, -0.014f, 0.016f, satinSilver);
    emitWheelBlock(-0.068f, 0.068f, -0.070f, 0.070f, -0.018f, 0.024f, leatherBlack);
    emitWheelBlock(-0.026f, 0.026f, -0.026f, 0.026f,  0.022f, 0.030f, satinSilver);
    emitWheelBlock(wheelR - 0.020f, wheelR + 0.020f, -0.018f, 0.018f, -0.022f, 0.022f, satinSilver);

    // 5. Overhead Lockers, Headliner & Signature Neon-Green Ambient Ceiling Halo Ring
    for (int side : {-1, 1}) {
        ChassisComponents::AppendClosedCylinderY(
            mesh, xDashFront + 0.20f, MZ(static_cast<float>(side) * (inHW - 0.10f)),
            dashTopY + 0.08f, dashTopY + 0.48f, 0.017f, 10, dashUpperSlate
        );
    }
    const float yHeaderBot = std::max(dashTopY + 0.78f, ceilY - 0.22f);
    MBox(xDashFront + 0.06f, yHeaderBot, -inHW + 0.06f, xDashFront + 0.50f, ceilY, inHW - 0.06f, dashCharcoal);
    MBox(xDashFront + 0.05f, ceilY, -inHW, xCabRear, ceilY + 0.045f, inHW, headlinerDark);
    MBox(xCabRear - 0.055f, effFloorY, -inHW, xCabRear, ceilY, inHW, headlinerDark);

    // Signature green ceiling ambient halo LED visible through panoramic windshield
    if (ambientGlowOn) {
        const float yLed0 = (windBotY > floorY + 0.42f) ? std::min(ceilY - 0.14f, windBotY + 0.72f) : (ceilY - 0.14f);
        const float yLed1 = yLed0 + 0.042f;
        MBox(xDashFront + 0.50f, yLed0, -inHW + 0.16f, xDashFront + 0.56f, yLed1,  inHW - 0.16f, glowStrip);
        MBox(xDashFront + 0.56f, yLed0, -inHW + 0.16f, xCabRear - 0.18f,   yLed1, -inHW + 0.22f, glowStrip);
        MBox(xDashFront + 0.56f, yLed0,  inHW - 0.22f, xCabRear - 0.18f,   yLed1,  inHW - 0.16f, glowStrip);
        MBox(xCabRear - 0.24f,   yLed0, -inHW + 0.16f, xCabRear - 0.18f,   yLed1,  inHW - 0.16f, glowStrip);
    }

    // 6. Twin Quilted High-Back Volvo Bucket Seats + Circular Headrest Emblem (visible in windshield!)
    const float seatX = std::clamp(xDashFront + 1.24f, xDashFront + 1.08f, xCabRear - 0.34f);
    const float seatH = 1.12f; // Headrest top reaches windBotY + 0.34m!
    for (int side : {-1, 1}) {
        float uS = static_cast<float>(side) * uDriver;
        AppendSeat(mesh, Vec3(seatX, effFloorY, MZ(uS)), -1.0f, 0.54f, seatH, seatQuilt, leatherBlack, dashCharcoal);
        // Quilted center bolster ribs + circular chrome Volvo badge on front of headrest (-X)
        float zSeat = MZ(uS);
        for (int q = 0; q < 4; ++q) {
            float qy = effFloorY + 0.56f + static_cast<float>(q) * 0.095f;
            MBox(seatX - 0.21f, qy, uS - 0.16f, seatX - 0.15f, qy + 0.035f, uS + 0.16f, leatherBlack);
        }
        AppendVerticalDiscNegX(mesh, seatX - 0.215f, effFloorY + seatH - 0.085f, zSeat, 0.044f, 0.012f, 14, satinSilver, accentColor);
        // Fold-down I-Shift selector arm next to driver seat
        if (side > 0) {
            MBox(seatX - 0.26f, effFloorY + 0.58f, uS - 0.34f, seatX - 0.06f, effFloorY + 0.67f, uS - 0.26f, dashUpperSlate);
            MBox(seatX - 0.28f, effFloorY + 0.67f, uS - 0.33f, seatX - 0.20f, effFloorY + 0.77f, uS - 0.27f, satinSilver);
        }
    }

    // 7. Sleeper Bunk & Enclosed Rear Compartment for 360 Head Rotation
    if (hasSleeperBunk && (xCabRear - seatX) > 0.55f) {
        const float bunkX0 = seatX + 0.30f;
        MBox(bunkX0, effFloorY, -inHW, xCabRear - 0.05f, effFloorY + 0.34f, inHW, dashCharcoal);
        MBox(bunkX0 + 0.02f, effFloorY + 0.34f, -inHW + 0.04f, xCabRear - 0.06f, effFloorY + 0.48f, inHW - 0.04f, Vec4(0.84f, 0.86f, 0.90f, 1.0f));
        MBox(bunkX0 + 0.08f, effFloorY + 0.48f, uDriver - 0.25f, bunkX0 + 0.40f, effFloorY + 0.56f, uDriver + 0.25f, seatQuilt);
        float sleepWallX0 = std::max(seatX + 0.14f, xCabRear - 0.82f);
        if (xCabRear - sleepWallX0 > 0.08f) {
            MBox(sleepWallX0, effFloorY + 0.32f, -inHW - 0.025f, xCabRear - 0.04f, dashTopY + 0.08f, -inHW + 0.015f, dashCharcoal);
            MBox(sleepWallX0, effFloorY + 0.32f,  inHW - 0.015f, xCabRear - 0.04f, dashTopY + 0.08f,  inHW + 0.025f, dashCharcoal);
            MBox(xCabRear - 0.22f, dashTopY + 0.08f, -inHW - 0.025f, xCabRear - 0.04f, ceilY, -inHW + 0.015f, headlinerDark);
            MBox(xCabRear - 0.22f, dashTopY + 0.08f,  inHW - 0.015f, xCabRear - 0.04f, ceilY,  inHW + 0.025f, headlinerDark);
        }
    }

    // 8. Interior Driver Eye Anchor for 360-degree Cabin Look-Around
    mesh->hasInteriorEye = true;
    mesh->interiorEyePos = Vec3(
        xDashFront + 0.98f,
        windBotY + 0.36f,
        MZ(uDriver * 0.72f)
    );
}

} // namespace sphere::vehicles::cab
