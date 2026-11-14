#include "ArticulatedTractor.h"
#include "CabComponents.h"
#include "ChassisComponents.h"
#include "RunningGear.h"
#include <algorithm>

namespace sphere::vehicles {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::cab::CabComponents;
using sphere::vehicles::chassis::ChassisComponents;

ArticulatedTractor::ArticulatedTractor(VehicleID id) : m_Def(CreateArticulatedTractorDefaultDef(id)) {}

std::shared_ptr<Mesh> ArticulatedTractor::GenerateMesh(uint32_t lod) {
    (void)lod;
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(m_Def.dimensions.x, 5.2f, 9.2f);
    const float W = std::clamp(m_Def.dimensions.y, 2.35f, 2.60f);
    const float H = std::clamp(m_Def.dimensions.z, 3.10f, 4.15f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(m_Def.wheel_radius_m, 0.45f, 0.60f);
    const float tireW = std::clamp(m_Def.tire_width_m, 0.28f, 0.38f);
    const float halfTrack = hw - tireW * 0.66f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float frameBotY = std::max(0.70f, wheelR * 2.0f - 0.14f);
    const float frameTopY = frameBotY + 0.25f;

    const Vec4 cabPaint = m_Def.base_color;
    const Vec4 accent = m_Def.accent_color;
    const Vec4 frameDark(0.13f, 0.15f, 0.18f, 1.0f);
    const Vec4 chromeCol(0.80f, 0.83f, 0.88f, 1.0f);
    const Vec4 darkTrim(0.10f, 0.11f, 0.13f, 1.0f);
    const Vec4 windowGlass(0.11f, 0.17f, 0.26f, 1.0f);
    const Vec4 alloyTank(0.76f, 0.79f, 0.84f, 1.0f);

    bool isLongNose = (m_Def.id == VehicleID::TRACTOR_CONVENTIONAL || m_Def.id == VehicleID::TRACTOR_AMERICAN_CAB);
    bool isCondo = (m_Def.id == VehicleID::TRACTOR_AMERICAN_CAB);
    bool isEuro = (m_Def.id == VehicleID::TRACTOR_EURO_CAB);

    // 1. Heavy Twin C-Channel Tractor Chassis Rails
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.10f, xRear - 0.08f, frameBotY, frameTopY, 0.43f, frameDark);

    // 2. Cab Architecture (Conventional Hooded vs Cab-Over-Engine)
    const float cabBaseY = frameBotY - 0.05f;
    const float cabHW = hw - 0.04f;
    float cabBackX = 0.0f;
    float cabFrontMainX = xFront;

    if (isLongNose) {
        float hoodLen = isCondo ? 1.55f : 1.48f;
        float hoodTopY = cabBaseY + 1.08f;
        float cowlTopY = cabBaseY + 1.26f;
        float hoodHW = cabHW * 0.68f;
        cabFrontMainX = xFront + hoodLen;
        cabBackX = cabFrontMainX + (isCondo ? 2.55f : 1.95f);

        // Chrome bumper + sloped fiberglass engine hood
        CabComponents::AppendFrontBumperAndLights(mesh.get(), xFront, cabBaseY - 0.16f, cabBaseY + 0.22f, cabHW, m_Def.headlights_on, chromeCol);
        CabComponents::AppendFrustumX(
            mesh.get(),
            xFront + 0.04f, cabBaseY + 0.20f, hoodTopY, hoodHW,
            cabFrontMainX,  cabBaseY + 0.20f, cowlTopY, cabHW * 0.86f,
            cabPaint
        );
        CabComponents::AppendHeavyGrille(mesh.get(), xFront + 0.03f, cabBaseY + 0.26f, hoodTopY - 0.08f, hoodHW * 0.82f, 6, chromeCol, darkTrim);

        // Dual exterior cylindrical stainless air cleaners behind hood
        ChassisComponents::AppendClosedCylinderY(mesh.get(), cabFrontMainX - 0.12f, -cabHW + 0.08f, cabBaseY + 0.72f, cowlTopY + 0.12f, 0.16f, 16, chromeCol);
        ChassisComponents::AppendClosedCylinderY(mesh.get(), cabFrontMainX - 0.12f,  cabHW - 0.08f, cabBaseY + 0.72f, cowlTopY + 0.12f, 0.16f, 16, chromeCol);

        // Main Cab + Raised Condo Sleeper Berth
        float midCabX = cabFrontMainX + 1.05f;
        float dayRoofY = cabBaseY + 2.15f;
        RunningGear::AppendBox(mesh.get(), Vec3(cabFrontMainX, cabBaseY + 0.18f, -cabHW), Vec3(cabBackX, cowlTopY, cabHW), cabPaint);
        CabComponents::AppendFrustumX(
            mesh.get(),
            cabFrontMainX + 0.06f, cowlTopY, dayRoofY, cabHW - 0.03f,
            midCabX,               cowlTopY, dayRoofY, cabHW,
            cabPaint
        );
        // Sleeper box (condo height vs flat-top sleeper)
        float sleeperTopY = isCondo ? std::max(dayRoofY + 0.75f, H - 0.05f) : (dayRoofY + 0.24f);
        RunningGear::AppendBox(mesh.get(), Vec3(midCabX, cowlTopY, -cabHW), Vec3(cabBackX, sleeperTopY, cabHW), cabPaint);
        if (isCondo) {
            // Sloped condo aero cap blending day-cab roof to high sleeper roof
            CabComponents::AppendFrustumX(
                mesh.get(),
                cabFrontMainX + 0.35f, dayRoofY, dayRoofY + 0.06f, cabHW - 0.10f,
                midCabX,               dayRoofY, sleeperTopY,      cabHW - 0.02f,
                cabPaint
            );
            // Upper condo sleeper skylight windows (opaque tinted glass)
            RunningGear::AppendBox(mesh.get(), Vec3(midCabX + 0.25f, dayRoofY + 0.14f, -cabHW - 0.01f), Vec3(cabBackX - 0.28f, sleeperTopY - 0.18f, -cabHW + 0.01f), windowGlass);
            RunningGear::AppendBox(mesh.get(), Vec3(midCabX + 0.25f, dayRoofY + 0.14f,  cabHW - 0.01f), Vec3(cabBackX - 0.28f, sleeperTopY - 0.18f,  cabHW + 0.01f), windowGlass);
        }

        // Accent racing pinstripe down cab & sleeper
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.12f, cabBaseY + 0.92f, -cabHW - 0.01f), Vec3(cabBackX - 0.04f, cabBaseY + 1.06f, -cabHW + 0.01f), accent);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.12f, cabBaseY + 0.92f,  cabHW - 0.01f), Vec3(cabBackX - 0.04f, cabBaseY + 1.06f,  cabHW + 0.01f), accent);

        CabComponents::AppendWindshield(mesh.get(), cabFrontMainX + 0.02f, cabFrontMainX + 0.18f, cowlTopY + 0.06f, dayRoofY - 0.14f, cabHW - 0.08f, windowGlass, darkTrim);
        CabComponents::AppendDoor(mesh.get(), cabFrontMainX + 0.08f, midCabX - 0.06f, cabBaseY + 0.24f, dayRoofY - 0.18f, -cabHW, false, cabPaint, windowGlass);
        CabComponents::AppendDoor(mesh.get(), cabFrontMainX + 0.08f, midCabX - 0.06f, cabBaseY + 0.24f, dayRoofY - 0.18f,  cabHW, true,  cabPaint, windowGlass);
        CabComponents::AppendWingMirrors(mesh.get(), cabFrontMainX + 0.05f, cowlTopY + 0.35f, cabHW, chromeCol);
        CabComponents::AppendAmberRoofMarkers(mesh.get(), cabFrontMainX + 0.22f, dayRoofY, cabHW, m_Def.headlights_on);
    } else {
        // European / Heavy Cab-Over-Engine Tractor
        float coeLen = isEuro ? 2.45f : 2.55f;
        cabBackX = xFront + coeLen;
        float cabTopY = std::clamp(m_Def.cab_height_m, 3.15f, 3.95f);

        CabComponents::AppendFrontBumperAndLights(mesh.get(), xFront, cabBaseY - 0.18f, cabBaseY + 0.24f, cabHW, m_Def.headlights_on, darkTrim);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.02f, cabBaseY + 0.24f, -cabHW), Vec3(cabBackX, cabBaseY + 1.24f, cabHW), cabPaint);
        CabComponents::AppendFrustumX(
            mesh.get(),
            xFront + 0.06f, cabBaseY + 1.24f, cabTopY - 0.22f, cabHW - 0.02f,
            cabBackX,       cabBaseY + 1.24f, cabTopY,         cabHW,
            cabPaint
        );

        // Euro aero roof spoiler & side cab air-deflector wings
        if (isEuro) {
            RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.06f, cabBaseY + 0.35f, -cabHW), Vec3(cabBackX + 0.22f, cabTopY, -cabHW + 0.05f), cabPaint);
            RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.06f, cabBaseY + 0.35f,  cabHW - 0.05f), Vec3(cabBackX + 0.22f, cabTopY,  cabHW), cabPaint);
        }

        // Accent grille surround / livery fascia
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.02f, cabBaseY + 1.04f, -cabHW - 0.01f), Vec3(cabBackX - 0.02f, cabBaseY + 1.20f, -cabHW + 0.01f), accent);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.02f, cabBaseY + 1.04f,  cabHW - 0.01f), Vec3(cabBackX - 0.02f, cabBaseY + 1.20f,  cabHW + 0.01f), accent);

        CabComponents::AppendWindshield(mesh.get(), xFront + 0.04f, xFront + 0.22f, cabBaseY + 1.28f, cabTopY - 0.34f, cabHW - 0.07f, windowGlass, darkTrim);
        CabComponents::AppendHeavyGrille(mesh.get(), xFront + 0.02f, cabBaseY + 0.32f, cabBaseY + 1.18f, cabHW * 0.74f, 5, isEuro ? accent : chromeCol, darkTrim);
        CabComponents::AppendDoor(mesh.get(), xFront + 0.32f, cabBackX - 0.78f, cabBaseY + 0.28f, cabTopY - 0.38f, -cabHW, false, cabPaint, windowGlass);
        CabComponents::AppendDoor(mesh.get(), xFront + 0.32f, cabBackX - 0.78f, cabBaseY + 0.28f, cabTopY - 0.38f,  cabHW, true,  cabPaint, windowGlass);
        CabComponents::AppendWingMirrors(mesh.get(), xFront + 0.22f, cabBaseY + 1.56f, cabHW, isEuro ? cabPaint : chromeCol);
        CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.18f, cabTopY - 0.22f, cabHW, m_Def.headlights_on);
    }

    // 3. Exhaust Stacks & Fuel Tanks & Side Aero Skirts
    float fifthWheelX = std::clamp(m_Def.fifth_wheel_offset_m, 0.85f, xRear - 0.95f);
    float frontAxleX = isLongNose ? (xFront + 1.15f) : (xFront + 1.42f);
    float driveCenterX = std::min(xRear - 0.95f, frontAxleX + m_Def.wheelbase_m);

    if (m_Def.has_exhaust_stacks) {
        ChassisComponents::AppendExhaustStacks(mesh.get(), cabBackX + 0.14f, frameBotY + 0.05f, std::min(H - 0.05f, m_Def.cab_height_m + 0.15f), hw - 0.18f, chromeCol);
    }

    float tankStart = cabBackX + 0.26f;
    float tankEnd = std::max(tankStart + 0.75f, driveCenterX - (m_Def.drive_config == VehicleDriveConfig::DRIVE_4X2 ? 0.85f : 1.48f));
    if (isEuro) {
        // Painted aerodynamic side chassis fairings covering tanks
        RunningGear::AppendBox(mesh.get(), Vec3(tankStart - 0.08f, frameBotY - 0.26f, -hw + 0.04f), Vec3(tankEnd + 0.06f, frameTopY - 0.02f, -hw + 0.10f), cabPaint);
        RunningGear::AppendBox(mesh.get(), Vec3(tankStart - 0.08f, frameBotY - 0.26f,  hw - 0.10f), Vec3(tankEnd + 0.06f, frameTopY - 0.02f,  hw - 0.04f), cabPaint);
    } else {
        ChassisComponents::AppendFuelTank(mesh.get(), tankStart, tankEnd, frameBotY + 0.04f, -(hw - 0.34f), 0.30f, alloyTank);
        ChassisComponents::AppendFuelTank(mesh.get(), tankStart, tankEnd, frameBotY + 0.04f,  (hw - 0.34f), 0.30f, alloyTank);
    }

    // Diamond-plate catwalk behind cab + air-brake coil stand
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.12f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.68f, frameTopY + 0.04f, 0.45f), alloyTank);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.14f, frameTopY + 0.04f, -0.14f), Vec3(cabBackX + 0.24f, frameTopY + 1.05f, 0.14f), frameDark);

    // 4. Axles & Drivetrain
    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, halfTrack, false, m_Def.steer_angle_deg);
    if (m_Def.drive_config == VehicleDriveConfig::DRIVE_4X2) {
        ChassisComponents::AppendVehicleAxle(mesh.get(), driveCenterX, wheelR, tireW, halfTrack, true, 0.0f);
    } else {
        ChassisComponents::AppendVehicleAxle(mesh.get(), driveCenterX - 0.68f, wheelR, tireW, halfTrack, true, 0.0f);
        ChassisComponents::AppendVehicleAxle(mesh.get(), driveCenterX + 0.68f, wheelR, tireW, halfTrack, true, 0.0f);
    }
    ChassisComponents::AppendDrivetrain(mesh.get(), frontAxleX + 0.3f, driveCenterX, std::max(0.24f, m_Def.ground_clearance_m), wheelR);

    // 5. Exposed SAE 2-Inch Fifth-Wheel Coupling Saddle (Hitch Point)
    ChassisComponents::AppendFifthWheelSaddle(mesh.get(), fifthWheelX, frameTopY);

    // Rear tractor mudguards + taillight bar
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.06f, frameBotY - 0.06f, -halfTrack - 0.22f), Vec3(xRear + 0.02f, frameTopY + 0.02f, -halfTrack + 0.22f), darkTrim);
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.06f, frameBotY - 0.06f,  halfTrack - 0.22f), Vec3(xRear + 0.02f, frameTopY + 0.02f,  halfTrack + 0.22f), darkTrim);
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.04f, frameBotY + 0.04f, -0.45f), Vec3(xRear + 0.02f, frameBotY + 0.14f, -0.22f), Vec4(0.86f, 0.10f, 0.10f, 1.0f));
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.04f, frameBotY + 0.04f,  0.22f), Vec3(xRear + 0.02f, frameBotY + 0.14f,  0.45f), Vec4(0.86f, 0.10f, 0.10f, 1.0f));

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

bool ArticulatedTractor::GenerateTextures(VkDevice, VkQueue, VkCommandPool) {
    m_Generated = true;
    return true;
}

} // namespace sphere::vehicles
