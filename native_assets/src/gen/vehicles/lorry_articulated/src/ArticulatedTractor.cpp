#include "ArticulatedTractor.h"
#include "BespokeTractors.h"
#include "CabComponents.h"
#include "ChassisComponents.h"
#include "RunningGear.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::cab::CabComponents;
using sphere::vehicles::cab::HullStationX;
using sphere::vehicles::chassis::ChassisComponents;

ArticulatedTractor::ArticulatedTractor(VehicleID id) : m_Def(CreateArticulatedTractorDefaultDef(id)) {}

std::shared_ptr<Mesh> ArticulatedTractor::GenerateMesh(uint32_t lod) {
    (void)lod;
    std::shared_ptr<Mesh> bespoke = nullptr;
    switch (m_Def.id) {
        case VehicleID::TRACTOR_VOLVO_FH_ELECTRIC:      bespoke = trucks::GenerateVolvoFhElectricMesh(m_Def); break;
        case VehicleID::TRACTOR_MERCEDES_EACTROS_600:   bespoke = trucks::GenerateMercedesEActros600Mesh(m_Def); break;
        case VehicleID::TRACTOR_MERCEDES_GENH2:         bespoke = trucks::GenerateMercedesGenH2Mesh(m_Def); break;
        case VehicleID::TRACTOR_SCANIA_45R_ELECTRIC:    bespoke = trucks::GenerateScania45RElectricMesh(m_Def); break;
        case VehicleID::TRACTOR_SCANIA_R560_SUPER:      bespoke = trucks::GenerateScaniaR560SuperMesh(m_Def); break;
        case VehicleID::TRACTOR_SCANIA_S770_LONGLINE:   bespoke = trucks::GenerateScaniaS770LonglineMesh(m_Def); break;
        case VehicleID::TRACTOR_SCANIA_40R_XT_ELECTRIC: bespoke = trucks::GenerateScania40RXtElectricMesh(m_Def); break;
        case VehicleID::TRACTOR_FORD_FMAX_500:          bespoke = trucks::GenerateFordFMax500Mesh(m_Def); break;
        case VehicleID::TRACTOR_IVECO_SEWAY_ELECTRIC:   bespoke = trucks::GenerateIvecoSEwayElectricMesh(m_Def); break;
        case VehicleID::TRACTOR_VW_CONSTELLATION_25390: bespoke = trucks::GenerateVwConstellation25390Mesh(m_Def); break;
        case VehicleID::TRACTOR_VW_METEOR_28460:        bespoke = trucks::GenerateVwMeteor28460Mesh(m_Def); break;
        case VehicleID::TRACTOR_MAN_TGX_18510:          bespoke = trucks::GenerateManTgx18510Mesh(m_Def); break;
        default: break;
    }
    if (bespoke) {
        m_Bounds = bespoke->bounds;
        m_CachedMesh = bespoke;
        return bespoke;
    }
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(m_Def.dimensions.x, 5.2f, 9.2f);
    const float W = std::clamp(m_Def.dimensions.y, 2.35f, 2.60f);
    const float H = std::clamp(m_Def.dimensions.z, 3.10f, 4.15f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(m_Def.wheel_radius_m, 0.45f, 0.60f);
    const float tireW = std::clamp(m_Def.tire_width_m, 0.28f, 0.38f);

    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = hw - tireW * 1.06f - 0.025f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float frameBotY = std::max(0.70f, wheelR * 2.0f - 0.12f);
    const float frameTopY = frameBotY + 0.25f;
    const float archTopY = wheelR * 2.0f + 0.14f;
    const float archHalfLen = wheelR + 0.13f;

    const Vec4 cabPaint = m_Def.base_color;
    const Vec4 accent = m_Def.accent_color;
    const Vec4 frameDark(0.13f, 0.15f, 0.18f, 1.0f);
    const Vec4 chromeCol(0.84f, 0.87f, 0.92f, 1.0f);
    const Vec4 glossBlack(0.06f, 0.07f, 0.09f, 1.0f);
    const Vec4 darkTrim(0.10f, 0.11f, 0.13f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 interiorSeat(0.18f, 0.20f, 0.24f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.15f, 1.0f);
    const Vec4 alloyTank(0.78f, 0.81f, 0.86f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);

    bool isLongNose = (m_Def.id == VehicleID::TRACTOR_CONVENTIONAL || m_Def.id == VehicleID::TRACTOR_AMERICAN_CAB);
    bool isCondo = (m_Def.id == VehicleID::TRACTOR_AMERICAN_CAB);
    bool isEuro = (m_Def.id == VehicleID::TRACTOR_EURO_CAB);

    // 1. Heavy Twin C-Channel Tractor Chassis Rails
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.14f, xRear - 0.08f, frameBotY, frameTopY, 0.43f, frameDark);

    const float cabBaseY = frameBotY - 0.04f;
    const float cabClearY = std::max(cabBaseY + 0.24f, archTopY + 0.02f); // Cleanly above front tires
    const float cabHW = hw - 0.04f;
    float cabBackX = 0.0f;
    float cabFrontMainX = xFront;

    float fifthWheelX = std::clamp(m_Def.fifth_wheel_offset_m, 0.85f, xRear - 0.95f);
    float frontAxleX = isLongNose ? (xFront + 1.15f) : (xFront + 1.42f);
    float driveCenterX = std::min(xRear - 0.95f, frontAxleX + m_Def.wheelbase_m);

    // Track translucent sleeper side portholes to emit LAST in glass pass
    bool emitCondoUpperGlass = false;
    float condoGx0 = 0.0f, condoGx1 = 0.0f, condoGy0 = 0.0f, condoGy1 = 0.0f;
    bool emitCoePortholeGlass = false;
    float coePx0 = 0.0f, coePx1 = 0.0f, coePy0 = 0.0f, coePy1 = 0.0f;

    float windFrontX = 0.0f, windBrowX = 0.0f, windBotY = 0.0f, windTopY = 0.0f, windBulge = 0.0f;
    float dDoorX0 = 0.0f, dDoorX1 = 0.0f, dDoorBotY = 0.0f, dDoorTopY = 0.0f;

    if (isLongNose) {
        // Modern Aero Conventional / Condo Sleeper (Volvo VNL / Kenworth T680 Next-Gen silhouette)
        float hoodLen = isCondo ? 1.56f : 1.46f;
        float hoodTopY = cabBaseY + 1.05f;
        float cowlTopY = cabBaseY + 1.28f;
        float hoodHW = cabHW * 0.72f;
        cabFrontMainX = xFront + hoodLen;
        cabBackX = cabFrontMainX + (isCondo ? 2.65f : 1.98f);
        float midCabX = cabFrontMainX + 1.08f;
        float dayRoofY = cabBaseY + 2.18f;
        float sleeperTopY = isCondo ? std::max(dayRoofY + 0.78f, H - 0.08f) : (dayRoofY + 0.22f);

        windFrontX = cabFrontMainX + 0.02f;
        windBrowX  = cabFrontMainX + 0.34f;
        windBotY   = cowlTopY + 0.06f;
        windTopY   = dayRoofY - 0.18f;
        windBulge  = 0.09f;

        dDoorX0   = cabFrontMainX + 0.14f;
        dDoorX1   = midCabX - 0.06f;
        dDoorBotY = cabClearY;
        dDoorTopY = windTopY - 0.02f;

        // 2A. Interior (visible through windshield, sleeper windows & openable cab doors)
        CabComponents::AppendTruckCabInterior(
            mesh.get(),
            cabFrontMainX + 0.10f, cabBackX - 0.08f,
            cabClearY + 0.06f, dayRoofY - 0.14f,
            cabHW, isCondo, dashDark, interiorSeat, accent
        );

        // Swept aero bumper + LED DRL fascia
        CabComponents::AppendModernLedFascia(
            mesh.get(), xFront + 0.08f, cabBaseY - 0.16f, cabBaseY + 0.24f,
            cabHW - 0.04f, m_Def.headlights_on,
            isCondo ? cabPaint : chromeCol, glossBlack
        );

        // Multi-station sculpted sloped hood sitting cleanly above front steer wheel well
        std::vector<HullStationX> hoodHull = {
            {xFront + 0.14f,          cabClearY + 0.02f, hoodTopY,        hoodHW,        0.08f, 0.20f, 0.06f, 0.06f},
            {xFront + hoodLen * 0.5f, cabClearY + 0.02f, (hoodTopY + cowlTopY) * 0.5f, hoodHW + 0.10f, 0.08f, 0.22f, 0.07f, 0.04f},
            {cabFrontMainX + 0.04f,   cabClearY,         cowlTopY,        cabHW * 0.96f, 0.08f, 0.22f, 0.06f, 0.00f},
        };
        CabComponents::AppendSculptedHullX(mesh.get(), hoodHull, cabPaint, 4);

        // Sculpted cowl pedestal below windshield + lower cab rocker below door sill
        std::vector<HullStationX> cowlPod = {
            {cabFrontMainX,         cabClearY, windBotY, cabHW - 0.04f, 0.08f, 0.10f, 0.02f, 0.08f},
            {cabFrontMainX + 0.20f, cabClearY, windBotY, cabHW,         0.08f, 0.10f, 0.02f, 0.06f},
        };
        CabComponents::AppendSculptedHullX(mesh.get(), cowlPod, cabPaint, 4);

        // Slanted A-pillars & B-pillar cab walls framing door & sleeper bay
        for (int side : {-1, 1}) {
            float s = static_cast<float>(side);
            float zMin = s > 0 ? cabHW - 0.085f : -cabHW;
            float zMax = s > 0 ? cabHW : -cabHW + 0.085f;
            RunningGear::AppendBox(mesh.get(), Vec3(cabFrontMainX + 0.06f, windBotY - 0.02f, zMin), Vec3(windBrowX + 0.04f, windTopY + 0.02f, zMax), glossBlack);
        }

        // Cab Roof Canopy above windshield top
        std::vector<HullStationX> roofHull = {
            {cabFrontMainX + 0.30f, windTopY, dayRoofY - 0.08f, cabHW - 0.02f, 0.03f, 0.20f, 0.07f, 0.06f},
            {midCabX,               windTopY, isCondo ? sleeperTopY - 0.04f : sleeperTopY, cabHW, 0.03f, 0.24f, 0.09f, 0.01f},
            {cabBackX,              windTopY, sleeperTopY, cabHW, 0.03f, 0.24f, 0.09f, 0.00f},
        };
        CabComponents::AppendSculptedHullX(mesh.get(), roofHull, cabPaint, 4);

        // Rear Sleeper Compartment Shell [midCabX .. cabBackX, cabClearY .. windTopY] + Rear Back Wall
        std::vector<HullStationX> sleeperWall = {
            {midCabX - 0.04f, cabClearY, windTopY + 0.02f, cabHW, 0.08f, 0.08f, 0.0f, 0.0f},
            {cabBackX,        cabClearY, windTopY + 0.02f, cabHW, 0.08f, 0.08f, 0.0f, 0.0f},
        };
        CabComponents::AppendSculptedHullX(mesh.get(), sleeperWall, cabPaint, 4);

        // Entry step bar below cab doors (behind front wheel well)
        RunningGear::AppendBox(mesh.get(), Vec3(dDoorX0, frameBotY - 0.06f, -cabHW + 0.02f), Vec3(dDoorX1, cabClearY + 0.02f, -cabHW + 0.14f), alloyTank);
        RunningGear::AppendBox(mesh.get(), Vec3(dDoorX0, frameBotY - 0.06f,  cabHW - 0.14f), Vec3(dDoorX1, cabClearY + 0.02f,  cabHW - 0.02f), alloyTank);

        CabComponents::AppendHeavyGrille(mesh.get(), xFront + 0.04f, cabBaseY + 0.28f, hoodTopY - 0.08f, hoodHW * 0.78f, 6, chromeCol, glossBlack);
        if (!isCondo) {
            ChassisComponents::AppendClosedCylinderY(mesh.get(), cabFrontMainX - 0.10f, -cabHW + 0.09f, cabBaseY + 0.72f, cowlTopY + 0.10f, 0.14f, 16, chromeCol);
            ChassisComponents::AppendClosedCylinderY(mesh.get(), cabFrontMainX - 0.10f,  cabHW - 0.09f, cabBaseY + 0.72f, cowlTopY + 0.10f, 0.14f, 16, chromeCol);
        }

        if (isCondo) {
            RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.04f, cabBaseY + 0.30f, -cabHW), Vec3(cabBackX + 0.32f, sleeperTopY - 0.16f, -cabHW + 0.05f), cabPaint);
            RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.04f, cabBaseY + 0.30f,  cabHW - 0.05f), Vec3(cabBackX + 0.32f, sleeperTopY - 0.16f,  cabHW), cabPaint);
            emitCondoUpperGlass = true;
            condoGx0 = midCabX + 0.22f; condoGx1 = cabBackX - 0.26f;
            condoGy0 = dayRoofY + 0.12f; condoGy1 = sleeperTopY - 0.26f;
        }

        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.24f, cabBaseY + 0.90f, -cabHW - 0.01f), Vec3(cabBackX - 0.06f, cabBaseY + 1.02f, -cabHW + 0.01f), accent);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.24f, cabBaseY + 0.90f,  cabHW - 0.01f), Vec3(cabBackX - 0.06f, cabBaseY + 1.02f,  cabHW + 0.01f), accent);

        CabComponents::AppendCurvedBlackNoseMask(
            mesh.get(), windFrontX, windBrowX,
            cowlTopY - 0.04f, windBotY, windTopY,
            cabHW - 0.06f, windBulge, glossBlack, glossBlack, false, true
        );
        CabComponents::AppendAnimatedDoor(mesh.get(), dDoorX0, dDoorX1, dDoorBotY, dDoorTopY, -cabHW, false, m_Def.door_open_ratio, false, false, cabPaint, glossBlack, sideGlass);
        CabComponents::AppendAnimatedDoor(mesh.get(), dDoorX0, dDoorX1, dDoorBotY, dDoorTopY,  cabHW, true,  m_Def.door_open_ratio, false, false, cabPaint, glossBlack, sideGlass);
        CabComponents::AppendCantileverCoachMirrors(mesh.get(), cabFrontMainX + 0.38f, dayRoofY - 0.30f, cabHW, isCondo ? cabPaint : chromeCol);
        CabComponents::AppendAmberRoofMarkers(mesh.get(), cabFrontMainX + 0.42f, dayRoofY - 0.04f, cabHW * 0.85f, m_Def.headlights_on);
    } else {
        // Modern European FH Aero / Scania S-Series Cab-Over-Engine Tractor
        float coeLen = isEuro ? 2.55f : 2.48f;
        cabBackX = xFront + coeLen;
        float cabTopY = std::clamp(m_Def.cab_height_m, 3.15f, 3.95f);

        windFrontX = xFront + 0.14f;
        windBrowX  = xFront + 0.44f;
        windBotY   = std::max(cabBaseY + 1.28f, cabClearY + 1.04f);
        windTopY   = cabTopY - 0.26f;
        windBulge  = 0.12f;

        dDoorX0   = xFront + 0.44f;
        dDoorX1   = cabBackX - 0.72f;
        dDoorBotY = cabClearY + 0.02f;
        dDoorTopY = cabTopY - 0.32f;

        // 2B. COE Cab Interior (Seats, dashboard, steering wheel, sleeper bunk)
        CabComponents::AppendTruckCabInterior(
            mesh.get(),
            xFront + 0.34f, cabBackX - 0.08f,
            cabClearY + 0.06f, cabTopY - 0.16f,
            cabHW, true, dashDark, interiorSeat, accent
        );

        CabComponents::AppendModernLedFascia(
            mesh.get(), xFront + 0.12f, cabBaseY - 0.18f, cabBaseY + 0.24f,
            cabHW - 0.03f, m_Def.headlights_on,
            isEuro ? cabPaint : darkTrim, glossBlack
        );

        // Lower COE nose shell below windshield (cleanly above front tire arch)
        std::vector<HullStationX> coeNose = {
            {xFront + 0.16f, cabClearY, cabBaseY + 1.24f, cabHW - 0.05f, 0.09f, 0.12f, 0.02f, 0.10f},
            {xFront + 0.48f, cabClearY, windBotY,         cabHW,         0.08f, 0.10f, 0.02f, 0.08f},
        };
        CabComponents::AppendSculptedHullX(mesh.get(), coeNose, cabPaint, 4);

        // Upper COE roof canopy above windshield & doors
        std::vector<HullStationX> coeRoof = {
            {xFront + 0.40f, windTopY, cabTopY - 0.14f, cabHW - 0.02f, 0.03f, 0.22f, 0.08f, 0.08f},
            {xFront + 1.15f, windTopY, cabTopY - 0.06f, cabHW,         0.03f, 0.22f, 0.09f, 0.02f},
            {cabBackX,       windTopY, cabTopY - 0.06f, cabHW,         0.03f, 0.22f, 0.09f, 0.00f},
        };
        CabComponents::AppendSculptedHullX(mesh.get(), coeRoof, cabPaint, 4);

        // Rear Sleeper Compartment Walls behind door latch jamb [cabBackX - 0.72f .. cabBackX]
        std::vector<HullStationX> coeSleeper = {
            {cabBackX - 0.72f, cabClearY, windTopY + 0.02f, cabHW, 0.08f, 0.08f, 0.0f, 0.0f},
            {cabBackX,         cabClearY, windTopY + 0.02f, cabHW, 0.08f, 0.08f, 0.0f, 0.0f},
        };
        CabComponents::AppendSculptedHullX(mesh.get(), coeSleeper, cabPaint, 4);

        // COE A-pillars framing curved windshield
        for (int side : {-1, 1}) {
            float s = static_cast<float>(side);
            RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.28f, windBotY - 0.04f, s > 0 ? cabHW - 0.085f : -cabHW), Vec3(xFront + 0.48f, windTopY + 0.03f, s > 0 ? cabHW : -cabHW + 0.085f), glossBlack);
        }

        if (isEuro) {
            std::vector<HullStationX> roofPod = {
                {xFront + 0.95f, cabTopY - 0.08f, cabTopY + 0.02f, cabHW - 0.12f, 0.04f, 0.12f, 0.03f, 0.08f},
                {cabBackX,       cabTopY - 0.08f, std::min(H - 0.02f, cabTopY + 0.26f), cabHW - 0.05f, 0.04f, 0.16f, 0.05f, 0.00f},
            };
            CabComponents::AppendSculptedHullX(mesh.get(), roofPod, cabPaint, 3);
            RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.06f, cabBaseY + 0.35f, -cabHW), Vec3(cabBackX + 0.26f, cabTopY - 0.14f, -cabHW + 0.05f), cabPaint);
            RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.06f, cabBaseY + 0.35f,  cabHW - 0.05f), Vec3(cabBackX + 0.26f, cabTopY - 0.14f,  cabHW), cabPaint);
        }

        CabComponents::AppendCurvedBlackNoseMask(
            mesh.get(), windFrontX, windBrowX,
            cabClearY + 0.80f, windBotY, windTopY,
            cabHW - 0.04f, windBulge, glossBlack, glossBlack, false, true
        );
        CabComponents::AppendHeavyGrille(
            mesh.get(), xFront + 0.03f,
            cabClearY + 0.06f, cabClearY + 0.78f,
            cabHW * 0.72f, 5, isEuro ? accent : chromeCol, glossBlack
        );

        CabComponents::AppendAnimatedDoor(mesh.get(), dDoorX0, dDoorX1, dDoorBotY, dDoorTopY, -cabHW, false, m_Def.door_open_ratio, false, false, cabPaint, glossBlack, sideGlass);
        CabComponents::AppendAnimatedDoor(mesh.get(), dDoorX0, dDoorX1, dDoorBotY, dDoorTopY,  cabHW, true,  m_Def.door_open_ratio, false, false, cabPaint, glossBlack, sideGlass);

        emitCoePortholeGlass = true;
        coePx0 = cabBackX - 0.62f; coePx1 = cabBackX - 0.18f;
        coePy0 = cabClearY + 1.00f; coePy1 = cabTopY - 0.44f;
        RunningGear::AppendBox(mesh.get(), Vec3(coePx0 - 0.02f, coePy0 - 0.02f, -cabHW - 0.008f), Vec3(coePx1 + 0.02f, coePy1 + 0.02f, -cabHW + 0.01f), glossBlack);
        RunningGear::AppendBox(mesh.get(), Vec3(coePx0 - 0.02f, coePy0 - 0.02f,  cabHW - 0.01f), Vec3(coePx1 + 0.02f, coePy1 + 0.02f,  cabHW + 0.008f), glossBlack);

        CabComponents::AppendCantileverCoachMirrors(mesh.get(), xFront + 0.48f, cabTopY - 0.34f, cabHW, isEuro ? cabPaint : darkTrim);
        CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.46f, cabTopY - 0.12f, cabHW * 0.85f, m_Def.headlights_on);
    }

    // 3. Exhaust Stacks, Fuel Tanks & Streamlined Chassis Aero Skirts
    if (m_Def.has_exhaust_stacks) {
        ChassisComponents::AppendExhaustStacks(mesh.get(), cabBackX + 0.14f, frameBotY + 0.05f, std::min(H - 0.05f, m_Def.cab_height_m + 0.15f), hw - 0.18f, chromeCol);
    }

    float driveFrontBound = (m_Def.drive_config == VehicleDriveConfig::DRIVE_4X2) ? (driveCenterX - archHalfLen - 0.10f) : (driveCenterX - 0.68f - archHalfLen - 0.08f);
    float tankStart = std::max(cabBackX + 0.22f, frontAxleX + archHalfLen + 0.08f);
    float tankEnd = std::max(tankStart + 0.60f, std::min(driveFrontBound, cabBackX + 1.95f));
    if (tankEnd > tankStart + 0.45f) {
        if (isEuro || isCondo) {
            RunningGear::AppendBox(mesh.get(), Vec3(tankStart - 0.06f, frameBotY - 0.24f, -hw + 0.03f), Vec3(tankEnd + 0.04f, frameTopY + 0.02f, -hw + 0.10f), cabPaint);
            RunningGear::AppendBox(mesh.get(), Vec3(tankStart - 0.06f, frameBotY - 0.24f,  hw - 0.10f), Vec3(tankEnd + 0.04f, frameTopY + 0.02f,  hw - 0.03f), cabPaint);
            RunningGear::AppendBox(mesh.get(), Vec3(tankStart - 0.04f, frameBotY - 0.10f, -hw + 0.02f), Vec3(tankEnd + 0.02f, frameBotY - 0.04f, -hw + 0.04f), accent);
            RunningGear::AppendBox(mesh.get(), Vec3(tankStart - 0.04f, frameBotY - 0.10f,  hw - 0.04f), Vec3(tankEnd + 0.02f, frameBotY - 0.04f,  hw - 0.02f), accent);
        } else {
            ChassisComponents::AppendFuelTank(mesh.get(), tankStart, tankEnd, frameBotY + 0.04f, -(hw - 0.34f), 0.30f, alloyTank);
            ChassisComponents::AppendFuelTank(mesh.get(), tankStart, tankEnd, frameBotY + 0.04f,  (hw - 0.34f), 0.30f, alloyTank);
        }
    }

    // Diamond-plate catwalk behind cab + air-brake coil stand
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.12f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.68f, frameTopY + 0.04f, 0.45f), alloyTank);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.14f, frameTopY + 0.04f, -0.14f), Vec3(cabBackX + 0.24f, frameTopY + 1.05f, 0.14f), frameDark);

    // 4. Axles (accurate track widths!), Wheel-Well Liners, Drivetrain & Fenders
    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, frontHalfTrack, false, m_Def.steer_angle_deg);
    CabComponents::AppendWheelWellLiner(mesh.get(), frontAxleX, archHalfLen, frameBotY - 0.10f, archTopY, cabHW, cabHW - tireW - 0.14f, linerDark);
    CabComponents::AppendWheelArchFender(mesh.get(), frontAxleX, wheelR, wheelR + 0.09f, -cabHW, false, 0.038f, darkTrim);
    CabComponents::AppendWheelArchFender(mesh.get(), frontAxleX, wheelR, wheelR + 0.09f,  cabHW, true,  0.038f, darkTrim);

    if (m_Def.drive_config == VehicleDriveConfig::DRIVE_4X2) {
        ChassisComponents::AppendVehicleAxle(mesh.get(), driveCenterX, wheelR, tireW, rearHalfTrack, true, 0.0f);
        CabComponents::AppendWheelArchFender(mesh.get(), driveCenterX, wheelR, wheelR + 0.08f, -hw, false, tireW * 1.15f, darkTrim);
        CabComponents::AppendWheelArchFender(mesh.get(), driveCenterX, wheelR, wheelR + 0.08f,  hw, true,  tireW * 1.15f, darkTrim);
    } else {
        float d0 = driveCenterX - 0.68f;
        float d1 = driveCenterX + 0.68f;
        ChassisComponents::AppendVehicleAxle(mesh.get(), d0, wheelR, tireW, rearHalfTrack, true, 0.0f);
        ChassisComponents::AppendVehicleAxle(mesh.get(), d1, wheelR, tireW, rearHalfTrack, true, 0.0f);
        for (float dx : {d0, d1}) {
            CabComponents::AppendWheelArchFender(mesh.get(), dx, wheelR, wheelR + 0.08f, -hw, false, tireW * 1.15f, darkTrim);
            CabComponents::AppendWheelArchFender(mesh.get(), dx, wheelR, wheelR + 0.08f,  hw, true,  tireW * 1.15f, darkTrim);
        }
    }
    ChassisComponents::AppendDrivetrain(mesh.get(), frontAxleX + 0.3f, driveCenterX, std::max(0.24f, m_Def.ground_clearance_m), wheelR);

    // 5. Exposed SAE 2-Inch Fifth-Wheel Coupling Saddle + Mudguards & Taillights
    ChassisComponents::AppendFifthWheelSaddle(mesh.get(), fifthWheelX, frameTopY);
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.06f, frameBotY - 0.06f, -hw + 0.04f), Vec3(xRear + 0.02f, frameTopY + 0.02f, -hw + 0.48f), darkTrim);
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.06f, frameBotY - 0.06f,  hw - 0.48f), Vec3(xRear + 0.02f, frameTopY + 0.02f,  hw - 0.04f), darkTrim);
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.04f, frameBotY + 0.04f, -0.46f), Vec3(xRear + 0.025f, frameBotY + 0.14f, -0.22f), Vec4(0.90f, 0.10f, 0.12f, 1.0f));
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.04f, frameBotY + 0.04f,  0.22f), Vec3(xRear + 0.025f, frameBotY + 0.14f,  0.46f), Vec4(0.90f, 0.10f, 0.12f, 1.0f));

    // 6. Translucent Cab Glass Pass (DRAWN LAST over cab interior)
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), windFrontX, windBrowX, windBotY, windTopY, cabHW - 0.05f, windBulge, windshieldGlass);
    CabComponents::AppendAnimatedDoor(mesh.get(), dDoorX0, dDoorX1, dDoorBotY, dDoorTopY, -cabHW, false, m_Def.door_open_ratio, false, true, cabPaint, glossBlack, sideGlass);
    CabComponents::AppendAnimatedDoor(mesh.get(), dDoorX0, dDoorX1, dDoorBotY, dDoorTopY,  cabHW, true,  m_Def.door_open_ratio, false, true, cabPaint, glossBlack, sideGlass);
    if (emitCondoUpperGlass) {
        RunningGear::AppendBox(mesh.get(), Vec3(condoGx0, condoGy0, -cabHW - 0.010f), Vec3(condoGx1, condoGy1, -cabHW + 0.004f), sideGlass);
        RunningGear::AppendBox(mesh.get(), Vec3(condoGx0, condoGy0,  cabHW - 0.004f), Vec3(condoGx1, condoGy1,  cabHW + 0.010f), sideGlass);
    }
    if (emitCoePortholeGlass) {
        RunningGear::AppendBox(mesh.get(), Vec3(coePx0, coePy0, -cabHW - 0.010f), Vec3(coePx1, coePy1, -cabHW + 0.004f), sideGlass);
        RunningGear::AppendBox(mesh.get(), Vec3(coePx0, coePy0,  cabHW - 0.004f), Vec3(coePx1, coePy1,  cabHW + 0.010f), sideGlass);
    }

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
