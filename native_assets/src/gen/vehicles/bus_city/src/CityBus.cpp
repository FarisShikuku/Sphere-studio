#include "CityBus.h"
#include "CabComponents.h"
#include "ChassisComponents.h"
#include "RunningGear.h"
#include <algorithm>
#include <cmath>

namespace sphere::vehicles {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::cab::CabComponents;
using sphere::vehicles::chassis::ChassisComponents;

namespace {
void AppendWindowDeck(
    Mesh* mesh,
    float xStart, float xEnd,
    float winBot, float winTop,
    float halfWidth,
    int bays,
    const Vec4& glassCol,
    const Vec4& pillarCol
) {
    float span = xEnd - xStart;
    if (span < 0.8f || bays < 1) return;
    // Continuous dark glass flush belt along both sides
    RunningGear::AppendBox(mesh, Vec3(xStart, winBot, -halfWidth - 0.01f), Vec3(xEnd, winTop, -halfWidth + 0.02f), glassCol);
    RunningGear::AppendBox(mesh, Vec3(xStart, winBot,  halfWidth - 0.02f), Vec3(xEnd, winTop,  halfWidth + 0.01f), glassCol);

    // Structural window mullions/pillars protruding 6 mm outside glass ribbon
    for (int i = 0; i <= bays; ++i) {
        float px = xStart + (span * static_cast<float>(i)) / static_cast<float>(bays);
        RunningGear::AppendBox(mesh, Vec3(px - 0.035f, winBot - 0.01f, -halfWidth - 0.016f), Vec3(px + 0.035f, winTop + 0.01f, -halfWidth + 0.01f), pillarCol);
        RunningGear::AppendBox(mesh, Vec3(px - 0.035f, winBot - 0.01f,  halfWidth - 0.01f), Vec3(px + 0.035f, winTop + 0.01f,  halfWidth + 0.016f), pillarCol);
    }
}

void AppendTransitDoors(
    Mesh* mesh,
    float doorCenterX,
    float floorY, float topY,
    float sideZ,
    float openRatio,
    const Vec4& frameCol,
    const Vec4& glassCol
) {
    float slide = std::clamp(openRatio, 0.0f, 1.0f) * 0.48f;
    float zIn = sideZ - 0.02f;
    float zOut = sideZ + 0.035f + slide * 0.06f;
    // Twin sliding door leaves
    RunningGear::AppendBox(mesh, Vec3(doorCenterX - 0.56f - slide, floorY, zIn), Vec3(doorCenterX - 0.02f - slide, topY, zOut), frameCol);
    RunningGear::AppendBox(mesh, Vec3(doorCenterX + 0.02f + slide, floorY, zIn), Vec3(doorCenterX + 0.56f + slide, topY, zOut), frameCol);
    // Door upper + lower glass panels
    RunningGear::AppendBox(mesh, Vec3(doorCenterX - 0.48f - slide, floorY + 0.35f, zOut - 0.005f), Vec3(doorCenterX - 0.10f - slide, topY - 0.12f, zOut + 0.012f), glassCol);
    RunningGear::AppendBox(mesh, Vec3(doorCenterX + 0.10f + slide, floorY + 0.35f, zOut - 0.005f), Vec3(doorCenterX + 0.48f + slide, topY - 0.12f, zOut + 0.012f), glassCol);
}
} // namespace

CityBus::CityBus(VehicleID id) : m_Def(CreateCityBusDefaultDef(id)) {}

std::shared_ptr<Mesh> CityBus::GenerateMesh(uint32_t lod) {
    (void)lod;
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(m_Def.dimensions.x, 9.0f, 19.5f);
    const float W = std::clamp(m_Def.dimensions.y, 2.35f, 2.60f);
    const float H = std::clamp(m_Def.dimensions.z, 2.85f, 4.55f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(m_Def.wheel_radius_m, 0.42f, 0.55f);
    const float tireW = std::clamp(m_Def.tire_width_m, 0.25f, 0.34f);
    const float halfTrack = hw - tireW * 0.68f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float floorY = std::clamp(m_Def.ground_clearance_m + 0.04f, 0.24f, 0.48f);

    const Vec4 bodyPaint = m_Def.base_color;
    const Vec4 accent = m_Def.accent_color;
    const Vec4 darkTrim(0.10f, 0.11f, 0.13f, 1.0f);
    const Vec4 windowGlass(0.10f, 0.15f, 0.23f, 1.0f);
    const Vec4 destAmber(0.98f, 0.65f, 0.08f, 1.0f);
    const Vec4 roofWhite(0.88f, 0.90f, 0.93f, 1.0f);

    bool isDoubleDecker = (m_Def.id == VehicleID::BUS_CITY_DOUBLE_DECKER);
    bool isBendy = (m_Def.id == VehicleID::BUS_CITY_ARTICULATED_BENDY);
    bool isLowFloor = (m_Def.id == VehicleID::BUS_CITY_LOW_FLOOR);

    // Structural floor chassis platform
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.15f, xRear - 0.15f, floorY, floorY + 0.16f, 0.44f, darkTrim);

    if (isBendy) {
        // Articulated 18m Bendy Bus: Front 10.4m hull + 1.35m corrugated rubber bellows + Rear 6.25m hull
        const float jointFrontX = xFront + L * 0.57f;
        const float jointRearX = jointFrontX + 1.35f;

        // Front & Rear Monocoque Hulls
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.04f, floorY, -hw), Vec3(jointFrontX, H - 0.10f, hw), bodyPaint);
        RunningGear::AppendBox(mesh.get(), Vec3(jointRearX, floorY, -hw), Vec3(xRear - 0.04f, H - 0.10f, hw), bodyPaint);
        // Accent side belt stripe on both hulls
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.06f, floorY + 0.58f, -hw - 0.01f), Vec3(jointFrontX, floorY + 0.80f, -hw + 0.01f), accent);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.06f, floorY + 0.58f,  hw - 0.01f), Vec3(jointFrontX, floorY + 0.80f,  hw + 0.01f), accent);
        RunningGear::AppendBox(mesh.get(), Vec3(jointRearX, floorY + 0.58f, -hw - 0.01f), Vec3(xRear - 0.06f, floorY + 0.80f, -hw + 0.01f), accent);
        RunningGear::AppendBox(mesh.get(), Vec3(jointRearX, floorY + 0.58f,  hw - 0.01f), Vec3(xRear - 0.06f, floorY + 0.80f,  hw + 0.01f), accent);

        // Pleated rubber accordion bellows (8 articulated bellows ribs)
        RunningGear::AppendBox(mesh.get(), Vec3(jointFrontX, floorY + 0.08f, -hw + 0.12f), Vec3(jointRearX, H - 0.18f, hw - 0.12f), Vec4(0.14f, 0.15f, 0.17f, 1.0f));
        for (int b = 0; b < 7; ++b) {
            float bx = jointFrontX + 0.10f + static_cast<float>(b) * 0.185f;
            float zSwing = std::sin(m_Def.action_ratio * 1.57f) * 0.09f * std::sin(static_cast<float>(b) * 0.5f);
            RunningGear::AppendBox(mesh.get(), Vec3(bx - 0.045f, floorY + 0.05f, -hw + 0.04f + zSwing), Vec3(bx + 0.045f, H - 0.14f, hw - 0.04f + zSwing), darkTrim);
        }

        // Windows along front & rear hull
        AppendWindowDeck(mesh.get(), xFront + 1.15f, jointFrontX - 0.25f, floorY + 0.92f, H - 0.34f, hw, 6, windowGlass, darkTrim);
        AppendWindowDeck(mesh.get(), jointRearX + 0.25f, xRear - 0.45f, floorY + 0.92f, H - 0.34f, hw, 4, windowGlass, darkTrim);

        // Three curb-side (+Z) boarding doors
        AppendTransitDoors(mesh.get(), xFront + 1.05f, floorY + 0.06f, H - 0.42f, hw, m_Def.action_ratio, accent, windowGlass);
        AppendTransitDoors(mesh.get(), xFront + 6.20f, floorY + 0.06f, H - 0.42f, hw, m_Def.action_ratio, accent, windowGlass);
        AppendTransitDoors(mesh.get(), jointRearX + 1.90f, floorY + 0.06f, H - 0.42f, hw, m_Def.action_ratio, accent, windowGlass);

        // 3 Axles: Front Steer + Center Drive + Rear Articulated Steerable Tag Axle
        ChassisComponents::AppendVehicleAxle(mesh.get(), xFront + 2.35f, wheelR, tireW, halfTrack, false, m_Def.steer_angle_deg);
        ChassisComponents::AppendVehicleAxle(mesh.get(), jointFrontX - 1.35f, wheelR, tireW, halfTrack, true, 0.0f);
        ChassisComponents::AppendVehicleAxle(mesh.get(), xRear - 2.35f, wheelR, tireW, halfTrack, true, -m_Def.steer_angle_deg * 0.45f);
    } else {
        // Rigid City Bus (Standard, Double-Decker, or Low-Floor BRT)
        float roofBotY = H - (isLowFloor ? 0.28f : 0.12f);
        CabComponents::AppendFrustumX(
            mesh.get(),
            xFront + 0.04f, floorY + 0.08f, roofBotY - 0.12f, hw - 0.02f,
            xFront + 0.45f, floorY + 0.08f, roofBotY,         hw,
            bodyPaint
        );
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.45f, floorY + 0.08f, -hw), Vec3(xRear, roofBotY, hw), bodyPaint);

        // Side livery accent band
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.12f, floorY + 0.56f, -hw - 0.01f), Vec3(xRear - 0.06f, floorY + 0.78f, -hw + 0.01f), accent);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.12f, floorY + 0.56f,  hw - 0.01f), Vec3(xRear - 0.06f, floorY + 0.78f,  hw + 0.01f), accent);

        if (isDoubleDecker) {
            // Lower deck windows + upper deck windows + upper panoramic front windshield
            AppendWindowDeck(mesh.get(), xFront + 0.95f, xRear - 0.55f, floorY + 0.90f, 2.08f, hw, 6, windowGlass, darkTrim);
            AppendWindowDeck(mesh.get(), xFront + 0.55f, xRear - 0.40f, 2.45f, roofBotY - 0.26f, hw, 7, windowGlass, darkTrim);
            CabComponents::AppendWindshield(mesh.get(), xFront + 0.14f, xFront + 0.34f, 2.52f, roofBotY - 0.30f, hw - 0.08f, windowGlass, darkTrim);
            // Center deck divider livery stripe
            RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.10f, 2.15f, -hw - 0.012f), Vec3(xRear - 0.04f, 2.38f, -hw + 0.012f), accent);
            RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.10f, 2.15f,  hw - 0.012f), Vec3(xRear - 0.04f, 2.38f,  hw + 0.012f), accent);
        } else {
            AppendWindowDeck(mesh.get(), xFront + 0.95f, xRear - 0.45f, floorY + 0.90f, roofBotY - 0.28f, hw, 7, windowGlass, darkTrim);
        }

        // Curb-side (+Z) transit doors
        float doorTopY = isDoubleDecker ? 2.12f : (roofBotY - 0.32f);
        AppendTransitDoors(mesh.get(), xFront + 1.05f, floorY + 0.08f, doorTopY, hw, m_Def.action_ratio, accent, windowGlass);
        AppendTransitDoors(mesh.get(), 0.35f, floorY + 0.08f, doorTopY, hw, m_Def.action_ratio, accent, windowGlass);

        // Low-Floor Electric BRT roof battery fairing pod
        if (isLowFloor) {
            CabComponents::AppendFrustumX(
                mesh.get(),
                xFront + 1.10f, roofBotY, H, hw - 0.22f,
                xRear - 1.20f,  roofBotY, H, hw - 0.18f,
                roofWhite
            );
            RunningGear::AppendBox(mesh.get(), Vec3(xFront + 1.35f, roofBotY + 0.04f, -hw + 0.19f), Vec3(xRear - 1.45f, H - 0.05f, hw - 0.19f), accent);
        }

        // Axles
        float frontAxleX = xFront + 2.35f;
        float rearAxleX = std::min(xRear - 2.10f, frontAxleX + m_Def.wheelbase_m);
        ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, halfTrack, false, m_Def.steer_angle_deg);
        ChassisComponents::AppendVehicleAxle(mesh.get(), rearAxleX, wheelR, tireW, halfTrack, true, 0.0f);
        if (m_Def.drive_config == VehicleDriveConfig::DRIVE_6X2_TAG) {
            ChassisComponents::AppendVehicleAxle(mesh.get(), rearAxleX + 1.32f, wheelR, tireW, halfTrack, false, -m_Def.steer_angle_deg * 0.3f);
        }
    }

    // Common Front Driver Windshield, Amber LED Destination Board, Bumper & Wing Mirrors
    float lowerWinTop = isDoubleDecker ? 2.10f : (H - 0.45f);
    CabComponents::AppendFrontBumperAndLights(mesh.get(), xFront, floorY - 0.06f, floorY + 0.32f, hw - 0.02f, m_Def.headlights_on, darkTrim);
    CabComponents::AppendWindshield(mesh.get(), xFront + 0.04f, xFront + 0.26f, floorY + 0.62f, lowerWinTop - 0.22f, hw - 0.08f, windowGlass, darkTrim);
    // Amber LED route display header bar
    RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.16f, lowerWinTop - 0.18f, -0.78f), Vec3(xFront + 0.26f, lowerWinTop - 0.02f, 0.78f), darkTrim);
    RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.14f, lowerWinTop - 0.15f, -0.68f), Vec3(xFront + 0.20f, lowerWinTop - 0.05f, 0.68f), destAmber);
    CabComponents::AppendWingMirrors(mesh.get(), xFront + 0.22f, floorY + 1.32f, hw, darkTrim);

    // Rooftop HVAC climate control pods
    if (!isDoubleDecker) {
        RunningGear::AppendBox(mesh.get(), Vec3(-1.4f, H - 0.12f, -0.75f), Vec3(1.2f, H + 0.08f, 0.75f), roofWhite);
    }

    // Rear engine louvers & vertical transit taillight clusters
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.02f, floorY + 0.22f, -0.72f), Vec3(xRear + 0.03f, floorY + 0.92f, 0.72f), darkTrim);
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.01f, floorY + 0.36f, -hw + 0.06f), Vec3(xRear + 0.04f, floorY + 1.05f, -hw + 0.22f), Vec4(0.86f, 0.10f, 0.10f, 1.0f));
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.01f, floorY + 0.36f,  hw - 0.22f), Vec3(xRear + 0.04f, floorY + 1.05f,  hw - 0.06f), Vec4(0.86f, 0.10f, 0.10f, 1.0f));

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

bool CityBus::GenerateTextures(VkDevice, VkQueue, VkCommandPool) {
    m_Generated = true;
    return true;
}

} // namespace sphere::vehicles
