#include "Minibus.h"
#include "CabComponents.h"
#include "ChassisComponents.h"
#include "MinibusDesigns.h"
#include "RunningGear.h"
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace sphere::vehicles {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::cab::CabComponents;
using sphere::vehicles::chassis::ChassisComponents;

namespace {
Vec4 HashMatatuSwatch(uint32_t seed, uint32_t idx, const Vec4& accent) {
    const Vec4 swatches[6] = {
        accent,
        Vec4(0.08f, 0.80f, 0.90f, 1.0f),
        Vec4(0.98f, 0.78f, 0.08f, 1.0f),
        Vec4(0.22f, 0.86f, 0.38f, 1.0f),
        Vec4(0.95f, 0.24f, 0.14f, 1.0f),
        Vec4(0.60f, 0.24f, 0.94f, 1.0f)
    };
    uint32_t h = (seed ^ (idx * 2654435761u)) * 1664525u + 1013904223u;
    return swatches[h % 6u];
}

inline void AppendTri(Mesh* mesh, const Vec3& a, const Vec3& b, const Vec3& c, const Vec4& col) {
    if (!mesh) return;
    Vec3 e1(b.x - a.x, b.y - a.y, b.z - a.z);
    Vec3 e2(c.x - a.x, c.y - a.y, c.z - a.z);
    Vec3 n(e1.y * e2.z - e1.z * e2.y, e1.z * e2.x - e1.x * e2.z, e1.x * e2.y - e1.y * e2.x);
    float lenSq = n.x * n.x + n.y * n.y + n.z * n.z;
    if (lenSq > 1e-10f) {
        float inv = 1.0f / std::sqrt(lenSq);
        n.x *= inv; n.y *= inv; n.z *= inv;
    } else {
        n = Vec3(0.0f, 1.0f, 0.0f);
    }
    uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
    Vertex v0; v0.position = a; v0.normal = n; v0.color = col; v0.uv[0] = 0.0f; v0.uv[1] = 0.0f;
    Vertex v1; v1.position = b; v1.normal = n; v1.color = col; v1.uv[0] = 1.0f; v1.uv[1] = 0.0f;
    Vertex v2; v2.position = c; v2.normal = n; v2.color = col; v2.uv[0] = 1.0f; v2.uv[1] = 1.0f;
    mesh->vertices.push_back(v0);
    mesh->vertices.push_back(v1);
    mesh->vertices.push_back(v2);
    mesh->indices.push_back(base + 0);
    mesh->indices.push_back(base + 1);
    mesh->indices.push_back(base + 2);
}

// Closed Octagon Stop Sign (facing +X / -X or pivoted outward around +Y)
void AppendOctagonStopSign(
    Mesh* mesh,
    const Vec3& hingePos,
    float radius,
    float thicknessZ,
    float swingRad,
    const Vec4& signRed,
    const Vec4& borderWhite
) {
    if (!mesh) return;
    const float cosA = std::cos(swingRad);
    const float sinA = std::sin(swingRad);
    // Swings from flush along +X outward toward -Z
    auto toWorld = [&](float localAlong, float localY, float localOut) -> Vec3 {
        float wx = hingePos.x + localAlong * cosA - localOut * sinA;
        float wz = hingePos.z - localAlong * sinA - localOut * cosA;
        return Vec3(wx, hingePos.y + localY, wz);
    };
    const Vec3 centerF = toWorld(radius + 0.04f, 0.0f, thicknessZ);
    const Vec3 centerB = toWorld(radius + 0.04f, 0.0f, -thicknessZ);

    std::vector<Vec3> rimF(8), rimB(8), inF(8), inB(8);
    for (int i = 0; i < 8; ++i) {
        float a = (2.0f * 3.14159265f * (static_cast<float>(i) + 0.5f)) / 8.0f;
        float lx = radius + 0.04f + radius * std::cos(a);
        float ly = radius * std::sin(a);
        float ilx = radius + 0.04f + (radius * 0.84f) * std::cos(a);
        float ily = (radius * 0.84f) * std::sin(a);
        rimF[i] = toWorld(lx, ly, thicknessZ);
        rimB[i] = toWorld(lx, ly, -thicknessZ);
        inF[i]  = toWorld(ilx, ily, thicknessZ + 0.004f);
        inB[i]  = toWorld(ilx, ily, -thicknessZ - 0.004f);
    }
    for (int i = 0; i < 8; ++i) {
        int in = (i + 1) % 8;
        AppendTri(mesh, centerF, rimF[in], rimF[i], borderWhite);
        AppendTri(mesh, centerB, rimB[i], rimB[in], borderWhite);
        AppendTri(mesh, centerF, inF[in], inF[i], signRed);
        AppendTri(mesh, centerB, inB[i], inB[in], signRed);
        CabComponents::AppendQuad(mesh, rimB[i], rimF[i], rimF[in], rimB[in], borderWhite);
    }
}
} // namespace

Minibus::Minibus(VehicleID id) : m_Def(CreateMinibusDefaultDef(id)) {}

std::shared_ptr<Mesh> Minibus::GenerateMesh(uint32_t lod) {
    (void)lod;
    auto mesh = std::make_shared<Mesh>();

    const minibus::MinibusStyle style = minibus::ResolveMinibusStyle(m_Def.id);
    const float L = std::clamp(m_Def.dimensions.x, 4.6f, 9.0f);
    const float W = std::clamp(m_Def.dimensions.y, 1.75f, 2.45f);
    const float H = std::clamp(m_Def.dimensions.z, 2.10f, 3.25f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(m_Def.wheel_radius_m, 0.32f, 0.45f);
    const float tireW = std::clamp(m_Def.tire_width_m, 0.19f, 0.28f);
    const bool dualRear = (style == minibus::MinibusStyle::AIRPORT_SHUTTLE);

    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = dualRear ? (hw - tireW * 1.06f - 0.025f) : frontHalfTrack;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float sillY = std::clamp(m_Def.ground_clearance_m + 0.04f, 0.22f, 0.36f);
    const float noseFloorY = sillY + 0.08f;
    const float archTopY = wheelR * 2.0f + 0.11f;
    const float archHalfLen = wheelR + 0.09f;
    const float roofBotY = H - (m_Def.has_roof_rack ? 0.16f : 0.04f);

    const Vec4 bodyPaint = m_Def.base_color;
    const Vec4 accent = m_Def.accent_color;
    const Vec4 glossBlack(0.06f, 0.07f, 0.09f, 1.0f);
    const Vec4 darkTrim(0.10f, 0.11f, 0.13f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 chromeCol(0.82f, 0.85f, 0.90f, 1.0f);
    const Vec4 seatCol(0.20f, 0.26f, 0.34f, 1.0f);
    const Vec4 floorCol(0.16f, 0.17f, 0.20f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);

    const bool isMatatu   = (style == minibus::MinibusStyle::MATATU_NGANYA);
    const bool isSprinter = (style == minibus::MinibusStyle::SPRINTER_COMMERCIAL);
    const bool isShuttle  = (style == minibus::MinibusStyle::AIRPORT_SHUTTLE);
    const bool isSchool   = (style == minibus::MinibusStyle::SCHOOL_BUS_VAN);

    // 1. Chassis Frame, Steerable Axles & Open Wheel-Well Pockets
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.20f, xRear - 0.14f, sillY + 0.02f, sillY + 0.14f, 0.36f, darkTrim);

    const float frontAxleX = xFront + 0.96f;
    const float rearAxleX = std::min(xRear - 0.92f, frontAxleX + m_Def.wheelbase_m);
    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, frontHalfTrack, false, m_Def.steer_angle_deg);
    ChassisComponents::AppendVehicleAxle(mesh.get(), rearAxleX,  wheelR, tireW, rearHalfTrack,  dualRear, 0.0f);
    auto emitMinibusWheelBay = [&](float ax, float innerZ, float fenderDepth) {
        float cutR = wheelR + 0.07f;
        const Vec4& fenderCol = isSprinter ? darkTrim : bodyPaint;
        CabComponents::AppendWheelWellLiner(mesh.get(), ax, archHalfLen, sillY, archTopY, hw, innerZ, linerDark);
        CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutR, archHalfLen, sillY + 0.06f, archTopY, -hw, false, 0.06f, bodyPaint, linerDark);
        CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutR, archHalfLen, sillY + 0.06f, archTopY,  hw, true,  0.06f, bodyPaint, linerDark);
        CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutR, -hw, false, fenderDepth, fenderCol);
        CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutR,  hw, true,  fenderDepth, fenderCol);
    };
    emitMinibusWheelBay(frontAxleX, hw - tireW - 0.12f, 0.028f);
    emitMinibusWheelBay(rearAxleX, hw - (dualRear ? tireW * 2.1f : tireW) - 0.12f, dualRear ? 0.055f : 0.028f);

    // Distinctive Shuttle Wide Dual-Rear Wheel Blisters
    if (isShuttle) {
        for (int side : {-1, 1}) {
            float sz0 = (side < 0) ? (-hw - 0.052f) : hw;
            float sz1 = (side < 0) ? (-hw) : (hw + 0.052f);
            RunningGear::AppendBox(mesh.get(), Vec3(rearAxleX - archHalfLen - 0.06f, archTopY - 0.04f, sz0), Vec3(rearAxleX + archHalfLen + 0.06f, archTopY + 0.05f, sz1), bodyPaint);
        }
    }

    // Key van nose, cab doorway, sliding passenger doorway & roof heights per variant
    float hoodEndX = isSprinter ? (xFront + 0.94f)
                   : isShuttle  ? (xFront + 0.54f)
                   : isSchool   ? (xFront + 0.52f)
                                : (xFront + 0.46f);
    float xRoofJoin = hoodEndX + 0.36f;
    float hoodTopY = std::max(archTopY + 0.06f, sillY + (roofBotY - sillY) * (isSprinter ? 0.35f : 0.38f));
    float winBot = std::max(hoodTopY + 0.04f, sillY + (roofBotY - sillY) * 0.44f);
    float winTop = roofBotY - 0.24f;
    float cabDoorX0 = xRoofJoin - 0.04f;
    float cabDoorX1 = cabDoorX0 + 0.74f;
    float slideBaseX0 = std::max(cabDoorX1 + 0.12f, frontAxleX + archHalfLen + 0.04f);
    float slideBaseX1 = std::min(rearAxleX - archHalfLen - 0.04f, slideBaseX0 + (isShuttle ? 1.16f : 1.02f));
    if (slideBaseX1 <= slideBaseX0 + 0.65f) {
        slideBaseX1 = slideBaseX0 + 0.90f;
    }
    float winEndX = xRear - 0.28f;

    // 2. Full 3D Van/Minibus Interior (Driver cockpit, multi-row seats, entry step well, luggage racks)
    CabComponents::AppendBusInterior(
        mesh.get(),
        hoodEndX + 0.06f, winEndX,
        sillY + 0.14f, winTop - 0.02f,
        hw,
        slideBaseX0, slideBaseX1,
        sillY + 0.06f,
        isMatatu ? 4 : 5,
        false, 0.0f, 0.0f,
        isMatatu ? accent : seatCol, floorCol, darkTrim
    );

    // Shuttle interior stainless multi-tier airport luggage rack visible inside front passenger bay
    if (isShuttle) {
        float lrX0 = cabDoorX1 + 0.08f;
        float lrX1 = slideBaseX0 - 0.06f;
        if (lrX1 > lrX0 + 0.25f) {
            float lzIn = hw - 0.62f;
            float lzOut = hw - 0.10f;
            RunningGear::AppendBox(mesh.get(), Vec3(lrX0, sillY + 0.16f, lzIn), Vec3(lrX0 + 0.03f, winTop - 0.25f, lzIn + 0.03f), chromeCol);
            RunningGear::AppendBox(mesh.get(), Vec3(lrX1 - 0.03f, sillY + 0.16f, lzIn), Vec3(lrX1, winTop - 0.25f, lzIn + 0.03f), chromeCol);
            RunningGear::AppendBox(mesh.get(), Vec3(lrX0, sillY + 0.64f, lzIn), Vec3(lrX1, sillY + 0.68f, lzOut), darkTrim);
            RunningGear::AppendBox(mesh.get(), Vec3(lrX0, sillY + 1.18f, lzIn), Vec3(lrX1, sillY + 1.22f, lzOut), darkTrim);
        }
    }

    // 3. Style-Specific Compound-Curved Front Nose, A-Pillars, Brow & Extruded Roof Canopy (Opaque Pass 1)
    const float rearCornerR = isSchool ? 0.11f : 0.15f;
    const float xRearFace = xRear - 0.02f;
    const float xRoofRear = xRearFace - rearCornerR;
    minibus::AppendMinibusFrontAndRoof(
        mesh.get(), style,
        xFront, hoodEndX, xRoofJoin, xRoofRear,
        noseFloorY, hoodTopY, winBot, winTop, roofBotY, hw,
        m_Def.headlights_on, m_Def.door_open_ratio,
        bodyPaint, accent, glossBlack, darkTrim, chromeCol
    );

    // Front Fender Cheek closing [xFront + 0.20f .. cabDoorX0] below cowl line (sloped A-pillar cheek handles hoodTopY..winTop)
    CabComponents::AppendSkirtWithCutouts(mesh.get(), xFront + 0.20f, cabDoorX0, archTopY - 0.01f, hoodTopY + 0.02f, -hw, false, 0.06f, {}, bodyPaint);
    CabComponents::AppendSkirtWithCutouts(mesh.get(), xFront + 0.20f, cabDoorX0, archTopY - 0.01f, hoodTopY + 0.02f,  hw, true,  0.06f, {}, bodyPaint);

    // 4. Lower Side Walls & Skirts (real open wheel-well and sliding doorway cutouts)
    std::vector<std::pair<float, float>> leftSkipSkirt = {
        {frontAxleX - archHalfLen, frontAxleX + archHalfLen},
        {rearAxleX - archHalfLen, rearAxleX + archHalfLen},
    };
    std::vector<std::pair<float, float>> rightSkipSkirt = {
        {frontAxleX - archHalfLen, frontAxleX + archHalfLen},
        {slideBaseX0, slideBaseX1},
        {rearAxleX - archHalfLen, rearAxleX + archHalfLen},
    };

    CabComponents::AppendSkirtWithCutouts(mesh.get(), xFront + 0.22f, winEndX, sillY + 0.06f, archTopY, -hw, false, 0.06f, leftSkipSkirt, bodyPaint);
    CabComponents::AppendSkirtWithCutouts(mesh.get(), xFront + 0.22f, winEndX, sillY + 0.06f, archTopY,  hw, true,  0.06f, rightSkipSkirt, bodyPaint);

    // Belt wall above wheel arches up to window sill [archTopY .. winBot]
    CabComponents::AppendSkirtWithCutouts(mesh.get(), cabDoorX1, winEndX, archTopY - 0.01f, winBot, -hw, false, 0.06f, {}, bodyPaint);
    CabComponents::AppendSkirtWithCutouts(mesh.get(), cabDoorX1, winEndX, archTopY - 0.01f, winBot,  hw, true,  0.06f, {{slideBaseX0, slideBaseX1}}, bodyPaint);

    // 5. Unique Side Trim, Graffiti, Protective Rub-Rails & School Van STOP Swing Arm
    if (isMatatu || m_Def.livery == LiveryID::NAIROBI_NGANYA) {
        float gx0 = slideBaseX1 + 0.06f;
        float gxSpan = (xRear - 0.25f) - gx0;
        if (gxSpan > 0.6f) {
            int blocks = 4;
            for (int b = 0; b < blocks; ++b) {
                float bx0 = gx0 + (gxSpan * static_cast<float>(b)) / static_cast<float>(blocks);
                float bx1 = gx0 + (gxSpan * static_cast<float>(b + 1)) / static_cast<float>(blocks);
                if (bx1 > rearAxleX - archHalfLen && bx0 < rearAxleX + archHalfLen) continue;
                Vec4 colL = HashMatatuSwatch(m_Def.seed, static_cast<uint32_t>(b), accent);
                Vec4 colR = HashMatatuSwatch(m_Def.seed + 17u, static_cast<uint32_t>(b), accent);
                RunningGear::AppendBox(mesh.get(), Vec3(bx0 + 0.02f, archTopY + 0.02f, -hw - 0.010f), Vec3(bx1 - 0.02f, winBot - 0.02f, -hw + 0.01f), colL);
                RunningGear::AppendBox(mesh.get(), Vec3(bx0 + 0.02f, archTopY + 0.02f,  hw - 0.01f), Vec3(bx1 - 0.02f, winBot - 0.02f,  hw + 0.010f), colR);
            }
        }
        // Neon glowing underglow rocker skirt strips
        const Vec4 neonCyan(0.14f, 0.90f, 1.0f, 1.0f);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), frontAxleX + archHalfLen + 0.04f, rearAxleX - archHalfLen - 0.04f, sillY + 0.07f, sillY + 0.11f, -hw - 0.010f, false, 0.02f, {}, accent);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), frontAxleX + archHalfLen + 0.04f, rearAxleX - archHalfLen - 0.04f, sillY + 0.07f, sillY + 0.11f,  hw + 0.010f, true,  0.02f, {{slideBaseX0, slideBaseX1}}, neonCyan);
        RunningGear::AppendBox(mesh.get(), Vec3(xRoofJoin, winTop - 0.04f, -hw - 0.010f), Vec3(xRear - 0.22f, winTop + 0.04f, -hw + 0.01f), accent);
        RunningGear::AppendBox(mesh.get(), Vec3(xRoofJoin, winTop - 0.04f,  hw - 0.01f), Vec3(xRear - 0.22f, winTop + 0.04f,  hw + 0.010f), accent);
    } else if (isSprinter) {
        // Molded dark-charcoal European lower rocker cladding + cobalt corporate stripe
        CabComponents::AppendSkirtWithCutouts(mesh.get(), xFront + 0.24f, winEndX, sillY + 0.06f, sillY + 0.26f, -hw - 0.010f, false, 0.02f, leftSkipSkirt, darkTrim);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), xFront + 0.24f, winEndX, sillY + 0.06f, sillY + 0.26f,  hw + 0.010f, true,  0.02f, rightSkipSkirt, darkTrim);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), cabDoorX1 + 0.04f, winEndX, winBot - 0.16f, winBot - 0.04f, -hw - 0.009f, false, 0.02f, {}, accent);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), cabDoorX1 + 0.04f, winEndX, winBot - 0.16f, winBot - 0.04f,  hw + 0.009f, true,  0.02f, {{slideBaseX0, slideBaseX1}}, accent);
        // Sprinter rear rooftop sleek HVAC pod
        RunningGear::AppendBox(mesh.get(), Vec3(rearAxleX - 0.90f, roofBotY, -hw * 0.42f), Vec3(rearAxleX + 0.30f, roofBotY + 0.15f, hw * 0.42f), bodyPaint);
    } else if (isShuttle) {
        // Gold airport transit stripe + curbside stainless boarding grab rail + commercial roof HVAC unit
        CabComponents::AppendSkirtWithCutouts(mesh.get(), cabDoorX1 + 0.04f, winEndX, winBot - 0.18f, winBot - 0.04f, -hw - 0.010f, false, 0.02f, {}, accent);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), cabDoorX1 + 0.04f, winEndX, winBot - 0.18f, winBot - 0.04f,  hw + 0.010f, true,  0.02f, {{slideBaseX0, slideBaseX1}}, accent);
        // Curbside stainless boarding grab rail next to shuttle entry doorway
        float railX = slideBaseX0 - 0.04f;
        RunningGear::AppendBox(mesh.get(), Vec3(railX - 0.02f, sillY + 0.42f, hw + 0.025f), Vec3(railX + 0.02f, winBot + 0.16f, hw + 0.055f), chromeCol);
        RunningGear::AppendBox(mesh.get(), Vec3(railX - 0.02f, sillY + 0.42f, hw), Vec3(railX + 0.02f, sillY + 0.45f, hw + 0.055f), chromeCol);
        RunningGear::AppendBox(mesh.get(), Vec3(railX - 0.02f, winBot + 0.13f, hw), Vec3(railX + 0.02f, winBot + 0.16f, hw + 0.055f), chromeCol);
        // Large roof commercial HVAC unit
        float acX0 = frontAxleX + 0.85f;
        float acX1 = rearAxleX - 0.35f;
        RunningGear::AppendBox(mesh.get(), Vec3(acX0, roofBotY, -hw * 0.50f), Vec3(acX1, roofBotY + 0.20f, hw * 0.50f), Vec4(0.92f, 0.94f, 0.96f, 1.0f));
        RunningGear::AppendBox(mesh.get(), Vec3(acX0 + 0.12f, roofBotY + 0.03f, -hw * 0.51f), Vec3(acX1 - 0.12f, roofBotY + 0.14f, hw * 0.51f), darkTrim);
    } else if (isSchool) {
        // Twin full-length structural black steel rub-rails along both flanks
        for (float ry : {sillY + 0.20f, winBot - 0.12f}) {
            CabComponents::AppendSkirtWithCutouts(mesh.get(), cabDoorX1 + 0.02f, winEndX, ry, ry + 0.06f, -hw - 0.015f, false, 0.022f, {}, darkTrim);
            CabComponents::AppendSkirtWithCutouts(mesh.get(), cabDoorX1 + 0.02f, winEndX, ry, ry + 0.06f,  hw + 0.015f, true,  0.022f, {{slideBaseX0, slideBaseX1}}, darkTrim);
        }
        // Dynamic Red Octagonal School STOP Swing Arm under driver window (rotates outward with door_open_ratio)
        float stopSwing = std::clamp(m_Def.door_open_ratio, 0.0f, 1.0f) * (3.14159265f * 0.48f);
        AppendOctagonStopSign(
            mesh.get(),
            Vec3(cabDoorX0 + 0.22f, winBot - 0.10f, -hw - 0.018f),
            0.16f, 0.012f, stopSwing,
            Vec4(0.90f, 0.10f, 0.12f, 1.0f), Vec4(0.96f, 0.96f, 0.98f, 1.0f)
        );
    }

    // 6. Side Window Mullions & Openable Cab + Curb Sliding Doors (Opaque Pass 1)
    float sideWinStartX = cabDoorX1 + 0.04f;
    int panes = isShuttle ? 5 : 4;
    for (int p = 1; p < panes; ++p) {
        float px = sideWinStartX + (winEndX - sideWinStartX) * (static_cast<float>(p) / static_cast<float>(panes));
        RunningGear::AppendBox(mesh.get(), Vec3(px - 0.016f, winBot, -hw - 0.014f), Vec3(px + 0.016f, winTop, -hw + 0.01f), glossBlack);
        if (px < slideBaseX0 - 0.03f || px > slideBaseX1 + 0.03f) {
            RunningGear::AppendBox(mesh.get(), Vec3(px - 0.016f, winBot, hw - 0.01f), Vec3(px + 0.016f, winTop, hw + 0.014f), glossBlack);
        }
    }

    float cabDoorBotY = std::max(sillY + 0.12f, archTopY + 0.02f);
    CabComponents::AppendAnimatedDoor(mesh.get(), cabDoorX0, cabDoorX1, cabDoorBotY, winTop, -hw, false, m_Def.door_open_ratio, false, false, bodyPaint, glossBlack, sideGlass);
    CabComponents::AppendAnimatedDoor(mesh.get(), cabDoorX0, cabDoorX1, cabDoorBotY, winTop,  hw, true,  m_Def.door_open_ratio, false, false, bodyPaint, glossBlack, sideGlass);

    CabComponents::AppendAnimatedDoor(
        mesh.get(), slideBaseX0, slideBaseX1, sillY + 0.10f, winTop + 0.02f,
        hw, true, m_Def.door_open_ratio, true, false,
        isMatatu ? accent : bodyPaint, glossBlack, sideGlass
    );

    // 7. Safari Roof Rack (Matatu), Model-Specific Mirrors & Model-Specific Rear Design
    if (isMatatu || m_Def.has_roof_rack) {
        float rx0 = xRoofJoin + 0.30f;
        float rx1 = xRear - 0.25f;
        float rz = hw - 0.16f;
        RunningGear::AppendBox(mesh.get(), Vec3(rx0, roofBotY + 0.04f, -rz), Vec3(rx1, roofBotY + 0.17f, -rz + 0.035f), darkTrim);
        RunningGear::AppendBox(mesh.get(), Vec3(rx0, roofBotY + 0.04f,  rz - 0.035f), Vec3(rx1, roofBotY + 0.17f,  rz), darkTrim);
        RunningGear::AppendBox(mesh.get(), Vec3(rx0, roofBotY + 0.04f, -rz), Vec3(rx0 + 0.04f, roofBotY + 0.17f, rz), darkTrim);
        RunningGear::AppendBox(mesh.get(), Vec3(rx1 - 0.04f, roofBotY + 0.04f, -rz), Vec3(rx1, roofBotY + 0.17f, rz), darkTrim);
        for (int c = 0; c < 6; ++c) {
            float cx = rx0 + (rx1 - rx0) * (static_cast<float>(c) / 5.0f);
            RunningGear::AppendBox(mesh.get(), Vec3(cx - 0.025f, roofBotY + 0.05f, -rz), Vec3(cx + 0.025f, roofBotY + 0.08f, rz), chromeCol);
        }
    }

    minibus::AppendMinibusMirrors(mesh.get(), style, hoodEndX, xRoofJoin, hoodTopY, winTop, hw, bodyPaint, darkTrim, chromeCol);
    minibus::AppendMinibusRearDesign(
        mesh.get(), style, winEndX, xRearFace, noseFloorY, winBot, winTop, roofBotY, hw,
        m_Def.headlights_on, bodyPaint, accent, glossBlack, darkTrim, chromeCol
    );

    // 8. Translucent Glass Pass (DRAWN LAST)
    minibus::AppendMinibusCurvedWindshield(
        mesh.get(), style,
        xFront, hoodEndX, xRoofJoin,
        noseFloorY, hoodTopY, winTop, roofBotY, hw,
        windshieldGlass
    );
    minibus::AppendMinibusRearGlass(
        mesh.get(), style,
        xRearFace, winBot, winTop, hw,
        sideGlass
    );
    RunningGear::AppendBox(mesh.get(), Vec3(sideWinStartX, winBot, -hw - 0.012f), Vec3(winEndX, winTop, -hw + 0.005f), sideGlass);
    if (slideBaseX0 > sideWinStartX + 0.04f) {
        RunningGear::AppendBox(mesh.get(), Vec3(sideWinStartX, winBot, hw - 0.005f), Vec3(slideBaseX0, winTop, hw + 0.012f), sideGlass);
    }
    if (winEndX > slideBaseX1 + 0.04f) {
        RunningGear::AppendBox(mesh.get(), Vec3(slideBaseX1, winBot, hw - 0.005f), Vec3(winEndX, winTop, hw + 0.012f), sideGlass);
    }
    CabComponents::AppendAnimatedDoor(mesh.get(), cabDoorX0, cabDoorX1, cabDoorBotY, winTop, -hw, false, m_Def.door_open_ratio, false, true, bodyPaint, glossBlack, sideGlass);
    CabComponents::AppendAnimatedDoor(mesh.get(), cabDoorX0, cabDoorX1, cabDoorBotY, winTop,  hw, true,  m_Def.door_open_ratio, false, true, bodyPaint, glossBlack, sideGlass);
    CabComponents::AppendAnimatedDoor(mesh.get(), slideBaseX0, slideBaseX1, sillY + 0.10f, winTop + 0.02f, hw, true, m_Def.door_open_ratio, true, true, isMatatu ? accent : bodyPaint, glossBlack, sideGlass);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

bool Minibus::GenerateTextures(VkDevice, VkQueue, VkCommandPool) {
    m_Generated = true;
    return true;
}

} // namespace sphere::vehicles
