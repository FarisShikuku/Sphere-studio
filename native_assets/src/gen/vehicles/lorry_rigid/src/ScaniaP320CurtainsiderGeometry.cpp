#include "BespokeRigidLorries.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/images (20).jpeg
// Signature features: Clean glacier-white Scania P 320 6x2 Curtainsider with carved cab corners,
// curved 2-bar wide charcoal horizontal grilles + SCANIA silver top collar bar, carved corner LED
// headlamps with amber outer blocks & curved lower bumper DRLs, curved black sunvisor with twin
// corner lamps, panoramic windshield matching kerb-stepped Scania door glass, white side skirt w/
// vertical orange-red accent bar & 14 curtain tensioner buckles.
std::shared_ptr<Mesh> GenerateScaniaP320CurtainsiderMesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 8.2f, 11.8f);
    const float W = std::clamp(def.dimensions.y, 2.35f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.20f, 4.05f);
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

    const Vec4 cabWhite = def.base_color;
    const Vec4 orangeStripe = def.accent_color;
    const Vec4 glossBlack(0.06f, 0.07f, 0.09f, 1.0f);
    const Vec4 grilleCharcoal(0.16f, 0.18f, 0.21f, 1.0f);
    const Vec4 curtainOffWhite(0.90f, 0.91f, 0.93f, 1.0f);
    const Vec4 greySkirt(0.84f, 0.85f, 0.88f, 1.0f);
    const Vec4 chromeCol(0.86f, 0.89f, 0.94f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 interiorSeat(0.18f, 0.20f, 0.24f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.15f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);
    const Vec4 amberCol(0.98f, 0.58f, 0.08f, 1.0f);
    const Vec4 drlWhite = def.headlights_on ? Vec4(0.96f, 0.99f, 1.0f, 1.0f) : Vec4(0.76f, 0.82f, 0.88f, 1.0f);

    // 1. Frame Rails & 6x2 Steer-Tag Chassis
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.14f, xRear - 0.06f, frameBotY, frameTopY, 0.43f, grilleCharcoal);

    const float frontAxleX = xFront + 1.38f;
    const float rearCenterX = std::min(xRear - 1.35f, frontAxleX + def.wheelbase_m);
    const float driveAxleX = rearCenterX - 0.68f;
    const float tagAxleX   = rearCenterX + 0.68f;

    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, frontHalfTrack, false, def.steer_angle_deg);
    CabComponents::AppendWheelWellLiner(mesh.get(), frontAxleX, archHalfLen, frameBotY - 0.12f, archTopY, hw - 0.02f, hw - tireW - 0.15f, linerDark);
    CabComponents::AppendWheelArchFender(mesh.get(), frontAxleX, wheelR, wheelR + 0.085f, -(hw - 0.02f), false, 0.042f, glossBlack);
    CabComponents::AppendWheelArchFender(mesh.get(), frontAxleX, wheelR, wheelR + 0.085f,  (hw - 0.02f), true,  0.042f, glossBlack);

    ChassisComponents::AppendVehicleAxle(mesh.get(), driveAxleX, wheelR, tireW, rearHalfTrack, true, 0.0f);
    ChassisComponents::AppendVehicleAxle(mesh.get(), tagAxleX,   wheelR, tireW, frontHalfTrack, false, -def.steer_angle_deg * 0.25f);
    for (float rx : {driveAxleX, tagAxleX}) {
        CabComponents::AppendWheelArchFender(mesh.get(), rx, wheelR, wheelR + 0.08f, -hw, false, tireW * 1.15f, glossBlack);
        CabComponents::AppendWheelArchFender(mesh.get(), rx, wheelR, wheelR + 0.08f,  hw, true,  tireW * 1.15f, glossBlack);
    }
    ChassisComponents::AppendDrivetrain(mesh.get(), frontAxleX, driveAxleX, std::max(0.24f, def.ground_clearance_m), wheelR);

    // 2. Low-Mount Scania Next-Gen P/G Forward Distribution Cab (panoramic windshield ratio)
    const float cabLen = 2.16f;
    const float cabBackX = xFront + cabLen;
    const float cabTopY = std::clamp(def.cab_height_m, 2.85f, 3.32f);
    const float cabBaseY = frameBotY - 0.04f;
    const float cabClearY = std::max(cabBaseY + 0.22f, archTopY + 0.02f);
    const float cabHW = hw - 0.03f;

    const float winFrontX = xFront + 0.14f;
    const float winBrowX  = xFront + 0.34f;
    const float winBotY   = cabClearY + 1.02f;
    const float winTopY   = std::min(cabTopY - 0.38f, winBotY + 0.76f);
    const float doorX0    = xFront + 0.42f;
    const float doorX1    = cabBackX - 0.34f;
    const float doorTopY  = winTopY + 0.05f;
    const float doorBeltY = winBotY - 0.02f;

    CabComponents::AppendTruckCabInterior(
        mesh.get(), xFront + 0.28f, cabBackX - 0.06f,
        cabClearY + 0.05f, cabTopY - 0.14f,
        cabHW, false, dashDark, interiorSeat, chromeCol
    );

    // Smoothly carved lower nose & rounded front corners
    AppendCarvedLowerNose(mesh.get(), xFront, xFront + 0.46f, cabBaseY, winBotY, cabHW, cabWhite);

    // Curved Scania Next-Gen P320 horizontal 2-bar upper & 2-bar lower charcoal grilles + SCANIA collar
    AppendVGrilleSlats(mesh.get(), xFront - 0.004f, cabClearY + 0.48f, winBotY - 0.20f, cabHW * 0.60f, cabHW * 0.68f, 2, grilleCharcoal, glossBlack, -1, chromeCol, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.004f, cabBaseY + 0.02f,  cabClearY + 0.40f, cabHW * 0.54f, cabHW * 0.60f, 2, grilleCharcoal, glossBlack, -1, chromeCol, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.002f, 0.095f, winBotY - 0.17f, winBotY - 0.03f, winBotY - 0.17f, winBotY - 0.03f, -cabHW * 0.70f, cabHW * 0.70f, cabHW, 0.09f, 6, glossBlack, 0.014f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.024f, winBotY - 0.13f, winBotY - 0.06f, winBotY - 0.13f, winBotY - 0.06f, -0.34f, 0.34f, cabHW, 0.09f, 4, chromeCol, 0.006f);
    float crestX = CurvedNoseXAtZ(cabHW * 0.50f, xFront - 0.010f, cabHW, 0.09f);
    AppendFrontDiscX(mesh.get(), crestX, cabClearY + 0.68f, cabHW * 0.50f, 0.045f, 0.014f, 14, chromeCol, Vec4(0.84f, 0.16f, 0.16f, 1.0f));

    // Carved corner Scania LED headlamps + curved lower bumper LED DRL / fog bars
    for (int side : {-1, 1}) {
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.002f, cabHW, 0.09f,
            cabClearY + 0.06f, cabClearY + 0.28f,
            cabHW * 0.62f, cabHW - 0.08f, side,
            0.032f, 0.015f, glossBlack, drlWhite, drlWhite, amberCol,
            true, false
        );
        float s = static_cast<float>(side);
        float zA = s * (cabHW * 0.63f), zB = s * (cabHW - 0.10f);
        AppendCurvedPanelX(mesh.get(), xFront - 0.002f, 0.080f, cabBaseY + 0.02f, cabBaseY + 0.085f, cabBaseY + 0.02f, cabBaseY + 0.085f,
                           std::min(zA, zB), std::max(zA, zB), cabHW, 0.09f, 3, drlWhite, 0.008f);
    }

    // Lower windshield cowl & wipers + carved roof & sleek A-pillars + curved sunvisor & brow spotlamps
    AppendTruckCowlAndWipersX(mesh.get(), xFront, cabHW, winBotY, glossBlack);
    AppendCarvedRoofAndSleeper(mesh.get(), xFront, cabBackX, doorX1, cabClearY, winBotY, winTopY, cabTopY, cabHW, cabWhite, glossBlack);
    AppendSunvisorBrow(mesh.get(), xFront + 0.30f, xFront + 0.54f, winTopY + 0.02f, winTopY + 0.18f, cabHW, glossBlack, drlWhite, false);
    for (float zSpot : {-0.84f, 0.84f}) {
        AppendCurvedPanelX(mesh.get(), xFront + 0.25f, 0.095f, winTopY + 0.21f, winTopY + 0.28f, winTopY + 0.21f, winTopY + 0.28f,
                           zSpot - 0.11f, zSpot + 0.11f, cabHW, 0.09f, 2, drlWhite, 0.008f);
    }

    std::vector<HullStationX> roofAero = {
        {xFront + 0.78f, cabTopY - 0.06f, cabTopY + 0.02f, cabHW - 0.14f, 0.04f, 0.12f, 0.03f, 0.05f},
        {cabBackX,       cabTopY - 0.06f, H - 0.04f,       cabHW - 0.04f, 0.04f, 0.14f, 0.04f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), roofAero, cabWhite, 4);

    // Stepped Scania doors aligned with windshield beltline & top
    AppendCabEntrySteps(mesh.get(), xFront + 0.42f, frontAxleX - archHalfLen + 0.04f, frameBotY, cabClearY, cabHW, glossBlack, chromeCol);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.14f, false, false, cabWhite, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.14f, false, false, cabWhite, glossBlack, sideGlass, doorBeltY);
    CabComponents::AppendWingMirrors(mesh.get(), xFront + 0.44f, cabTopY - 0.42f, cabHW, glossBlack);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.34f, cabTopY - 0.18f, cabHW, glossBlack);

    // 3. White Rigid Curtainsider Body with 14 Tension Buckle Straps & Signature Orange Front Skirt Pillar
    const float bodyFrontX = cabBackX + 0.16f;
    const float bodyRearX  = xRear;
    const float deckY = frameTopY + 0.04f;
    float curtainRetract = std::clamp(def.action_ratio, 0.0f, 1.0f) * ((bodyRearX - bodyFrontX) * 0.42f);

    RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX, deckY, -hw), Vec3(bodyRearX, deckY + 0.12f, hw), greySkirt);
    RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX, deckY + 0.12f, -hw), Vec3(bodyFrontX + 0.12f, H - 0.02f, hw), curtainOffWhite);
    RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX, H - 0.14f, -hw), Vec3(bodyRearX, H - 0.02f, hw), curtainOffWhite);
    RunningGear::AppendBox(mesh.get(), Vec3(bodyRearX - 0.10f, deckY + 0.12f, -hw), Vec3(bodyRearX, H - 0.02f, -hw + 0.12f), greySkirt);
    RunningGear::AppendBox(mesh.get(), Vec3(bodyRearX - 0.10f, deckY + 0.12f,  hw - 0.12f), Vec3(bodyRearX, H - 0.02f,  hw), greySkirt);
    RunningGear::AppendBox(mesh.get(), Vec3(bodyRearX - 0.02f, deckY + 0.10f, -hw + 0.12f), Vec3(bodyRearX + 0.02f, H - 0.08f, hw - 0.12f), greySkirt);

    float cEnd = std::max(bodyFrontX + 0.60f, bodyRearX - curtainRetract);
    RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX + 0.10f, deckY + 0.12f, -hw), Vec3(cEnd, H - 0.14f, -hw + 0.05f), curtainOffWhite);
    RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX + 0.10f, deckY + 0.12f,  hw - 0.05f), Vec3(cEnd, H - 0.14f,  hw), curtainOffWhite);

    int strapCount = 14;
    for (int i = 0; i < strapCount; ++i) {
        float t = (static_cast<float>(i) + 0.5f) / static_cast<float>(strapCount);
        float sx = bodyFrontX + 0.25f + t * (cEnd - bodyFrontX - 0.45f);
        RunningGear::AppendBox(mesh.get(), Vec3(sx - 0.022f, deckY + 0.06f, -hw - 0.014f), Vec3(sx + 0.022f, deckY + 0.32f, -hw), grilleCharcoal);
        RunningGear::AppendBox(mesh.get(), Vec3(sx - 0.022f, deckY + 0.06f,  hw), Vec3(sx + 0.022f, deckY + 0.32f,  hw + 0.014f), grilleCharcoal);
    }

    float skirtX0 = cabBackX + 0.18f;
    float skirtX1 = driveAxleX - archHalfLen - 0.14f;
    if (skirtX1 > skirtX0 + 0.40f) {
        RunningGear::AppendBox(mesh.get(), Vec3(skirtX0, frameBotY - 0.22f, -hw + 0.02f), Vec3(skirtX1, deckY, -hw + 0.09f), greySkirt);
        RunningGear::AppendBox(mesh.get(), Vec3(skirtX0, frameBotY - 0.22f,  hw - 0.09f), Vec3(skirtX1, deckY,  hw - 0.02f), greySkirt);
        RunningGear::AppendBox(mesh.get(), Vec3(skirtX0 + 0.02f, frameBotY - 0.22f, -hw + 0.01f), Vec3(skirtX0 + 0.13f, deckY + 0.01f, -hw + 0.095f), orangeStripe);
        RunningGear::AppendBox(mesh.get(), Vec3(skirtX0 + 0.02f, frameBotY - 0.22f,  hw - 0.095f), Vec3(skirtX0 + 0.13f, deckY + 0.01f,  hw - 0.01f), orangeStripe);
    }

    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.14f, 0.44f, -hw + 0.08f), Vec3(xRear + 0.04f, 0.56f, hw - 0.08f), greySkirt);
    RunningGear::AppendBox(mesh.get(), Vec3(xRear + 0.02f, 0.46f, -hw + 0.10f), Vec3(xRear + 0.06f, 0.55f, -hw + 0.36f), Vec4(0.90f, 0.10f, 0.12f, 1.0f));
    RunningGear::AppendBox(mesh.get(), Vec3(xRear + 0.02f, 0.46f,  hw - 0.36f), Vec3(xRear + 0.06f, 0.55f,  hw - 0.10f), Vec4(0.90f, 0.10f, 0.12f, 1.0f));

    // 4. Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), winFrontX, winBrowX, winBotY, winTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.14f, false, true, cabWhite, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.14f, false, true, cabWhite, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
