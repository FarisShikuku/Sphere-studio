#include "BespokeRigidLorries.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/png-clipart-cargo-truck-transport-commercial-vehicle-truck-freight-transport-truck-thumbnail.png
// Signature features: Classic bright-orange MAN TGX 26.440 6x2 rigid box lorry with carved curved
// cab corners, wide curved gloss-black MAN shield topped by chrome Lion brow bar, curved 3-vent
// bumper & carved crystal headlamps, panoramic windshield aligned to 2-pane split doors, and
// orange roof aero fairing flowing into crisp white dry freight box.
std::shared_ptr<Mesh> GenerateManTgxBoxLorryMesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 8.2f, 11.8f);
    const float W = std::clamp(def.dimensions.y, 2.35f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.25f, 4.10f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(def.wheel_radius_m, 0.45f, 0.56f);
    const float tireW = std::clamp(def.tire_width_m, 0.28f, 0.36f);
    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = hw - tireW * 1.06f - 0.025f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float frameBotY = std::max(0.68f, wheelR * 2.0f - 0.14f);
    const float frameTopY = frameBotY + 0.24f;
    const float archTopY = wheelR * 2.0f + 0.14f;
    const float archHalfLen = wheelR + 0.13f;

    const Vec4 orangePaint = def.base_color;
    const Vec4 boxWhite = def.accent_color;
    const Vec4 glossBlack(0.06f, 0.07f, 0.09f, 1.0f);
    const Vec4 darkTrim(0.11f, 0.12f, 0.14f, 1.0f);
    const Vec4 chromeCol(0.86f, 0.89f, 0.94f, 1.0f);
    const Vec4 alloyCol(0.76f, 0.79f, 0.84f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 interiorSeat(0.18f, 0.20f, 0.24f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.15f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);
    const Vec4 amberCol(0.98f, 0.58f, 0.08f, 1.0f);
    const Vec4 lampCol = def.headlights_on ? Vec4(0.97f, 0.98f, 1.0f, 1.0f) : Vec4(0.76f, 0.82f, 0.88f, 1.0f);

    // 1. Frame Rails & 6x2 Steerable Tag Axles
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.14f, xRear - 0.06f, frameBotY, frameTopY, 0.43f, darkTrim);

    const float frontAxleX = xFront + 1.38f;
    const float rearCenterX = std::min(xRear - 1.35f, frontAxleX + def.wheelbase_m);
    const float driveAxleX = rearCenterX - 0.68f;
    const float tagAxleX   = rearCenterX + 0.68f;

    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, frontHalfTrack, false, def.steer_angle_deg);
    CabComponents::AppendWheelWellLiner(mesh.get(), frontAxleX, archHalfLen, frameBotY - 0.12f, archTopY, hw - 0.02f, hw - tireW - 0.15f, linerDark);
    CabComponents::AppendWheelArchFender(mesh.get(), frontAxleX, wheelR, wheelR + 0.085f, -(hw - 0.02f), false, 0.048f, orangePaint);
    CabComponents::AppendWheelArchFender(mesh.get(), frontAxleX, wheelR, wheelR + 0.085f,  (hw - 0.02f), true,  0.048f, orangePaint);

    ChassisComponents::AppendVehicleAxle(mesh.get(), driveAxleX, wheelR, tireW, rearHalfTrack, true, 0.0f);
    ChassisComponents::AppendVehicleAxle(mesh.get(), tagAxleX,   wheelR, tireW, frontHalfTrack, false, -def.steer_angle_deg * 0.25f);
    for (float rx : {driveAxleX, tagAxleX}) {
        CabComponents::AppendWheelArchFender(mesh.get(), rx, wheelR, wheelR + 0.08f, -hw, false, tireW * 1.15f, darkTrim);
        CabComponents::AppendWheelArchFender(mesh.get(), rx, wheelR, wheelR + 0.08f,  hw, true,  tireW * 1.15f, darkTrim);
    }
    ChassisComponents::AppendDrivetrain(mesh.get(), frontAxleX, driveAxleX, std::max(0.24f, def.ground_clearance_m), wheelR);

    // 2. Classic MAN TGX Sculpted Forward Cab (panoramic windshield ratio)
    const float cabLen = 2.24f;
    const float cabBackX = xFront + cabLen;
    const float cabTopY = std::clamp(def.cab_height_m, 2.95f, 3.40f);
    const float cabBaseY = frameBotY - 0.04f;
    const float cabClearY = std::max(cabBaseY + 0.22f, archTopY + 0.02f);
    const float cabHW = hw - 0.03f;

    const float winFrontX = xFront + 0.14f;
    const float winBrowX  = xFront + 0.34f;
    const float winBotY   = cabClearY + 1.10f;
    const float winTopY   = std::min(cabTopY - 0.40f, winBotY + 0.78f);
    const float doorX0    = xFront + 0.44f;
    const float doorX1    = cabBackX - 0.56f;
    const float doorTopY  = winTopY + 0.05f;
    const float doorBeltY = winBotY - 0.02f;

    CabComponents::AppendTruckCabInterior(
        mesh.get(), xFront + 0.30f, cabBackX - 0.06f,
        cabClearY + 0.05f, cabTopY - 0.14f,
        cabHW, true, dashDark, interiorSeat, amberCol,
        def.is_rhd, winBotY, def.steer_angle_deg
    );

    // Smoothly carved front nose & bumper wrap
    AppendCarvedLowerNose(mesh.get(), xFront, xFront + 0.48f, cabBaseY, winBotY, cabHW, orangePaint);

    // Curved MAN 3-opening bumper mouth + carved corner crystal headlamps + slim fog slots
    float bmpYTop = cabClearY + 0.18f;
    float bmpYBot = cabBaseY - 0.08f;
    AppendTrapezoidGrillePocketX(mesh.get(), xFront - 0.002f, xFront + 0.105f, bmpYBot, bmpYTop, 0.46f, 0.54f, 0.026f, darkTrim, glossBlack, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.008f, 0.035f, (bmpYBot + bmpYTop) * 0.5f - 0.028f, (bmpYBot + bmpYTop) * 0.5f + 0.028f,
                       (bmpYBot + bmpYTop) * 0.5f - 0.028f, (bmpYBot + bmpYTop) * 0.5f + 0.028f, -0.48f, 0.48f, cabHW, 0.09f, 6, orangePaint, 0.008f);

    for (int side : {-1, 1}) {
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.002f, cabHW, 0.09f,
            cabBaseY + 0.06f, bmpYTop - 0.01f,
            cabHW - 0.50f, cabHW - 0.09f, side,
            0.030f, 0.012f, glossBlack, lampCol, lampCol, amberCol,
            true, false
        );
        float s = static_cast<float>(side);
        float zA = s * (cabHW - 0.48f), zB = s * (cabHW - 0.11f);
        AppendCurvedPanelX(mesh.get(), xFront - 0.001f, 0.095f, bmpYBot + 0.01f, cabBaseY + 0.025f, bmpYBot + 0.01f, cabBaseY + 0.025f,
                           std::min(zA, zB), std::max(zA, zB), cabHW, 0.09f, 4, glossBlack, 0.010f);
    }

    // Curved MAN Gloss-Black Upper Shield + Chrome Brow Bar + MAN Crest Bar
    float shYBot = bmpYTop + 0.12f;
    float shYTop = winBotY - 0.10f;
    AppendTrapezoidGrillePocketX(mesh.get(), xFront - 0.004f, xFront + 0.105f, shYBot, shYTop, cabHW * 0.48f, cabHW * 0.62f, 0.032f, glossBlack, glossBlack, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.030f, shYTop - 0.095f, shYTop - 0.015f, shYTop - 0.095f, shYTop - 0.015f, -cabHW * 0.57f, cabHW * 0.57f, cabHW, 0.09f, 6, chromeCol, 0.008f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.014f, 0.028f, (shYBot + shYTop) * 0.5f - 0.042f, (shYBot + shYTop) * 0.5f + 0.042f,
                       (shYBot + shYTop) * 0.5f - 0.042f, (shYBot + shYTop) * 0.5f + 0.042f, -0.25f, 0.25f, cabHW, 0.09f, 4, chromeCol, 0.008f);

    // Lower windshield cowl & wipers + carved roof/pillars/sleeper + aero deflector
    AppendTruckCowlAndWipersX(mesh.get(), xFront, cabHW, winBotY, glossBlack);
    AppendCarvedRoofAndSleeper(mesh.get(), xFront, cabBackX, doorX1, cabClearY, winBotY, winTopY, cabTopY, cabHW, orangePaint, glossBlack);

    std::vector<HullStationX> roofAero = {
        {xFront + 0.85f, cabTopY - 0.06f, cabTopY + 0.02f, cabHW - 0.12f, 0.04f, 0.12f, 0.03f, 0.05f},
        {cabBackX,       cabTopY - 0.06f, H - 0.03f,       cabHW - 0.03f, 0.04f, 0.14f, 0.04f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), roofAero, orangePaint, 4);
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.42f, cabTopY - 0.08f, cabHW * 0.85f, def.headlights_on);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        for (int l = 0; l < 4; ++l) {
            float ly = cabClearY + 1.05f + static_cast<float>(l) * 0.12f;
            RunningGear::AppendBox(mesh.get(), Vec3(doorX1 + 0.06f, ly, s > 0 ? cabHW - 0.01f : -cabHW - 0.012f), Vec3(cabBackX - 0.08f, ly + 0.045f, s > 0 ? cabHW + 0.012f : -cabHW + 0.01f), glossBlack);
        }
    }

    AppendCabEntrySteps(mesh.get(), xFront + 0.44f, frontAxleX - archHalfLen + 0.04f, frameBotY, cabClearY, cabHW, darkTrim, alloyCol);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.05f, true, false, orangePaint, glossBlack, sideGlass, doorBeltY, def.is_rhd, true);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.05f, true, false, orangePaint, glossBlack, sideGlass, doorBeltY, !def.is_rhd, true);
    CabComponents::AppendCantileverCoachMirrors(mesh.get(), xFront + 0.46f, cabTopY - 0.30f, cabHW, darkTrim);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.34f, cabTopY - 0.18f, cabHW, glossBlack);

    // 3. Tall Crisp White Insulated Rigid Box Body + Alloy Battery Box + Dark Side Rail Guard
    const float bodyFrontX = cabBackX + 0.14f;
    const float bodyRearX  = xRear;
    const float deckY = frameTopY + 0.04f;
    std::vector<HullStationX> boxStations = {
        {bodyFrontX, deckY, H - 0.02f, hw, 0.04f, 0.08f, 0.02f, 0.02f},
        {bodyRearX,  deckY, H - 0.02f, hw, 0.04f, 0.08f, 0.02f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), boxStations, boxWhite, 3);

    RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX - 0.01f, deckY, -hw - 0.01f), Vec3(bodyFrontX + 0.08f, H - 0.02f, -hw + 0.08f), alloyCol);
    RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX - 0.01f, deckY,  hw - 0.08f), Vec3(bodyFrontX + 0.08f, H - 0.02f,  hw + 0.01f), alloyCol);
    for (int d = 0; d < 6; ++d) {
        float dx0 = bodyFrontX + 0.30f + static_cast<float>(d) * 0.92f;
        if (dx0 + 0.55f < bodyRearX) {
            RunningGear::AppendBox(mesh.get(), Vec3(dx0, deckY + 0.04f, -hw - 0.012f), Vec3(dx0 + 0.55f, deckY + 0.10f, -hw + 0.01f), amberCol);
            RunningGear::AppendBox(mesh.get(), Vec3(dx0, deckY + 0.04f,  hw - 0.01f), Vec3(dx0 + 0.55f, deckY + 0.10f,  hw + 0.012f), amberCol);
        }
    }

    float guardX0 = frontAxleX + archHalfLen + 0.08f;
    float guardX1 = std::max(guardX0 + 0.45f, driveAxleX - archHalfLen - 0.10f);
    RunningGear::AppendBox(mesh.get(), Vec3(guardX0, frameBotY - 0.16f, -hw + 0.05f), Vec3(guardX0 + 0.42f, deckY, -hw + 0.25f), alloyCol);
    RunningGear::AppendBox(mesh.get(), Vec3(guardX0, frameBotY - 0.16f,  hw - 0.25f), Vec3(guardX0 + 0.42f, deckY,  hw - 0.05f), alloyCol);
    RunningGear::AppendBox(mesh.get(), Vec3(guardX0 + 0.44f, frameBotY - 0.14f, -hw + 0.04f), Vec3(guardX1, frameTopY, -hw + 0.10f), darkTrim);
    RunningGear::AppendBox(mesh.get(), Vec3(guardX0 + 0.44f, frameBotY - 0.14f,  hw - 0.10f), Vec3(guardX1, frameTopY,  hw - 0.04f), darkTrim);

    float shutterLift = std::clamp(def.action_ratio, 0.0f, 1.0f) * (H - deckY - 0.28f);
    RunningGear::AppendBox(mesh.get(), Vec3(bodyRearX - 0.01f, deckY + 0.06f + shutterLift, -hw + 0.12f), Vec3(bodyRearX + 0.025f, H - 0.10f, hw - 0.12f), alloyCol);
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.14f, 0.44f, -hw + 0.08f), Vec3(xRear + 0.04f, 0.56f, hw - 0.08f), chromeCol);
    RunningGear::AppendBox(mesh.get(), Vec3(xRear + 0.02f, 0.46f, -hw + 0.10f), Vec3(xRear + 0.06f, 0.55f, -hw + 0.36f), Vec4(0.90f, 0.10f, 0.12f, 1.0f));
    RunningGear::AppendBox(mesh.get(), Vec3(xRear + 0.02f, 0.46f,  hw - 0.36f), Vec3(xRear + 0.06f, 0.55f,  hw - 0.10f), Vec4(0.90f, 0.10f, 0.12f, 1.0f));

    // 4. Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), winFrontX, winBrowX, winBotY, winTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.05f, true, true, orangePaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.05f, true, true, orangePaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
