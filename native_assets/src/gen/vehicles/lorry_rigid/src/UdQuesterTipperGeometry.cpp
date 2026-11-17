#include "BespokeRigidLorries.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/Quester-3-Config-26-Euro5 - Copy.png
// Signature features: Deep teal Quester 8x4 mining tipper, carved curved front bumper & fenders,
// tall curved hexagonal charcoal grille split by twin brushed-chrome wing bars, chrome U-brow
// cradling circular UD badge, dark curved 3-piece steel bumper with carved L-chrome headlamp pods,
// panoramic rigid-cab windshield ratio aligned with stepped door glass, charcoal rock dump body.
std::shared_ptr<Mesh> GenerateUdQuesterTipperMesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 7.8f, 11.2f);
    const float W = std::clamp(def.dimensions.y, 2.35f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.05f, 3.90f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(def.wheel_radius_m, 0.48f, 0.62f);
    const float tireW = std::clamp(def.tire_width_m, 0.28f, 0.38f);
    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = hw - tireW * 1.06f - 0.025f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float frameBotY = std::max(0.76f, wheelR * 2.0f - 0.12f);
    const float frameTopY = frameBotY + 0.26f;
    const float archTopY = wheelR * 2.0f + 0.14f;
    const float archHalfLen = wheelR + 0.13f;

    const Vec4 tealPaint = def.base_color;
    const Vec4 steelCharcoal(0.18f, 0.20f, 0.23f, 1.0f);
    const Vec4 steelBlack(0.09f, 0.10f, 0.12f, 1.0f);
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 bumperDark = def.accent_color;
    const Vec4 chromeCol(0.88f, 0.91f, 0.96f, 1.0f);
    const Vec4 alloyCol(0.74f, 0.77f, 0.82f, 1.0f);
    const Vec4 silverStripe(0.86f, 0.88f, 0.91f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 interiorSeat(0.18f, 0.20f, 0.24f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.15f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);
    const Vec4 headlampWhite = def.headlights_on ? Vec4(0.96f, 0.98f, 1.00f, 1.0f) : Vec4(0.76f, 0.82f, 0.88f, 1.0f);
    const Vec4 amberCol(0.96f, 0.58f, 0.08f, 1.0f);

    // 1. Heavy-duty ladder frame + 8x4 Dual-Steer Front + Tandem Rear Axles
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.16f, xRear - 0.06f, frameBotY, frameTopY, 0.43f, steelBlack);

    const float frontAxle1X = xFront + 1.38f;
    const float frontAxle2X = frontAxle1X + 1.46f;
    const float rearCenterX = std::min(xRear - 1.36f, frontAxle1X + def.wheelbase_m);

    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxle1X, wheelR, tireW, frontHalfTrack, false, def.steer_angle_deg, false);
    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxle2X, wheelR, tireW, frontHalfTrack, false, def.steer_angle_deg * 0.72f, false);
    CabComponents::AppendWheelWellLiner(mesh.get(), frontAxle1X, archHalfLen, frameBotY - 0.10f, archTopY, hw - 0.03f, hw - tireW - 0.16f, linerDark);
    CabComponents::AppendWheelArchFender(mesh.get(), frontAxle1X, wheelR, wheelR + 0.095f, -(hw - 0.02f), false, 0.055f, steelBlack);
    CabComponents::AppendWheelArchFender(mesh.get(), frontAxle1X, wheelR, wheelR + 0.095f,  (hw - 0.02f), true,  0.055f, steelBlack);
    CabComponents::AppendWheelArchFender(mesh.get(), frontAxle2X, wheelR, wheelR + 0.095f, -(hw - 0.02f), false, 0.055f, steelBlack);
    CabComponents::AppendWheelArchFender(mesh.get(), frontAxle2X, wheelR, wheelR + 0.095f,  (hw - 0.02f), true,  0.055f, steelBlack);

    float rearAxleA = rearCenterX - 0.68f;
    float rearAxleB = rearCenterX + 0.68f;
    ChassisComponents::AppendVehicleAxle(mesh.get(), rearAxleA, wheelR, tireW, rearHalfTrack, true, 0.0f, false);
    ChassisComponents::AppendVehicleAxle(mesh.get(), rearAxleB, wheelR, tireW, rearHalfTrack, true, 0.0f, false);

    RunningGear::AppendBox(mesh.get(), Vec3(rearAxleB + archHalfLen - 0.04f, frameBotY - 0.22f, -hw), Vec3(rearAxleB + archHalfLen + 0.02f, archTopY, -hw + tireW * 2.15f), steelBlack);
    RunningGear::AppendBox(mesh.get(), Vec3(rearAxleB + archHalfLen - 0.04f, frameBotY - 0.22f,  hw - tireW * 2.15f), Vec3(rearAxleB + archHalfLen + 0.02f, archTopY,  hw), steelBlack);
    ChassisComponents::AppendDrivetrain(mesh.get(), frontAxle1X, rearCenterX, std::max(0.28f, def.ground_clearance_m), wheelR);

    // 2. UD Trucks Quester Cab Shell & Interior (wide panoramic windshield ratio)
    const float cabLen = 2.18f;
    const float cabBackX = xFront + cabLen;
    const float cabTopY = std::clamp(def.cab_height_m, 2.75f, 3.25f);
    const float cabBaseY = frameBotY - 0.04f;
    const float cabClearY = std::max(cabBaseY + 0.24f, archTopY + 0.02f);
    const float cabHW = hw - 0.04f;

    const float winFrontX = xFront + 0.14f;
    const float winBrowX  = xFront + 0.34f;
    const float winBotY   = cabClearY + 0.98f;
    const float winTopY   = std::min(cabTopY - 0.36f, winBotY + 0.76f);
    const float doorX0    = xFront + 0.44f;
    const float doorX1    = cabBackX - 0.24f;
    const float doorTopY  = winTopY + 0.05f;
    const float doorBeltY = winBotY - 0.02f;

    CabComponents::AppendTruckCabInterior(
        mesh.get(), xFront + 0.30f, cabBackX - 0.06f,
        cabClearY + 0.05f, cabTopY - 0.14f,
        cabHW, false, dashDark, interiorSeat, chromeCol
    );

    // Smoothly carved lower nose & rounded front corners
    AppendCarvedLowerNose(mesh.get(), xFront, xFront + 0.46f, cabClearY + 0.02f, winBotY, cabHW, tealPaint);

    // Rugged matte-black 3-piece curved construction steel bumper wrapping seamlessly across corners
    AppendCurvedPanelX(mesh.get(), xFront - 0.015f, 0.22f, cabBaseY - 0.14f, cabClearY + 0.05f, cabBaseY - 0.14f, cabClearY + 0.05f, -cabHW, cabHW, cabHW, 0.09f, 8, bumperDark, 0.035f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.024f, 0.055f, cabBaseY - 0.11f, cabBaseY + 0.01f, cabBaseY - 0.11f, cabBaseY + 0.01f, -0.50f, 0.50f, cabHW, 0.09f, 4, steelBlack, 0.015f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.030f, 0.025f, cabBaseY - 0.05f, cabBaseY + 0.045f, cabBaseY - 0.05f, cabBaseY + 0.045f, -0.25f, 0.25f, cabHW, 0.09f, 4, silverStripe, 0.008f);

    // Carved corner vertical headlamp clusters with chrome L-bezel
    for (int side : {-1, 1}) {
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.022f, cabHW, 0.09f,
            cabBaseY - 0.06f, cabClearY + 0.04f,
            cabHW - 0.40f, cabHW - 0.07f, side,
            0.05f, 0.02f, glossBlack, headlampWhite, chromeCol, amberCol,
            true, false
        );
        float s = static_cast<float>(side);
        float zA = s * (cabHW - 0.42f);
        float zB = s * (cabHW - 0.38f);
        AppendCurvedPanelX(mesh.get(), xFront - 0.028f, 0.022f, cabBaseY - 0.07f, cabClearY + 0.05f, cabBaseY - 0.07f, cabClearY + 0.05f, std::min(zA, zB), std::max(zA, zB), cabHW, 0.09f, 2, chromeCol, 0.006f);
    }

    // Towering Curved UD Quester Hexagonal Grille + Twin Brushed-Chrome Wing Bars + Circular UD Badge
    float grYBot = cabBaseY + 0.07f;
    float grYTop = winBotY - 0.14f;
    float grHWBot = cabHW * 0.54f;
    float grHWTop = cabHW * 0.62f;
    AppendTrapezoidGrillePocketX(mesh.get(), xFront - 0.004f, xFront + 0.095f, grYBot, grYTop, grHWBot, grHWTop, 0.042f, glossBlack, glossBlack, cabHW, 0.09f);
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zCheekIn  = s * (grHWTop + 0.01f);
        float zCheekOut = s * (grHWTop + 0.07f);
        AppendCurvedPanelX(mesh.get(), xFront - 0.005f, 0.075f, cabClearY + 0.06f, winBotY - 0.10f, cabClearY + 0.06f, winBotY - 0.10f, std::min(zCheekIn, zCheekOut), std::max(zCheekIn, zCheekOut), cabHW, 0.09f, 2, alloyCol, 0.015f);
    }
    float yUpperWing = cabClearY + 0.46f;
    float yMidWing   = cabClearY + 0.25f;
    float yBumperBar = cabClearY + 0.03f;
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.032f, yUpperWing - 0.042f, yUpperWing + 0.042f, yUpperWing - 0.042f, yUpperWing + 0.042f, -grHWTop + 0.02f, grHWTop - 0.02f, cabHW, 0.09f, 6, chromeCol, 0.008f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.032f, yMidWing - 0.040f,   yMidWing + 0.040f,   yMidWing - 0.040f,   yMidWing + 0.040f,   -grHWTop + 0.03f, grHWTop - 0.03f, cabHW, 0.09f, 6, chromeCol, 0.008f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.024f, 0.032f, yBumperBar - 0.028f, yBumperBar + 0.028f, yBumperBar - 0.028f, yBumperBar + 0.028f, -grHWBot + 0.02f, grHWBot - 0.02f, cabHW, 0.09f, 6, chromeCol, 0.008f);

    float badgeY = winBotY - 0.15f;
    AppendCurvedPanelX(mesh.get(), xFront - 0.008f, 0.036f, badgeY - 0.036f, badgeY + 0.022f, badgeY - 0.036f, badgeY + 0.022f, -0.34f, 0.34f, cabHW, 0.09f, 4, chromeCol, 0.008f);
    AppendFrontDiscX(mesh.get(), xFront - 0.014f, badgeY, 0.0f, 0.112f, 0.024f, 22, chromeCol, chromeCol);
    AppendFrontDiscX(mesh.get(), xFront - 0.024f, badgeY, 0.0f, 0.086f, 0.014f, 22, glossBlack, chromeCol);
    AppendFrontDiscX(mesh.get(), xFront - 0.030f, badgeY, 0.0f, 0.052f, 0.010f, 18, chromeCol, tealPaint);

    // Lower windshield cowl seal & wipers + carved roof & sleek A-pillars
    AppendTruckCowlAndWipersX(mesh.get(), xFront, cabHW, winBotY, glossBlack);
    AppendCarvedRoofAndSleeper(mesh.get(), xFront, cabBackX, doorX1, cabClearY, winBotY, winTopY, cabTopY, cabHW, tealPaint, glossBlack);
    AppendSunvisorBrow(mesh.get(), xFront + 0.32f, xFront + 0.56f, winTopY + 0.02f, winTopY + 0.18f, cabHW, glossBlack, chromeCol, false);

    // Side corner trim deflectors, ESCOT stripe, & rear cab cooling louvers
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.24f, cabClearY + 0.16f, s > 0 ? cabHW - 0.02f : -cabHW - 0.014f), Vec3(xFront + 0.38f, winBotY - 0.05f, s > 0 ? cabHW + 0.014f : -cabHW + 0.02f), glossBlack);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.26f, cabClearY + 0.20f, s > 0 ? cabHW - 0.01f : -cabHW - 0.020f), Vec3(xFront + 0.32f, winBotY - 0.09f, s > 0 ? cabHW + 0.020f : -cabHW + 0.01f), silverStripe);
        AppendSideRibbonZ(mesh.get(), cabHW * s, 0.014f, side > 0, xFront + 0.42f, cabClearY + 0.58f, cabBackX - 0.04f, cabClearY + 0.74f, 0.042f, silverStripe);
        for (int v = 0; v < 3; ++v) {
            float vy = cabClearY + 1.02f + static_cast<float>(v) * 0.13f;
            RunningGear::AppendBox(mesh.get(), Vec3(doorX1 + 0.04f, vy, s > 0 ? cabHW - 0.01f : -cabHW - 0.014f), Vec3(cabBackX - 0.06f, vy + 0.055f, s > 0 ? cabHW + 0.014f : -cabHW + 0.01f), glossBlack);
        }
    }

    AppendCabEntrySteps(mesh.get(), xFront + 0.46f, frontAxle1X - archHalfLen + 0.04f, frameBotY, cabClearY, cabHW, bumperDark, alloyCol);
    ChassisComponents::AppendClosedCylinderY(mesh.get(), cabBackX + 0.10f, -cabHW + 0.16f, frameTopY, cabTopY + 0.18f, 0.075f, 14, chromeCol);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.04f, frameTopY, cabHW - 0.24f), Vec3(cabBackX + 0.20f, cabTopY + 0.06f, cabHW - 0.06f), steelBlack);

    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.08f, false, false, tealPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.08f, false, false, tealPaint, glossBlack, sideGlass, doorBeltY);
    CabComponents::AppendWingMirrors(mesh.get(), xFront + 0.46f, cabTopY - 0.44f, cabHW, tealPaint);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.34f, cabTopY - 0.20f, cabHW, glossBlack);
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.44f, cabTopY - 0.08f, cabHW * 0.85f, def.headlights_on);

    // 3. Heavy Charcoal Quarry Steel Dump Body
    const float bodyFrontX = cabBackX + 0.24f;
    const float bodyRearX  = xRear - 0.04f;
    const float deckY = frameTopY + 0.08f;
    const float pivotX = rearAxleB + 0.35f;
    const float pivotY = frameTopY + 0.08f;
    const float tilt = -std::clamp(def.action_ratio, 0.0f, 1.0f) * 0.62f;

    AppendTiltedBoxZ(mesh.get(), Vec3(cabBackX - 0.56f, H - 0.14f, -hw), Vec3(bodyFrontX + 0.08f, H + 0.02f, hw), pivotX, pivotY, tilt, steelCharcoal);
    AppendTiltedBoxZ(mesh.get(), Vec3(bodyFrontX, deckY, -hw), Vec3(bodyRearX, deckY + 0.16f, hw), pivotX, pivotY, tilt, steelBlack);
    AppendTiltedBoxZ(mesh.get(), Vec3(bodyFrontX, deckY + 0.14f, -hw), Vec3(bodyFrontX + 0.14f, H - 0.12f, hw), pivotX, pivotY, tilt, steelCharcoal);
    AppendTiltedBoxZ(mesh.get(), Vec3(bodyFrontX, deckY + 0.14f, -hw), Vec3(bodyRearX, H - 0.12f, -hw + 0.12f), pivotX, pivotY, tilt, steelCharcoal);
    AppendTiltedBoxZ(mesh.get(), Vec3(bodyFrontX, deckY + 0.14f,  hw - 0.12f), Vec3(bodyRearX, H - 0.12f,  hw), pivotX, pivotY, tilt, steelCharcoal);
    AppendTiltedBoxZ(mesh.get(), Vec3(bodyRearX - 0.12f, deckY + 0.14f, -hw + 0.04f), Vec3(bodyRearX, H - 0.14f, hw - 0.04f), pivotX, pivotY, tilt, steelCharcoal);
    AppendTiltedBoxZ(mesh.get(), Vec3(bodyFrontX, deckY + 0.55f, -hw - 0.05f), Vec3(bodyRearX, deckY + 0.66f, -hw), pivotX, pivotY, tilt, steelBlack);
    AppendTiltedBoxZ(mesh.get(), Vec3(bodyFrontX, deckY + 0.55f,  hw), Vec3(bodyRearX, deckY + 0.66f,  hw + 0.05f), pivotX, pivotY, tilt, steelBlack);
    AppendTiltedBoxZ(mesh.get(), Vec3(bodyRearX - 0.22f, deckY + 0.14f, -hw - 0.05f), Vec3(bodyRearX - 0.06f, H - 0.12f, -hw), pivotX, pivotY, tilt, steelCharcoal);
    AppendTiltedBoxZ(mesh.get(), Vec3(bodyRearX - 0.22f, deckY + 0.14f,  hw), Vec3(bodyRearX - 0.06f, H - 0.12f,  hw + 0.05f), pivotX, pivotY, tilt, steelCharcoal);

    float ramTopY = deckY + std::clamp(def.action_ratio, 0.0f, 1.0f) * 2.05f + 0.50f;
    ChassisComponents::AppendClosedCylinderY(mesh.get(), bodyFrontX + 0.24f, 0.0f, frameBotY + 0.10f, ramTopY, 0.115f, 14, chromeCol);

    float midStart = frontAxle2X + archHalfLen + 0.10f;
    float midEnd   = std::max(midStart + 0.45f, rearAxleA - archHalfLen - 0.12f);
    ChassisComponents::AppendFuelTank(mesh.get(), midStart, midEnd, frameBotY + 0.04f, -(hw - 0.36f), 0.28f, alloyCol);
    RunningGear::AppendBox(mesh.get(), Vec3(midStart, frameBotY + 0.18f, hw - 0.08f), Vec3(midEnd, frameBotY + 0.25f, hw - 0.02f), alloyCol);
    RunningGear::AppendBox(mesh.get(), Vec3(midStart, frameBotY - 0.04f, hw - 0.08f), Vec3(midEnd, frameBotY + 0.03f, hw - 0.02f), alloyCol);
    ChassisComponents::AppendBatteryAndDefBox(mesh.get(), midStart, std::min(midEnd, midStart + 0.60f), frameBotY - 0.10f, frameTopY, hw - 0.42f, true);

    // 4. Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), winFrontX, winBrowX, winBotY, winTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.08f, false, true, tealPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.08f, false, true, tealPaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
