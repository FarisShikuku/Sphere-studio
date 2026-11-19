#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/scania-e-lkw-2026-01.jpg
// Signature features: White low-roof Scania 40 R XT Electric cab with carved corners & low XT
// windshield ratio, contrasting curved gloss-black grille mask w/ silver SCANIA letters + XT badge +
// cyan EV bar, heavy protruding curved matte-charcoal steel XT bumper w/ recessed circular bumper
// fog pods & carved U-DRL corner headlamps, rugged XT mirrors & exposed high-voltage battery rack.
std::shared_ptr<Mesh> GenerateScania40RXtElectricMesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 5.4f, 8.6f);
    const float W = std::clamp(def.dimensions.y, 2.38f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.25f, 3.90f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(def.wheel_radius_m, 0.48f, 0.60f);
    const float tireW = std::clamp(def.tire_width_m, 0.28f, 0.38f);

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float frameBotY = std::max(0.74f, wheelR * 2.0f - 0.12f);
    const float frameTopY = frameBotY + 0.25f;
    const float archTopY = wheelR * 2.0f + 0.14f;
    const float archHalfLen = wheelR + 0.13f;

    const float cabBaseY = frameBotY - 0.04f;
    const float cabClearY = std::max(cabBaseY + 0.24f, archTopY + 0.02f);
    const float cabHW = hw - 0.03f;
    const float cabBackX = xFront + 2.42f;
    const float cabTopY = std::clamp(def.cab_height_m, 3.22f, 3.68f); // Low/Normal roof XT profile

    const float frontAxleX = xFront + 1.42f;
    const float driveCenterX = std::min(xRear - 0.95f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX = std::clamp(def.fifth_wheel_offset_m, 0.85f, xRear - 0.95f);

    const Vec4 whitePaint = def.base_color;
    const Vec4 evBlue = def.accent_color;
    const Vec4 xtCharcoal(0.14f, 0.16f, 0.19f, 1.0f);
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 chromeCol(0.86f, 0.89f, 0.94f, 1.0f);
    const Vec4 plateRed(0.92f, 0.94f, 0.96f, 1.0f);
    const Vec4 alloyCol(0.76f, 0.79f, 0.84f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 interiorSeat(0.18f, 0.20f, 0.24f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.15f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);
    const Vec4 amberCol(0.98f, 0.58f, 0.08f, 1.0f);
    const Vec4 ledWhite = def.headlights_on ? Vec4(0.96f, 0.99f, 1.00f, 1.0f) : Vec4(0.76f, 0.82f, 0.88f, 1.0f);

    AppendTractorChassisBase(
        mesh.get(), def, xFront, xRear, frontAxleX, driveCenterX, fifthWheelX,
        wheelR, tireW, hw, cabHW, frameBotY, frameTopY, archTopY, archHalfLen,
        xtCharcoal, xtCharcoal, linerDark, alloyCol
    );

    const float windFrontX = xFront + 0.14f;
    const float windBrowX  = xFront + 0.34f;
    const float windBotY   = cabClearY + 1.12f;
    const float windTopY   = std::min(cabTopY - 0.42f, windBotY + 0.78f);
    const float doorX0     = xFront + 0.44f;
    const float doorX1     = cabBackX - 0.66f;
    const float doorTopY   = windTopY + 0.05f;
    const float doorBeltY  = windBotY - 0.02f;

    CabComponents::AppendScaniaCabInterior(
        mesh.get(), xFront + 0.32f, cabBackX - 0.08f,
        cabClearY + 0.06f, windBotY, cabTopY - 0.16f,
        cabHW, true, def.is_rhd, def.steer_angle_deg, dashDark, interiorSeat, evBlue
    );

    // 1. Smoothly Carved Lower Cab Nose & Rugged Heavy-Duty Curved XT Steel Bumper
    AppendCarvedLowerNose(mesh.get(), xFront, xFront + 0.48f, cabClearY + 0.02f, windBotY, cabHW, whitePaint);
    AppendCurvedPanelX(mesh.get(), xFront - 0.032f, 0.24f, cabBaseY - 0.10f, cabClearY + 0.18f, cabBaseY - 0.10f, cabClearY + 0.18f, -cabHW, cabHW, cabHW, 0.09f, 8, xtCharcoal, 0.035f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.042f, 0.045f, cabBaseY - 0.08f, cabBaseY + 0.04f, cabBaseY - 0.08f, cabBaseY + 0.04f, -0.46f, 0.46f, cabHW, 0.09f, 4, glossBlack, 0.012f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.048f, 0.022f, cabBaseY + 0.04f, cabBaseY + 0.13f, cabBaseY + 0.04f, cabBaseY + 0.13f, -0.24f, 0.24f, cabHW, 0.09f, 4, plateRed, 0.006f);

    // Full Gloss-Black Center Curved XT Grille Mask + Twin Corner Slanted White Markers + Cyan EV Bar
    AppendCurvedPanelX(mesh.get(), xFront - 0.004f, 0.100f, windBotY - 0.21f, windBotY - 0.03f, windBotY - 0.21f, windBotY - 0.03f, -cabHW * 0.72f, cabHW * 0.72f, cabHW, 0.09f, 6, glossBlack, 0.015f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.024f, windBotY - 0.15f, windBotY - 0.08f, windBotY - 0.15f, windBotY - 0.08f, -0.34f, 0.34f, cabHW, 0.09f, 4, chromeCol, 0.006f);
    AppendSlantedRibbonX(mesh.get(), xFront - 0.012f, 0.016f, windBotY - 0.12f, -cabHW * 0.68f, windBotY - 0.05f, -cabHW * 0.56f, 0.020f, ledWhite, cabHW, 0.09f);
    AppendSlantedRibbonX(mesh.get(), xFront - 0.012f, 0.016f, windBotY - 0.12f,  cabHW * 0.68f, windBotY - 0.05f,  cabHW * 0.56f, 0.020f, ledWhite, cabHW, 0.09f);

    AppendVGrilleSlats(mesh.get(), xFront - 0.005f, cabClearY + 0.44f, windBotY - 0.23f, cabHW * 0.56f, cabHW * 0.66f, 3, xtCharcoal, glossBlack, -1, chromeCol, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.012f, cabClearY + 0.12f, cabClearY + 0.40f, cabHW * 0.52f, cabHW * 0.56f, 2, xtCharcoal, glossBlack, 1, evBlue, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.014f, 0.022f, cabClearY + 0.44f, cabClearY + 0.51f, cabClearY + 0.44f, cabClearY + 0.51f, -cabHW * 0.44f, -cabHW * 0.22f, cabHW, 0.09f, 2, chromeCol, 0.005f);
    float crestX = CurvedNoseXAtZ(cabHW * 0.50f, xFront - 0.010f, cabHW, 0.09f);
    AppendFrontDiscX(mesh.get(), crestX, cabClearY + 0.68f, cabHW * 0.50f, 0.046f, 0.014f, 14, chromeCol, Vec4(0.84f, 0.16f, 0.16f, 1.0f));

    // Carved XT Corner Headlamps + Circular Steel-Bezel Bumper Fog Lamps hugging curved bumper corner!
    for (int side : {-1, 1}) {
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.036f, cabHW, 0.09f,
            cabClearY + 0.03f, cabClearY + 0.20f,
            cabHW * 0.58f, cabHW - 0.08f, side,
            0.026f, 0.012f, glossBlack, ledWhite, ledWhite, amberCol,
            true, false
        );
        float s = static_cast<float>(side);
        float zPod = s * (cabHW * 0.72f);
        float podX = CurvedNoseXAtZ(zPod, xFront - 0.036f, cabHW, 0.09f);
        AppendCircularLampPodX(mesh.get(), podX - 0.006f, cabBaseY + 0.08f, zPod, 0.068f, 0.044f, 16, glossBlack, chromeCol, ledWhite);
    }

    // Lower windshield cowl & wipers + Low/Normal XT Carved Roof & Sleeper
    AppendTruckCowlAndWipersX(mesh.get(), xFront, cabHW, windBotY, glossBlack);
    AppendCarvedRoofAndSleeper(mesh.get(), xFront, cabBackX, doorX1, cabClearY, windBotY, windTopY, cabTopY, cabHW, whitePaint, glossBlack);
    for (float zSpot : {-0.72f, 0.72f}) {
        AppendCurvedPanelX(mesh.get(), xFront + 0.25f, 0.095f, windTopY + 0.08f, windTopY + 0.15f, windTopY + 0.08f, windTopY + 0.15f, zSpot - 0.12f, zSpot + 0.12f, cabHW, 0.09f, 2, ledWhite, 0.006f);
    }
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.42f, cabTopY - 0.06f, cabHW * 0.85f, def.headlights_on);

    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.14f, false, false, whitePaint, glossBlack, sideGlass, doorBeltY, def.is_rhd, true);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.14f, false, false, whitePaint, glossBlack, sideGlass, doorBeltY, !def.is_rhd, true);
    CabComponents::AppendWingMirrors(mesh.get(), xFront + 0.46f, cabTopY - 0.42f, cabHW, xtCharcoal);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.34f, cabTopY - 0.16f, cabHW, xtCharcoal);

    float batStart = cabBackX + 0.24f;
    float batEnd   = std::max(batStart + 0.55f, driveCenterX - archHalfLen - 0.12f);
    RunningGear::AppendBox(mesh.get(), Vec3(batStart, frameBotY - 0.12f, -hw + 0.10f), Vec3(batEnd, frameTopY + 0.06f, -hw + 0.52f), xtCharcoal);
    RunningGear::AppendBox(mesh.get(), Vec3(batStart, frameBotY - 0.12f,  hw - 0.52f), Vec3(batEnd, frameTopY + 0.06f,  hw - 0.10f), xtCharcoal);
    RunningGear::AppendBox(mesh.get(), Vec3(batStart + 0.08f, frameBotY - 0.02f, -hw + 0.08f), Vec3(batEnd - 0.08f, frameBotY + 0.04f, -hw + 0.11f), evBlue);
    RunningGear::AppendBox(mesh.get(), Vec3(batStart + 0.08f, frameBotY - 0.02f,  hw - 0.11f), Vec3(batEnd - 0.08f, frameBotY + 0.04f,  hw - 0.08f), evBlue);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.12f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.66f, frameTopY + 0.04f, 0.45f), alloyCol);

    // Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), windFrontX, windBrowX, windBotY, windTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.14f, false, true, whitePaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.14f, false, true, whitePaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
