#include "CoachBus.h"
#include "CabComponents.h"
#include "ChassisComponents.h"
#include "RunningGear.h"
#include <algorithm>

namespace sphere::vehicles {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::cab::CabComponents;
using sphere::vehicles::chassis::ChassisComponents;

CoachBus::CoachBus(VehicleID id) : m_Def(CreateCoachBusDefaultDef(id)) {}

std::shared_ptr<Mesh> CoachBus::GenerateMesh(uint32_t lod) {
    (void)lod;
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(m_Def.dimensions.x, 10.5f, 15.0f);
    const float W = std::clamp(m_Def.dimensions.y, 2.40f, 2.60f);
    const float H = std::clamp(m_Def.dimensions.z, 3.25f, 4.10f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(m_Def.wheel_radius_m, 0.45f, 0.56f);
    const float tireW = std::clamp(m_Def.tire_width_m, 0.27f, 0.35f);
    const float halfTrack = hw - tireW * 0.68f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float skirtBotY = std::clamp(m_Def.ground_clearance_m, 0.22f, 0.40f);
    const float deckFloorY = skirtBotY + 1.08f; // High-decker luggage bay underneath

    const Vec4 bodyPaint = m_Def.base_color;
    const Vec4 accent = m_Def.accent_color;
    const Vec4 darkTrim(0.09f, 0.10f, 0.12f, 1.0f);
    const Vec4 tintedGlass(0.07f, 0.11f, 0.18f, 1.0f); // Deep bonded opaque touring glass
    const Vec4 chromeCol(0.80f, 0.83f, 0.88f, 1.0f);
    const Vec4 bayDoorCol(
        std::min(1.0f, bodyPaint.x * 0.88f + 0.04f),
        std::min(1.0f, bodyPaint.y * 0.88f + 0.04f),
        std::min(1.0f, bodyPaint.z * 0.88f + 0.04f),
        1.0f
    );

    bool isSleeper = (m_Def.id == VehicleID::COACH_SLEEPER);
    bool isTouring = (m_Def.id == VehicleID::COACH_TOURING);
    bool isLuxury  = (m_Def.id == VehicleID::COACH_LUXURY);

    // 1. Structural frame & aerodynamic sloped coach shell
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.25f, xRear - 0.18f, skirtBotY + 0.06f, skirtBotY + 0.24f, 0.45f, darkTrim);

    // Aerodynamic sloped front nose + main touring body
    CabComponents::AppendFrustumX(
        mesh.get(),
        xFront + 0.02f, skirtBotY + 0.06f, H - 0.34f, hw - 0.04f,
        xFront + 0.72f, skirtBotY,         H - 0.06f, hw,
        bodyPaint
    );
    RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.72f, skirtBotY, -hw), Vec3(xRear - 0.12f, H - 0.06f, hw), bodyPaint);
    // Slightly tapered aerodynamic rear tail
    CabComponents::AppendFrustumX(
        mesh.get(),
        xRear - 0.12f, skirtBotY,         H - 0.06f, hw,
        xRear,         skirtBotY + 0.14f, H - 0.16f, hw - 0.03f,
        bodyPaint
    );

    // Accent livery swoosh beltline along coach sides
    RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.25f, deckFloorY - 0.08f, -hw - 0.01f), Vec3(xRear - 0.18f, deckFloorY + 0.10f, -hw + 0.01f), accent);
    RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.25f, deckFloorY - 0.08f,  hw - 0.01f), Vec3(xRear - 0.18f, deckFloorY + 0.10f,  hw + 0.01f), accent);

    // 2. Axles
    const float frontAxleX = xFront + 2.48f;
    const float driveAxleX = std::min(xRear - 2.35f, frontAxleX + m_Def.wheelbase_m);
    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, halfTrack, false, m_Def.steer_angle_deg);
    ChassisComponents::AppendVehicleAxle(mesh.get(), driveAxleX, wheelR, tireW, halfTrack, true, 0.0f);
    if (m_Def.drive_config == VehicleDriveConfig::DRIVE_6X2_TAG) {
        ChassisComponents::AppendVehicleAxle(mesh.get(), driveAxleX + 1.38f, wheelR, tireW, halfTrack, false, -m_Def.steer_angle_deg * 0.35f);
    }

    // 3. Underfloor Luggage Compartment Bay Doors (tilt open upward when action_ratio > 0)
    float bayStartX = frontAxleX + wheelR + 0.25f;
    float bayEndX = driveAxleX - wheelR - 0.28f;
    float bayLift = std::clamp(m_Def.action_ratio, 0.0f, 1.0f) * 0.45f;
    int bayCount = 3;
    float baySpan = bayEndX - bayStartX;
    for (int b = 0; b < bayCount; ++b) {
        float bx0 = bayStartX + (baySpan * static_cast<float>(b)) / static_cast<float>(bayCount) + 0.04f;
        float bx1 = bayStartX + (baySpan * static_cast<float>(b + 1)) / static_cast<float>(bayCount) - 0.04f;
        float zOut = hw + 0.025f + bayLift * 0.28f;
        float yTop = deckFloorY - 0.12f;
        float yBot = skirtBotY + 0.10f + bayLift * 0.48f;
        RunningGear::AppendBox(mesh.get(), Vec3(bx0, yBot, -zOut), Vec3(bx1, yTop, -hw + 0.01f), bayDoorCol);
        RunningGear::AppendBox(mesh.get(), Vec3(bx0, yBot,  hw - 0.01f), Vec3(bx1, yTop,  zOut), bayDoorCol);
        // Chrome luggage latches
        float midB = (bx0 + bx1) * 0.5f;
        RunningGear::AppendBox(mesh.get(), Vec3(midB - 0.07f, yBot + 0.09f, -zOut - 0.015f), Vec3(midB + 0.07f, yBot + 0.15f, -zOut), chromeCol);
        RunningGear::AppendBox(mesh.get(), Vec3(midB - 0.07f, yBot + 0.09f,  zOut), Vec3(midB + 0.07f, yBot + 0.15f,  zOut + 0.015f), chromeCol);
    }

    // 4. Deep-Tinted Bonded Panoramic Passenger Windows
    float winStartX = xFront + 1.05f;
    float winEndX = xRear - 0.52f;
    if (isSleeper) {
        // Two-tier sleeper berth porthole window bands
        RunningGear::AppendBox(mesh.get(), Vec3(winStartX, deckFloorY + 0.26f, -hw - 0.01f), Vec3(winEndX, deckFloorY + 0.88f, -hw + 0.02f), tintedGlass);
        RunningGear::AppendBox(mesh.get(), Vec3(winStartX, deckFloorY + 0.26f,  hw - 0.02f), Vec3(winEndX, deckFloorY + 0.88f,  hw + 0.01f), tintedGlass);
        RunningGear::AppendBox(mesh.get(), Vec3(winStartX, deckFloorY + 1.12f, -hw - 0.01f), Vec3(winEndX, H - 0.26f, -hw + 0.02f), tintedGlass);
        RunningGear::AppendBox(mesh.get(), Vec3(winStartX, deckFloorY + 1.12f,  hw - 0.02f), Vec3(winEndX, H - 0.26f,  hw + 0.01f), tintedGlass);
    } else {
        RunningGear::AppendBox(mesh.get(), Vec3(winStartX, deckFloorY + 0.22f, -hw - 0.01f), Vec3(winEndX, H - 0.24f, -hw + 0.02f), tintedGlass);
        RunningGear::AppendBox(mesh.get(), Vec3(winStartX, deckFloorY + 0.22f,  hw - 0.02f), Vec3(winEndX, H - 0.24f,  hw + 0.01f), tintedGlass);
    }
    // Bonded vertical mullions
    int winBays = 7;
    float winSpan = winEndX - winStartX;
    for (int i = 0; i <= winBays; ++i) {
        float px = winStartX + (winSpan * static_cast<float>(i)) / static_cast<float>(winBays);
        RunningGear::AppendBox(mesh.get(), Vec3(px - 0.025f, deckFloorY + 0.22f, -hw - 0.014f), Vec3(px + 0.025f, H - 0.22f, -hw + 0.01f), darkTrim);
        RunningGear::AppendBox(mesh.get(), Vec3(px - 0.025f, deckFloorY + 0.22f,  hw - 0.01f), Vec3(px + 0.025f, H - 0.22f,  hw + 0.014f), darkTrim);
    }

    // Touring panoramic roof skylight panels
    if (isTouring || isLuxury) {
        RunningGear::AppendBox(mesh.get(), Vec3(winStartX + 0.6f, H - 0.08f, -hw + 0.24f), Vec3(winEndX - 1.8f, H - 0.03f, hw - 0.24f), tintedGlass);
    }

    // Rooftop climate A/C units
    RunningGear::AppendBox(mesh.get(), Vec3(0.4f, H - 0.08f, -0.82f), Vec3(3.1f, H + 0.14f, 0.82f), isLuxury ? darkTrim : chromeCol);

    // 5. Front Sloped Panoramic Windshield, Chrome Grille, LED Fascia & Long Touring Mirrors
    CabComponents::AppendFrontBumperAndLights(mesh.get(), xFront, skirtBotY, skirtBotY + 0.40f, hw - 0.04f, m_Def.headlights_on, bodyPaint);
    CabComponents::AppendHeavyGrille(mesh.get(), xFront + 0.05f, skirtBotY + 0.44f, skirtBotY + 0.86f, hw * 0.56f, 3, chromeCol, darkTrim);
    CabComponents::AppendWindshield(mesh.get(), xFront + 0.10f, xFront + 0.62f, skirtBotY + 0.94f, H - 0.34f, hw - 0.09f, tintedGlass, darkTrim);
    CabComponents::AppendWingMirrors(mesh.get(), xFront + 0.14f, deckFloorY + 0.22f, hw, bodyPaint);

    // Rear engine bay deck + chrome coach trim + LED light blades
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.02f, skirtBotY + 0.26f, -0.78f), Vec3(xRear + 0.02f, deckFloorY + 0.10f, 0.78f), darkTrim);
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.01f, skirtBotY + 0.38f, -hw + 0.08f), Vec3(xRear + 0.03f, deckFloorY + 0.15f, -hw + 0.32f), Vec4(0.88f, 0.12f, 0.12f, 1.0f));
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.01f, skirtBotY + 0.38f,  hw - 0.32f), Vec3(xRear + 0.03f, deckFloorY + 0.15f,  hw - 0.08f), Vec4(0.88f, 0.12f, 0.12f, 1.0f));

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

bool CoachBus::GenerateTextures(VkDevice, VkQueue, VkCommandPool) {
    m_Generated = true;
    return true;
}

} // namespace sphere::vehicles
