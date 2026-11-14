#include "Minibus.h"
#include "CabComponents.h"
#include "ChassisComponents.h"
#include "RunningGear.h"
#include <algorithm>

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
} // namespace

Minibus::Minibus(VehicleID id) : m_Def(CreateMinibusDefaultDef(id)) {}

std::shared_ptr<Mesh> Minibus::GenerateMesh(uint32_t lod) {
    (void)lod;
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(m_Def.dimensions.x, 4.6f, 9.0f);
    const float W = std::clamp(m_Def.dimensions.y, 1.75f, 2.45f);
    const float H = std::clamp(m_Def.dimensions.z, 2.10f, 3.25f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(m_Def.wheel_radius_m, 0.32f, 0.45f);
    const float tireW = std::clamp(m_Def.tire_width_m, 0.19f, 0.28f);
    const float halfTrack = hw - tireW * 0.68f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float sillY = std::clamp(m_Def.ground_clearance_m + 0.04f, 0.22f, 0.36f);
    const float roofBotY = H - (m_Def.has_roof_rack ? 0.16f : 0.04f);

    const Vec4 bodyPaint = m_Def.base_color;
    const Vec4 accent = m_Def.accent_color;
    const Vec4 darkTrim(0.10f, 0.11f, 0.13f, 1.0f);
    const Vec4 chromeCol(0.78f, 0.81f, 0.86f, 1.0f);
    const Vec4 windowGlass(0.10f, 0.16f, 0.25f, 1.0f);

    bool isMatatu = (m_Def.id == VehicleID::MATATU_14_SEATER);
    bool isSprinter = (m_Def.id == VehicleID::SPRINTER_VAN);
    bool isSchool = (m_Def.id == VehicleID::SCHOOL_VAN);

    // 1. Frame & Van/Minibus Unibody Hull
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.18f, xRear - 0.14f, sillY + 0.02f, sillY + 0.14f, 0.36f, darkTrim);

    float hoodEndX = isSprinter ? (xFront + 1.02f) : (xFront + 0.42f);
    float hoodTopY = isSprinter ? (sillY + 0.78f) : (sillY + 0.68f);

    // Lower front nose / hood + main tall body shell
    CabComponents::AppendFrustumX(
        mesh.get(),
        xFront + 0.02f, sillY + 0.06f, hoodTopY - 0.08f, hw - 0.04f,
        hoodEndX,       sillY + 0.04f, hoodTopY + 0.06f, hw,
        bodyPaint
    );
    CabComponents::AppendFrustumX(
        mesh.get(),
        hoodEndX,       hoodTopY, roofBotY - 0.14f, hw - 0.03f,
        hoodEndX + 0.55f, hoodTopY, roofBotY,         hw,
        bodyPaint
    );
    RunningGear::AppendBox(mesh.get(), Vec3(hoodEndX, sillY + 0.04f, -hw), Vec3(xRear - 0.06f, roofBotY, hw), bodyPaint);

    // 2. Matatu Algorithmic Graffiti Panels OR Clean Transit Stripe
    if (isMatatu || m_Def.livery == LiveryID::NAIROBI_NGANYA) {
        // Multi-panel Nganya graffiti blocks along lower side panels seeded by m_Def.seed
        float gx0 = hoodEndX + 0.25f;
        float gxSpan = (xRear - 0.25f) - gx0;
        int blocks = 6;
        for (int b = 0; b < blocks; ++b) {
            float bx0 = gx0 + (gxSpan * static_cast<float>(b)) / static_cast<float>(blocks);
            float bx1 = gx0 + (gxSpan * static_cast<float>(b + 1)) / static_cast<float>(blocks);
            Vec4 colL = HashMatatuSwatch(m_Def.seed, static_cast<uint32_t>(b), accent);
            Vec4 colR = HashMatatuSwatch(m_Def.seed + 17u, static_cast<uint32_t>(b), accent);
            float hLo = sillY + 0.14f + (b % 2) * 0.08f;
            float hHi = sillY + 0.68f + ((b + 1) % 2) * 0.08f;
            RunningGear::AppendBox(mesh.get(), Vec3(bx0 + 0.02f, hLo, -hw - 0.012f), Vec3(bx1 - 0.02f, hHi, -hw + 0.01f), colL);
            RunningGear::AppendBox(mesh.get(), Vec3(bx0 + 0.02f, hLo,  hw - 0.01f), Vec3(bx1 - 0.02f, hHi,  hw + 0.012f), colR);
        }
        // Bright neon rim strip above windows
        RunningGear::AppendBox(mesh.get(), Vec3(hoodEndX + 0.35f, roofBotY - 0.14f, -hw - 0.012f), Vec3(xRear - 0.12f, roofBotY - 0.04f, -hw + 0.01f), accent);
        RunningGear::AppendBox(mesh.get(), Vec3(hoodEndX + 0.35f, roofBotY - 0.14f,  hw - 0.01f), Vec3(xRear - 0.12f, roofBotY - 0.04f,  hw + 0.012f), accent);
    } else {
        RunningGear::AppendBox(mesh.get(), Vec3(hoodEndX + 0.2f, sillY + 0.44f, -hw - 0.01f), Vec3(xRear - 0.1f, sillY + 0.62f, -hw + 0.01f), accent);
        RunningGear::AppendBox(mesh.get(), Vec3(hoodEndX + 0.2f, sillY + 0.44f,  hw - 0.01f), Vec3(xRear - 0.1f, sillY + 0.62f,  hw + 0.01f), accent);
    }

    // 3. School Van Stop-Arm Sign & Flashing Warning Bars
    if (isSchool) {
        RunningGear::AppendBox(mesh.get(), Vec3(hoodEndX + 0.22f, roofBotY - 0.22f, -hw - 0.04f), Vec3(hoodEndX + 0.56f, roofBotY - 0.04f, -hw), Vec4(0.90f, 0.12f, 0.12f, 1.0f));
        CabComponents::AppendAmberRoofMarkers(mesh.get(), hoodEndX + 0.35f, roofBotY, hw, m_Def.headlights_on);
    }

    // 4. Side Windows + Front Cab Doors + Sliding Curb Passenger Door
    float winBot = sillY + 0.80f;
    float winTop = roofBotY - 0.18f;
    RunningGear::AppendBox(mesh.get(), Vec3(hoodEndX + 0.72f, winBot, -hw - 0.01f), Vec3(xRear - 0.22f, winTop, -hw + 0.02f), windowGlass);
    RunningGear::AppendBox(mesh.get(), Vec3(hoodEndX + 0.72f, winBot,  hw - 0.02f), Vec3(xRear - 0.22f, winTop,  hw + 0.01f), windowGlass);
    for (int p = 0; p <= 4; ++p) {
        float px = hoodEndX + 0.72f + ((xRear - 0.22f) - (hoodEndX + 0.72f)) * (static_cast<float>(p) / 4.0f);
        RunningGear::AppendBox(mesh.get(), Vec3(px - 0.03f, winBot, -hw - 0.015f), Vec3(px + 0.03f, winTop, -hw + 0.01f), darkTrim);
        RunningGear::AppendBox(mesh.get(), Vec3(px - 0.03f, winBot,  hw - 0.01f), Vec3(px + 0.03f, winTop,  hw + 0.015f), darkTrim);
    }

    // Driver & Passenger front doors
    CabComponents::AppendDoor(mesh.get(), hoodEndX + 0.04f, hoodEndX + 0.88f, sillY + 0.10f, winTop, -hw, false, bodyPaint, windowGlass);
    CabComponents::AppendDoor(mesh.get(), hoodEndX + 0.04f, hoodEndX + 0.88f, sillY + 0.10f, winTop,  hw, true,  bodyPaint, windowGlass);

    // Curb-side (+Z) sliding van passenger door (slides rearward along +X with action_ratio)
    float slideShift = std::clamp(m_Def.action_ratio, 0.0f, 1.0f) * 0.78f;
    float sDoorX0 = hoodEndX + 1.05f + slideShift;
    float sDoorX1 = sDoorX0 + 1.02f;
    RunningGear::AppendBox(mesh.get(), Vec3(sDoorX0, sillY + 0.10f, hw - 0.01f), Vec3(sDoorX1, winTop + 0.02f, hw + 0.032f), isMatatu ? accent : bodyPaint);
    RunningGear::AppendBox(mesh.get(), Vec3(sDoorX0 + 0.08f, winBot + 0.02f, hw + 0.025f), Vec3(sDoorX1 - 0.08f, winTop - 0.04f, hw + 0.040f), windowGlass);

    // 5. Roof Luggage Rack (Matatu / Expedition option)
    if (isMatatu || m_Def.has_roof_rack) {
        float rx0 = hoodEndX + 0.65f;
        float rx1 = xRear - 0.25f;
        float rz = hw - 0.14f;
        RunningGear::AppendBox(mesh.get(), Vec3(rx0, roofBotY, -rz), Vec3(rx1, roofBotY + 0.14f, -rz + 0.035f), darkTrim);
        RunningGear::AppendBox(mesh.get(), Vec3(rx0, roofBotY,  rz - 0.035f), Vec3(rx1, roofBotY + 0.14f,  rz), darkTrim);
        RunningGear::AppendBox(mesh.get(), Vec3(rx0, roofBotY, -rz), Vec3(rx0 + 0.04f, roofBotY + 0.14f, rz), darkTrim);
        RunningGear::AppendBox(mesh.get(), Vec3(rx1 - 0.04f, roofBotY, -rz), Vec3(rx1, roofBotY + 0.14f, rz), darkTrim);
        for (int c = 0; c < 6; ++c) {
            float cx = rx0 + (rx1 - rx0) * (static_cast<float>(c) / 5.0f);
            RunningGear::AppendBox(mesh.get(), Vec3(cx - 0.025f, roofBotY + 0.01f, -rz), Vec3(cx + 0.025f, roofBotY + 0.04f, rz), chromeCol);
        }
    }

    // 6. Windshield, Heavy Grille, Tubular Matatu Bull-Bar, Bumper & Mirrors
    CabComponents::AppendFrontBumperAndLights(mesh.get(), xFront, sillY - 0.04f, sillY + 0.24f, hw - 0.03f, m_Def.headlights_on, darkTrim);
    if (isMatatu) {
        // Steel front bull-bar + chrome twin foglamps
        RunningGear::AppendBox(mesh.get(), Vec3(xFront - 0.14f, sillY + 0.06f, -hw * 0.62f), Vec3(xFront - 0.09f, sillY + 0.44f, -hw * 0.56f), chromeCol);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront - 0.14f, sillY + 0.06f,  hw * 0.56f), Vec3(xFront - 0.09f, sillY + 0.44f,  hw * 0.62f), chromeCol);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront - 0.15f, sillY + 0.38f, -hw * 0.62f), Vec3(xFront - 0.08f, sillY + 0.44f,  hw * 0.62f), chromeCol);
    }
    CabComponents::AppendHeavyGrille(mesh.get(), xFront + 0.02f, sillY + 0.28f, hoodTopY - 0.12f, hw * 0.62f, 3, chromeCol, darkTrim);
    CabComponents::AppendWindshield(mesh.get(), hoodEndX + 0.04f, hoodEndX + 0.46f, hoodTopY, winTop, hw - 0.07f, windowGlass, darkTrim);
    CabComponents::AppendWingMirrors(mesh.get(), hoodEndX + 0.12f, sillY + 1.02f, hw, darkTrim);

    // Rear bumper & dual taillights
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.10f, sillY - 0.02f, -hw + 0.04f), Vec3(xRear + 0.04f, sillY + 0.20f, hw - 0.04f), darkTrim);
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.02f, sillY + 0.30f, -hw + 0.05f), Vec3(xRear + 0.02f, sillY + 0.72f, -hw + 0.19f), Vec4(0.86f, 0.10f, 0.10f, 1.0f));
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.02f, sillY + 0.30f,  hw - 0.19f), Vec3(xRear + 0.02f, sillY + 0.72f,  hw - 0.05f), Vec4(0.86f, 0.10f, 0.10f, 1.0f));

    // 7. Solid Front + Rear Axles (tires resting on Y = 0.0f)
    const float frontAxleX = xFront + 0.96f;
    const float rearAxleX = std::min(xRear - 0.92f, frontAxleX + m_Def.wheelbase_m);
    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, halfTrack, false, m_Def.steer_angle_deg);
    ChassisComponents::AppendVehicleAxle(mesh.get(), rearAxleX, wheelR, tireW, halfTrack, m_Def.id == VehicleID::SHUTTLE_BUS, 0.0f);

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
