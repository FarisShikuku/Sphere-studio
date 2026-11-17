#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/Volkswagen_Constellation_25-390_Tractor_Truck_3axle_2011_600_0001-551x551.webp
// Signature features: Carved rounded white mid-roof sleeper cab with aero roof blade & curved
// black sunvisor, panoramic Constellation windshield ratio aligned with door glass, curved 4-slat
// matte-black grille with circular chrome VW medallion, curved bumper with matte-black corner cheek
// housings holding stacked circular twin projector pods, 3-step stairwell & alloy fuel tanks.
std::shared_ptr<Mesh> GenerateVwConstellation25390Mesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 5.8f, 8.8f);
    const float W = std::clamp(def.dimensions.y, 2.34f, 2.58f);
    const float H = std::clamp(def.dimensions.z, 3.10f, 3.85f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(def.wheel_radius_m, 0.45f, 0.56f);
    const float tireW = std::clamp(def.tire_width_m, 0.28f, 0.36f);

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float frameBotY = std::max(0.68f, wheelR * 2.0f - 0.14f);
    const float frameTopY = frameBotY + 0.25f;
    const float archTopY = wheelR * 2.0f + 0.14f;
    const float archHalfLen = wheelR + 0.13f;

    const float cabBaseY = frameBotY - 0.04f;
    const float cabClearY = std::max(cabBaseY + 0.22f, archTopY + 0.02f);
    const float cabHW = hw - 0.03f;
    const float cabBackX = xFront + 2.34f;
    const float cabTopY = std::clamp(def.cab_height_m, 3.15f, 3.65f);

    const float frontAxleX = xFront + 1.48f;
    const float driveCenterX = std::min(xRear - 1.10f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX = std::clamp(def.fifth_wheel_offset_m, 1.15f, xRear - 0.95f);

    const Vec4 whitePaint = def.base_color;
    const Vec4 bumperDark = def.accent_color;
    const Vec4 frameDark(0.13f, 0.15f, 0.18f, 1.0f);
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 chromeCol(0.88f, 0.91f, 0.96f, 1.0f);
    const Vec4 alloyTank(0.78f, 0.81f, 0.86f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 interiorSeat(0.18f, 0.20f, 0.24f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.15f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);
    const Vec4 amberCol(0.96f, 0.62f, 0.08f, 1.0f);
    const Vec4 lampCol = def.headlights_on ? Vec4(0.96f, 0.98f, 1.00f, 1.0f) : chromeCol;

    // 1. Chassis & 6x2 drive+tag axles
    AppendTractorChassisBase(
        mesh.get(), def, xFront, xRear, frontAxleX, driveCenterX, fifthWheelX,
        wheelR, tireW, hw, cabHW, frameBotY, frameTopY, archTopY, archHalfLen,
        frameDark, bumperDark, linerDark, alloyTank
    );

    const float windFrontX = xFront + 0.14f;
    const float windBrowX  = xFront + 0.34f;
    const float windBotY   = cabClearY + 1.10f;
    const float windTopY   = std::min(cabTopY - 0.38f, windBotY + 0.78f);
    const float doorX0     = xFront + 0.44f;
    const float doorX1     = cabBackX - 0.54f;
    const float doorTopY   = windTopY + 0.05f;
    const float doorBeltY  = windBotY - 0.02f;

    // 2. Interior
    CabComponents::AppendTruckCabInterior(
        mesh.get(), xFront + 0.30f, cabBackX - 0.08f,
        cabClearY + 0.06f, cabTopY - 0.16f, cabHW, true, dashDark, interiorSeat, chromeCol
    );

    // 3. Smoothly Carved Curved Lower Nose & Wrap Bumper
    AppendCarvedLowerNose(mesh.get(), xFront, xFront + 0.48f, cabClearY + 0.04f, windBotY, cabHW, whitePaint);
    AppendCurvedPanelX(mesh.get(), xFront - 0.002f, 0.22f, cabBaseY - 0.14f, cabClearY + 0.06f, cabBaseY - 0.14f, cabClearY + 0.06f, -cabHW, cabHW, cabHW, 0.09f, 8, whitePaint, 0.032f);

    // Curved matte-black 2-slat lower bumper mouth + Euro plate
    AppendVGrilleSlats(mesh.get(), xFront - 0.008f, cabBaseY - 0.08f, cabClearY + 0.05f, cabHW * 0.46f, cabHW * 0.50f, 2, bumperDark, glossBlack, -1, chromeCol, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.016f, 0.024f, cabBaseY - 0.01f, cabBaseY + 0.05f, cabBaseY - 0.01f, cabBaseY + 0.05f, -0.22f, 0.22f, cabHW, 0.09f, 4, Vec4(0.95f, 0.96f, 0.98f, 1.0f), 0.006f);

    // Upper matte-black 4-slat VW Constellation grille + Large Round Chrome VW Medallion + Silver Tags
    float upGrBot = cabClearY + 0.14f;
    float upGrTop = windBotY - 0.16f;
    AppendVGrilleSlats(mesh.get(), xFront - 0.005f, upGrBot, upGrTop, cabHW * 0.46f, cabHW * 0.52f, 4, bumperDark, glossBlack, -1, chromeCol, cabHW, 0.09f);

    float vwY = (upGrBot + upGrTop) * 0.5f;
    AppendFrontDiscX(mesh.get(), xFront - 0.014f, vwY, 0.0f, 0.135f, 0.025f, 24, glossBlack, chromeCol);
    AppendFrontDiscX(mesh.get(), xFront - 0.022f, vwY, 0.0f, 0.118f, 0.014f, 24, chromeCol, chromeCol);
    AppendFrontDiscX(mesh.get(), xFront - 0.028f, vwY, 0.0f, 0.095f, 0.010f, 24, glossBlack, chromeCol);
    AppendSlantedRibbonX(mesh.get(), xFront - 0.032f, 0.012f, vwY + 0.07f, -0.055f, vwY - 0.06f, 0.0f, 0.014f, chromeCol, cabHW, 0.09f);
    AppendSlantedRibbonX(mesh.get(), xFront - 0.032f, 0.012f, vwY + 0.07f,  0.055f, vwY - 0.06f, 0.0f, 0.014f, chromeCol, cabHW, 0.09f);

    AppendCurvedPanelX(mesh.get(), xFront - 0.002f, 0.025f, windBotY - 0.11f, windBotY - 0.07f, windBotY - 0.11f, windBotY - 0.07f, -cabHW * 0.48f, -cabHW * 0.18f, cabHW, 0.09f, 2, chromeCol, 0.004f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.002f, 0.025f, windBotY - 0.11f, windBotY - 0.07f, windBotY - 0.11f, windBotY - 0.07f,  cabHW * 0.18f,  cabHW * 0.44f, cabHW, 0.09f, 2, chromeCol, 0.004f);

    // Signature Stacked Circular Twin-Pod Corner Headlamps in Carved Curved Matte-Black Cheek Towers
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zPod = s * (cabHW * 0.66f);
        float z0 = zPod - 0.13f, z1 = zPod + 0.13f;
        float podX = CurvedNoseXAtZ(zPod, xFront - 0.004f, cabHW, 0.09f);
        AppendCurvedPanelX(mesh.get(), xFront - 0.002f, 0.115f, cabBaseY - 0.07f, cabClearY + 0.36f, cabBaseY - 0.07f, cabClearY + 0.36f, z0, z1, cabHW, 0.09f, 3, bumperDark, 0.024f);
        AppendCircularLampPodX(mesh.get(), podX - 0.006f, cabClearY + 0.22f, zPod, 0.085f, 0.056f, 18, glossBlack, amberCol, lampCol);
        AppendCircularLampPodX(mesh.get(), podX - 0.008f, cabBaseY + 0.12f,  zPod, 0.080f, 0.052f, 18, glossBlack, chromeCol, lampCol);
        AppendCircularLampPodX(mesh.get(), podX - 0.008f, cabBaseY - 0.02f, zPod, 0.052f, 0.032f, 16, glossBlack, chromeCol, lampCol);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.14f, cabClearY + 0.16f, s > 0 ? cabHW - 0.07f : -cabHW - 0.01f), Vec3(xFront + 0.44f, windBotY - 0.06f, s > 0 ? cabHW + 0.01f : -cabHW + 0.07f), bumperDark);
    }

    AppendCabEntrySteps(mesh.get(), xFront + 0.44f, frontAxleX - archHalfLen + 0.02f, frameBotY, cabClearY, cabHW, bumperDark, alloyTank);
    AppendTruckCowlAndWipersX(mesh.get(), xFront, cabHW, windBotY, glossBlack);

    // 4. Rounded White Mid-Roof Cab + Matte-Black Sunvisor + Roof Blade Spoiler
    AppendCarvedRoofAndSleeper(mesh.get(), xFront, cabBackX, doorX1, cabClearY, windBotY, windTopY, cabTopY, cabHW, whitePaint, glossBlack);
    AppendSunvisorBrow(mesh.get(), xFront + 0.30f, xFront + 0.54f, windTopY + 0.02f, windTopY + 0.18f, cabHW, bumperDark, chromeCol, false);

    std::vector<HullStationX> roofPod = {
        {xFront + 0.90f, cabTopY - 0.06f, cabTopY + 0.04f, cabHW - 0.14f, 0.04f, 0.10f, 0.02f, 0.05f},
        {cabBackX,       cabTopY - 0.06f, std::min(H - 0.02f, cabTopY + 0.24f), cabHW - 0.06f, 0.04f, 0.12f, 0.03f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), roofPod, whitePaint, 4);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(doorX0 + 0.06f, cabClearY + 0.72f, s > 0 ? cabHW - 0.01f : -cabHW - 0.012f), Vec3(doorX0 + 0.30f, cabClearY + 0.76f, s > 0 ? cabHW + 0.012f : -cabHW + 0.01f), glossBlack);
    }

    // 5. Stepped Doors aligned to panoramic windshield + Mirrors + Fuel Tanks
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.06f, false, false, whitePaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.06f, false, false, whitePaint, glossBlack, sideGlass, doorBeltY);
    CabComponents::AppendWingMirrors(mesh.get(), xFront + 0.44f, cabTopY - 0.42f, cabHW, bumperDark);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.34f, windTopY - 0.14f, -cabHW, bumperDark);
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.44f, cabTopY - 0.08f, cabHW * 0.85f, def.headlights_on);

    float tankStart = std::max(cabBackX + 0.16f, frontAxleX + archHalfLen + 0.10f);
    float tankEnd   = std::max(tankStart + 0.50f, driveCenterX - 0.68f - archHalfLen - 0.12f);
    ChassisComponents::AppendFuelTank(mesh.get(), tankStart, tankEnd, frameBotY + 0.06f, -(hw - 0.34f), 0.30f, alloyTank);
    ChassisComponents::AppendFuelTank(mesh.get(), tankStart, tankEnd, frameBotY + 0.06f,  (hw - 0.34f), 0.30f, alloyTank);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.12f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.66f, frameTopY + 0.04f, 0.45f), alloyTank);

    // 6. Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), windFrontX, windBrowX, windBotY, windTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.06f, false, true, whitePaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.06f, false, true, whitePaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
