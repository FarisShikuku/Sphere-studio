#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/2019-ford-f-max-semi-truck-europe-tractor.jpg & images (22).jpeg
// Signature features: Carved highline F-MAX Aero sleeper cab, panoramic windshield & sculpted
// roof crown with black LED corner marker brow, two-tier curved trapezoidal Ford grille w/ silver
// F-MAX header & centered Blue Oval medallion, carved vertical corner headlamp towers with white
// C-frame DRLs, MirrorCam wings, split 2-pane doors, upward-sloping dark side swage & full skirts.
std::shared_ptr<Mesh> GenerateFordFMax500Mesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 5.4f, 8.6f);
    const float W = std::clamp(def.dimensions.y, 2.38f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.30f, 4.10f);
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
    const float cabClearY = std::max(cabBaseY + 0.24f, archTopY + 0.02f);
    const float cabHW = hw - 0.03f;
    const float cabBackX = xFront + 2.50f;
    const float cabTopY = std::clamp(def.cab_height_m, 3.45f, 4.02f);

    const float frontAxleX = xFront + 1.42f;
    const float driveCenterX = std::min(xRear - 0.95f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX = std::clamp(def.fifth_wheel_offset_m, 0.85f, xRear - 0.95f);

    const Vec4 cabPaint = def.base_color;
    const Vec4 fordBlueOval = def.accent_color;
    const Vec4 frameDark(0.13f, 0.15f, 0.18f, 1.0f);
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 grilleCharcoal(0.10f, 0.11f, 0.13f, 1.0f);
    const Vec4 chromeCol(0.88f, 0.91f, 0.96f, 1.0f);
    const Vec4 alloyCol(0.78f, 0.81f, 0.86f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 interiorSeat(0.18f, 0.20f, 0.24f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.15f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);
    const Vec4 ledWhite = def.headlights_on ? Vec4(0.96f, 0.99f, 1.00f, 1.0f) : Vec4(0.76f, 0.82f, 0.88f, 1.0f);
    const Vec4 amberCol(0.96f, 0.62f, 0.08f, 1.0f);

    // 1. Chassis & axles
    AppendTractorChassisBase(
        mesh.get(), def, xFront, xRear, frontAxleX, driveCenterX, fifthWheelX,
        wheelR, tireW, hw, cabHW, frameBotY, frameTopY, archTopY, archHalfLen,
        frameDark, cabPaint, linerDark, alloyCol
    );

    const float windFrontX = xFront + 0.14f;
    const float windBrowX  = xFront + 0.34f;
    const float windBotY   = cabClearY + 1.24f;
    const float windTopY   = std::min(cabTopY - 0.54f, windBotY + 0.86f);
    const float doorX0     = xFront + 0.44f;
    const float doorX1     = cabBackX - 0.68f;
    const float doorTopY   = windTopY + 0.05f;
    const float doorBeltY  = windBotY - 0.02f;

    // 2. Interior
    CabComponents::AppendTruckCabInterior(
        mesh.get(), xFront + 0.32f, cabBackX - 0.08f,
        cabClearY + 0.06f, cabTopY - 0.16f, cabHW, true, dashDark, interiorSeat, fordBlueOval
    );

    // 3. Smoothly Carved Curved Lower Nose & Corners
    AppendCarvedLowerNose(mesh.get(), xFront, xFront + 0.48f, cabBaseY, windBotY, cabHW, cabPaint);

    // Curved horizontal bumper seam & silver F-MAX badge plate above upper grille
    AppendCurvedPanelX(mesh.get(), xFront - 0.002f, 0.032f, cabClearY + 0.37f, cabClearY + 0.41f, cabClearY + 0.37f, cabClearY + 0.41f, -(cabHW - 0.12f), cabHW - 0.12f, cabHW, 0.09f, 6, glossBlack, 0.006f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.008f, 0.028f, windBotY - 0.13f, windBotY - 0.08f, windBotY - 0.13f, windBotY - 0.08f, -0.24f, 0.24f, cabHW, 0.09f, 4, chromeCol, 0.006f);

    // Two-Tier Curved Trapezoidal F-MAX Grille + Multi-Slat Louvers + Ford Blue Oval
    float upGrBot = cabClearY + 0.45f;
    float upGrTop = windBotY - 0.17f;
    AppendTrapezoidGrillePocketX(mesh.get(), xFront - 0.004f, xFront + 0.105f, upGrBot, upGrTop, cabHW * 0.58f, cabHW * 0.54f, 0.028f, grilleCharcoal, glossBlack, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.004f, upGrBot + 0.03f, upGrTop - 0.03f, cabHW * 0.55f, cabHW * 0.52f, 4, grilleCharcoal, glossBlack, -1, chromeCol, cabHW, 0.09f);

    float lowGrBot = cabBaseY - 0.02f;
    float lowGrTop = cabClearY + 0.35f;
    AppendTrapezoidGrillePocketX(mesh.get(), xFront - 0.004f, xFront + 0.105f, lowGrBot, lowGrTop, cabHW * 0.50f, cabHW * 0.58f, 0.028f, grilleCharcoal, glossBlack, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.004f, lowGrBot + 0.03f, lowGrTop - 0.02f, cabHW * 0.47f, cabHW * 0.55f, 3, grilleCharcoal, glossBlack, -1, chromeCol, cabHW, 0.09f);

    float ovalY = (upGrBot + upGrTop) * 0.5f;
    AppendCurvedPanelX(mesh.get(), xFront - 0.016f, 0.026f, ovalY - 0.072f, ovalY + 0.072f, ovalY - 0.072f, ovalY + 0.072f, -0.175f, 0.175f, cabHW, 0.09f, 4, chromeCol, 0.022f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.022f, 0.018f, ovalY - 0.056f, ovalY + 0.056f, ovalY - 0.056f, ovalY + 0.056f, -0.155f, 0.155f, cabHW, 0.09f, 4, fordBlueOval, 0.018f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.028f, cabBaseY - 0.09f, cabBaseY - 0.02f, cabBaseY - 0.09f, cabBaseY - 0.02f, -0.22f, 0.22f, cabHW, 0.09f, 4, Vec4(0.95f, 0.96f, 0.98f, 1.0f), 0.006f);
    AppendCurvedPanelX(mesh.get(), xFront + 0.004f, 0.095f, cabBaseY - 0.14f, cabBaseY - 0.09f, cabBaseY - 0.14f, cabBaseY - 0.09f, -(cabHW - 0.16f), cabHW - 0.16f, cabHW, 0.09f, 6, frameDark, 0.012f);

    // Carved Corner Vertical Headlamp Towers hugging curved bumper corners
    for (int side : {-1, 1}) {
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.002f, cabHW, 0.09f,
            cabBaseY + 0.01f, cabClearY + 0.36f,
            cabHW * 0.62f, cabHW - 0.08f, side,
            0.06f, 0.03f, glossBlack, ledWhite, ledWhite, amberCol,
            true, false
        );
    }

    // Lower windshield cowl & wipers
    AppendTruckCowlAndWipersX(mesh.get(), xFront, cabHW, windBotY, glossBlack);

    // 4. Highline F-MAX Roof, Curved Black Sunvisor, Upper Roof Brow Insert with Corner LEDs
    AppendCarvedRoofAndSleeper(mesh.get(), xFront, cabBackX, doorX1, cabClearY, windBotY, windTopY, cabTopY, cabHW, cabPaint, glossBlack);
    AppendSunvisorBrow(mesh.get(), xFront + 0.30f, xFront + 0.54f, windTopY + 0.02f, windTopY + 0.18f, cabHW, glossBlack, chromeCol, false);

    // Curved iconic F-MAX upper roof brow black insert strip + corner white LED marker lamps
    AppendCurvedPanelX(mesh.get(), xFront + 0.31f, 0.11f, cabTopY - 0.24f, cabTopY - 0.16f, cabTopY - 0.24f, cabTopY - 0.16f, -(cabHW - 0.18f), cabHW - 0.18f, cabHW, 0.09f, 6, glossBlack, 0.010f);
    for (float zSide : {-1.0f, 1.0f}) {
        float zM0 = zSide > 0 ? (cabHW - 0.34f) : -(cabHW - 0.20f);
        float zM1 = zSide > 0 ? (cabHW - 0.20f) : -(cabHW - 0.34f);
        AppendCurvedPanelX(mesh.get(), xFront + 0.302f, 0.04f, cabTopY - 0.23f, cabTopY - 0.17f, cabTopY - 0.23f, cabTopY - 0.17f, zM0, zM1, cabHW, 0.09f, 2, ledWhite, 0.005f);
    }
    ChassisComponents::AppendClosedCylinderY(mesh.get(), xFront + 0.72f, -0.58f, cabTopY - 0.04f, cabTopY + 0.26f, 0.010f, 8, glossBlack);
    ChassisComponents::AppendClosedCylinderY(mesh.get(), xFront + 0.72f,  0.58f, cabTopY - 0.04f, cabTopY + 0.26f, 0.010f, 8, glossBlack);

    std::vector<HullStationX> roofPod = {
        {xFront + 0.92f, cabTopY - 0.08f, cabTopY + 0.02f, cabHW - 0.12f, 0.04f, 0.12f, 0.03f, 0.06f},
        {cabBackX,       cabTopY - 0.08f, std::min(H - 0.02f, cabTopY + 0.24f), cabHW - 0.04f, 0.04f, 0.16f, 0.05f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), roofPod, cabPaint, 4);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.04f, cabBaseY + 0.34f, s > 0 ? cabHW - 0.05f : -cabHW), Vec3(cabBackX + 0.26f, cabTopY - 0.10f, s > 0 ? cabHW : -cabHW + 0.05f), cabPaint);
        AppendSideRibbonZ(mesh.get(), cabHW * s, 0.015f, side > 0, xFront + 0.46f, cabClearY + 0.68f, doorX1 + 0.06f, cabClearY + 0.46f, 0.024f, glossBlack);
        AppendSideRibbonZ(mesh.get(), cabHW * s, 0.015f, side > 0, doorX1 + 0.06f, cabClearY + 0.46f, cabBackX - 0.02f, cabClearY + 0.56f, 0.024f, glossBlack);
        for (int v = 0; v < 3; ++v) {
            float vy = cabClearY + 0.68f + static_cast<float>(v) * 0.065f;
            RunningGear::AppendBox(mesh.get(), Vec3(doorX1 + 0.16f, vy, s > 0 ? cabHW - 0.01f : -cabHW - 0.012f), Vec3(cabBackX - 0.10f, vy + 0.030f, s > 0 ? cabHW + 0.012f : -cabHW + 0.01f), glossBlack);
        }
    }

    // 5. Split 2-Pane F-MAX Doors aligned to panoramic windshield + MirrorCam Wings + Side Skirts
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.06f, true, false, cabPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.06f, true, false, cabPaint, glossBlack, sideGlass, doorBeltY);
    AppendMirrorCamWings(mesh.get(), xFront + 0.46f, windTopY - 0.14f, cabHW, glossBlack, cabPaint, chromeCol);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.34f, windTopY - 0.14f, -cabHW, glossBlack);

    float skirtStart = cabBackX + 0.22f;
    float skirtEnd   = std::max(skirtStart + 0.55f, driveCenterX - archHalfLen - 0.10f);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f, -hw + 0.03f), Vec3(skirtEnd, frameTopY + 0.02f, -hw + 0.10f), cabPaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f,  hw - 0.10f), Vec3(skirtEnd, frameTopY + 0.02f,  hw - 0.03f), cabPaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.16f, frameBotY - 0.16f, -hw + 0.02f), Vec3(skirtStart + 0.40f, frameBotY - 0.02f, -hw + 0.04f), glossBlack);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.16f, frameBotY - 0.16f,  hw - 0.04f), Vec3(skirtStart + 0.40f, frameBotY - 0.02f,  hw - 0.02f), glossBlack);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.12f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.66f, frameTopY + 0.04f, 0.45f), alloyCol);

    // 6. Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), windFrontX, windBrowX, windBotY, windTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.06f, true, true, cabPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.06f, true, true, cabPaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
