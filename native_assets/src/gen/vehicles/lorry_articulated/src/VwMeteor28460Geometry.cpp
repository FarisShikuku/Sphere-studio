#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/images (17).jpeg
// Signature features: Metallic-silver VW Meteor 28.460 6x4 heavyweight sleeper cab with carved
// rounded corners, panoramic windshield aligned with split 2-pane doors, curved silver sunvisor
// with METEOR brow strip & whip antenna, curved 3-slat brushed-chrome VW grille w/ large round
// VW medallion, carved twin corner LED headlamps & fog pods, 7-slot rear louvers & alloy tanks.
std::shared_ptr<Mesh> GenerateVwMeteor28460Mesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 6.0f, 9.0f);
    const float W = std::clamp(def.dimensions.y, 2.38f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.30f, 4.05f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(def.wheel_radius_m, 0.45f, 0.58f);
    const float tireW = std::clamp(def.tire_width_m, 0.28f, 0.38f);

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float frameBotY = std::max(0.68f, wheelR * 2.0f - 0.14f);
    const float frameTopY = frameBotY + 0.25f;
    const float archTopY = wheelR * 2.0f + 0.14f;
    const float archHalfLen = wheelR + 0.13f;

    const float cabBaseY = frameBotY - 0.04f;
    const float cabClearY = std::max(cabBaseY + 0.22f, archTopY + 0.02f);
    const float cabHW = hw - 0.03f;
    const float cabBackX = xFront + 2.48f;
    const float cabTopY = std::clamp(def.cab_height_m, 3.40f, 3.96f);

    const float frontAxleX = xFront + 1.42f;
    const float driveCenterX = std::min(xRear - 1.10f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX = std::clamp(def.fifth_wheel_offset_m, 1.15f, xRear - 0.95f);

    const Vec4 silverPaint = def.base_color;
    const Vec4 obsidianTrim = def.accent_color;
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
    const Vec4 ledWhite = def.headlights_on ? Vec4(0.96f, 0.99f, 1.00f, 1.0f) : chromeCol;

    // 1. Chassis & 6x4 axles
    AppendTractorChassisBase(
        mesh.get(), def, xFront, xRear, frontAxleX, driveCenterX, fifthWheelX,
        wheelR, tireW, hw, cabHW, frameBotY, frameTopY, archTopY, archHalfLen,
        frameDark, obsidianTrim, linerDark, alloyTank
    );

    const float windFrontX = xFront + 0.14f;
    const float windBrowX  = xFront + 0.34f;
    const float windBotY   = cabClearY + 1.20f;
    const float windTopY   = std::min(cabTopY - 0.50f, windBotY + 0.84f);
    const float doorX0     = xFront + 0.44f;
    const float doorX1     = cabBackX - 0.68f;
    const float doorTopY   = windTopY + 0.05f;
    const float doorBeltY  = windBotY - 0.02f;

    // 2. Interior
    CabComponents::AppendTruckCabInterior(
        mesh.get(), xFront + 0.32f, cabBackX - 0.08f,
        cabClearY + 0.06f, cabTopY - 0.16f, cabHW, true, dashDark, interiorSeat, chromeCol,
        def.is_rhd, windBotY, def.steer_angle_deg
    );

    // 3. Smoothly Carved Lower Cab Nose & Corners
    AppendCarvedLowerNose(mesh.get(), xFront, xFront + 0.48f, cabBaseY, windBotY, cabHW, silverPaint);

    // Wide Curved 3-Slat Brushed-Chrome Upper VW Meteor Grille + Large Round VW Medallion + METEOR Tag
    float upGrBot = cabClearY + 0.50f;
    float upGrTop = windBotY - 0.10f;
    AppendTrapezoidGrillePocketX(mesh.get(), xFront - 0.004f, xFront + 0.105f, upGrBot, upGrTop, cabHW * 0.64f, cabHW * 0.78f, 0.024f, glossBlack, glossBlack, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.005f, upGrBot + 0.03f, upGrTop - 0.03f, cabHW * 0.61f, cabHW * 0.74f, 3, chromeCol, glossBlack, -1, chromeCol, cabHW, 0.09f);
    float vwY = (upGrBot + upGrTop) * 0.5f;
    AppendFrontDiscX(mesh.get(), xFront - 0.016f, vwY, 0.0f, 0.140f, 0.025f, 24, glossBlack, chromeCol);
    AppendFrontDiscX(mesh.get(), xFront - 0.024f, vwY, 0.0f, 0.122f, 0.014f, 24, chromeCol, chromeCol);
    AppendFrontDiscX(mesh.get(), xFront - 0.030f, vwY, 0.0f, 0.098f, 0.010f, 24, glossBlack, chromeCol);
    AppendSlantedRibbonX(mesh.get(), xFront - 0.034f, 0.012f, vwY + 0.07f, -0.055f, vwY - 0.06f, 0.0f, 0.014f, chromeCol, cabHW, 0.09f);
    AppendSlantedRibbonX(mesh.get(), xFront - 0.034f, 0.012f, vwY + 0.07f,  0.055f, vwY - 0.06f, 0.0f, 0.014f, chromeCol, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.002f, 0.025f, upGrTop + 0.012f, upGrTop + 0.045f, upGrTop + 0.012f, upGrTop + 0.045f, cabHW * 0.24f, cabHW * 0.52f, cabHW, 0.09f, 2, chromeCol, 0.004f);

    // Wide Curved Heavy-Duty Black Trapezoidal Lower Bumper Mouth (3 dark-silver bars)
    float lowGrBot = cabBaseY - 0.06f;
    float lowGrTop = cabClearY + 0.42f;
    AppendTrapezoidGrillePocketX(mesh.get(), xFront - 0.004f, xFront + 0.105f, lowGrBot, lowGrTop, cabHW * 0.44f, cabHW * 0.50f, 0.026f, obsidianTrim, glossBlack, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.005f, lowGrBot + 0.04f, lowGrTop - 0.03f, cabHW * 0.41f, cabHW * 0.47f, 3, alloyTank, glossBlack, -1, chromeCol, cabHW, 0.09f);

    // Carved Silver Bumper Cheeks Housing Twin Stacked Curved LED Headlamps & Fog Pods
    for (int side : {-1, 1}) {
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.002f, cabHW, 0.09f,
            cabClearY + 0.10f, cabClearY + 0.30f,
            cabHW * 0.54f, cabHW - 0.08f, side,
            0.035f, 0.012f, glossBlack, ledWhite, ledWhite, amberCol,
            true, false
        );
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.002f, cabHW, 0.09f,
            cabBaseY - 0.03f, cabBaseY + 0.08f,
            cabHW * 0.56f, cabHW - 0.10f, side,
            0.016f, 0.010f, glossBlack, ledWhite, ledWhite, amberCol,
            false, false
        );
    }

    // Lower windshield cowl & wipers
    AppendTruckCowlAndWipersX(mesh.get(), xFront, cabHW, windBotY, glossBlack);

    // 4. Metallic-Silver Roof + Carved Sleeper + Silver Sunvisor + Whip Antenna + Cooling Louvers
    AppendCarvedRoofAndSleeper(mesh.get(), xFront, cabBackX, doorX1, cabClearY, windBotY, windTopY, cabTopY, cabHW, silverPaint, glossBlack);
    AppendSunvisorBrow(mesh.get(), xFront + 0.30f, xFront + 0.54f, windTopY + 0.02f, windTopY + 0.18f, cabHW, silverPaint, glossBlack, true);
    ChassisComponents::AppendClosedCylinderY(mesh.get(), xFront + 0.72f, -0.36f, cabTopY - 0.02f, cabTopY + 0.48f, 0.010f, 8, glossBlack);
    RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.62f, cabTopY - 0.05f, -0.45f), Vec3(xFront + 0.96f, cabTopY + 0.04f, 0.45f), glossBlack);

    std::vector<HullStationX> roofPod = {
        {xFront + 0.90f, cabTopY - 0.08f, cabTopY + 0.02f, cabHW - 0.12f, 0.04f, 0.12f, 0.03f, 0.06f},
        {cabBackX,       cabTopY - 0.08f, std::min(H - 0.02f, cabTopY + 0.18f), cabHW - 0.04f, 0.04f, 0.14f, 0.04f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), roofPod, silverPaint, 4);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.04f, cabBaseY + 0.34f, s > 0 ? cabHW - 0.05f : -cabHW), Vec3(cabBackX + 0.24f, cabTopY - 0.12f, s > 0 ? cabHW : -cabHW + 0.05f), silverPaint);
        for (int lv = 0; lv < 7; ++lv) {
            float ly = cabClearY + 0.18f + static_cast<float>(lv) * 0.16f;
            RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.16f, ly, s > 0 ? cabHW - 0.01f : -cabHW - 0.014f), Vec3(cabBackX - 0.04f, ly + 0.08f, s > 0 ? cabHW + 0.014f : -cabHW + 0.01f), glossBlack);
        }
        RunningGear::AppendBox(mesh.get(), Vec3(frontAxleX + archHalfLen - 0.04f, archTopY + 0.04f, s > 0 ? cabHW - 0.01f : -cabHW - 0.014f), Vec3(frontAxleX + archHalfLen + 0.04f, archTopY + 0.10f, s > 0 ? cabHW + 0.014f : -cabHW + 0.01f), amberCol);
    }

    // 5. Split 2-Pane Doors aligned to panoramic windshield + Mirrors + 6x4 Alloy Tank & DEF Box
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.06f, true, false, silverPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.06f, true, false, silverPaint, glossBlack, sideGlass, doorBeltY);
    CabComponents::AppendCantileverCoachMirrors(mesh.get(), xFront + 0.46f, cabTopY - 0.34f, cabHW, obsidianTrim);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.34f, windTopY - 0.14f, -cabHW, obsidianTrim);
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.46f, cabTopY - 0.08f, cabHW * 0.85f, def.headlights_on);

    float tankStart = std::max(cabBackX + 0.16f, frontAxleX + archHalfLen + 0.10f);
    float tankEnd   = std::max(tankStart + 0.50f, driveCenterX - 0.68f - archHalfLen - 0.10f);
    ChassisComponents::AppendFuelTank(mesh.get(), tankStart, tankEnd, frameBotY + 0.06f, -(hw - 0.34f), 0.31f, alloyTank);
    ChassisComponents::AppendFuelTank(mesh.get(), tankStart, tankEnd, frameBotY + 0.06f,  (hw - 0.34f), 0.31f, alloyTank);
    ChassisComponents::AppendBatteryAndDefBox(mesh.get(), tankStart, std::min(tankEnd, tankStart + 0.65f), frameBotY - 0.12f, frameTopY, hw - 0.42f, true);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.12f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.66f, frameTopY + 0.04f, 0.45f), alloyTank);

    // 6. Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), windFrontX, windBrowX, windBotY, windTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.06f, true, true, silverPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.06f, true, true, silverPaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
