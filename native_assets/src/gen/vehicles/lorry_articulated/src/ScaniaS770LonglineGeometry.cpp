#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/images (14).jpeg
// Signature features: Extended Longline XXL crimson flat-floor S 770 6x4 cab (+0.78m carved sleeper),
// taller flat-floor S-series panoramic windshield ratio aligned with kerb-dipped door glass, curved
// 4-bar upper + 3-bar lower matte-anthracite V-grille w/ gold SCANIA emblem, carved dual-deck LED
// corner headlamps & lower fog blades, curved crimson sunvisor + corner roof LEDs & lounge window.
std::shared_ptr<Mesh> GenerateScaniaS770LonglineMesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 6.4f, 9.5f);
    const float W = std::clamp(def.dimensions.y, 2.40f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.45f, 4.15f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(def.wheel_radius_m, 0.46f, 0.58f);
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
    const float cabBackX = xFront + 3.25f; // +0.78m LONGLINE XL SLEEPER EXTENSION
    const float cabTopY = std::clamp(def.cab_height_m, 3.50f, 4.08f);

    const float frontAxleX = xFront + 1.44f;
    const float driveCenterX = std::min(xRear - 1.15f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX = std::clamp(def.fifth_wheel_offset_m, 1.35f, xRear - 0.95f);

    const Vec4 crimsonPaint = def.base_color;
    const Vec4 darkRedGraphic(0.42f, 0.06f, 0.09f, 1.0f);
    const Vec4 obsidianTrim = def.accent_color;
    const Vec4 frameDark(0.12f, 0.14f, 0.17f, 1.0f);
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 chromeCol(0.86f, 0.89f, 0.94f, 1.0f);
    const Vec4 goldBadge(0.84f, 0.72f, 0.44f, 1.0f);
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
        frameDark, crimsonPaint, linerDark, alloyCol
    );

    const float windFrontX = xFront + 0.14f;
    const float windBrowX  = xFront + 0.34f;
    const float windBotY   = cabClearY + 1.26f; // Higher flat-floor S-series windshield sill
    const float windTopY   = std::min(cabTopY - 0.54f, windBotY + 0.86f);
    const float doorX0     = xFront + 0.44f;
    const float doorX1     = xFront + 1.74f;
    const float doorTopY   = windTopY + 0.05f;
    const float doorBeltY  = windBotY - 0.02f;

    CabComponents::AppendScaniaCabInterior(
        mesh.get(), xFront + 0.32f, cabBackX - 0.08f,
        cabClearY + 0.06f, windBotY, cabTopY - 0.16f,
        cabHW, true, def.is_rhd, def.steer_angle_deg, dashDark, interiorSeat, crimsonPaint
    );

    // Smoothly Carved Lower Cab Nose & Rounded Corners
    AppendCarvedLowerNose(mesh.get(), xFront, xFront + 0.48f, cabBaseY, windBotY, cabHW, crimsonPaint);

    // Tall Flat-Floor S770 Curved Anthracite 4-Slat Upper + 3-Slat Lower V-Grille + Gold SCANIA Emblem
    AppendCurvedPanelX(mesh.get(), xFront - 0.002f, 0.100f, windBotY - 0.21f, windBotY - 0.03f, windBotY - 0.21f, windBotY - 0.03f, -cabHW * 0.72f, cabHW * 0.72f, cabHW, 0.09f, 6, obsidianTrim, 0.015f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.024f, windBotY - 0.15f, windBotY - 0.08f, windBotY - 0.15f, windBotY - 0.08f, -0.36f, 0.36f, cabHW, 0.09f, 4, goldBadge, 0.006f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.004f, cabClearY + 0.46f, windBotY - 0.23f, cabHW * 0.56f, cabHW * 0.68f, 4, obsidianTrim, glossBlack, -1, goldBadge, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.004f, cabBaseY + 0.04f,  cabClearY + 0.40f, cabHW * 0.50f, cabHW * 0.56f, 3, obsidianTrim, glossBlack, -1, goldBadge, cabHW, 0.09f);
    float crestX = CurvedNoseXAtZ(cabHW * 0.50f, xFront - 0.010f, cabHW, 0.09f);
    AppendFrontDiscX(mesh.get(), crestX, cabClearY + 0.68f, cabHW * 0.50f, 0.046f, 0.014f, 14, chromeCol, obsidianTrim);
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.026f, cabBaseY - 0.07f, cabBaseY + 0.01f, cabBaseY - 0.07f, cabBaseY + 0.01f, -0.26f, 0.26f, cabHW, 0.09f, 4, chromeCol, 0.006f);

    // Carved Glowing S-Series Dual-Deck LED Corner Headlamps + Slim Fog Blades
    for (int side : {-1, 1}) {
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.002f, cabHW, 0.09f,
            cabClearY + 0.08f, cabClearY + 0.28f,
            cabHW * 0.58f, cabHW - 0.08f, side,
            0.032f, 0.015f, glossBlack, ledWhite, ledWhite, amberCol,
            true, false
        );
        float s = static_cast<float>(side);
        float zA = s * (cabHW * 0.60f), zB = s * (cabHW - 0.10f);
        AppendCurvedPanelX(mesh.get(), xFront - 0.002f, 0.075f, cabBaseY + 0.01f, cabBaseY + 0.075f, cabBaseY + 0.01f, cabBaseY + 0.075f,
                           std::min(zA, zB), std::max(zA, zB), cabHW, 0.09f, 3, ledWhite, 0.008f);
    }

    // Lower windshield cowl & wipers + carved Longline XXL sleeper roof, A-pillars & rounded rear walls
    AppendTruckCowlAndWipersX(mesh.get(), xFront, cabHW, windBotY, glossBlack);
    AppendCarvedRoofAndSleeper(mesh.get(), xFront, cabBackX, doorX1, cabClearY, windBotY, windTopY, cabTopY, cabHW, crimsonPaint, glossBlack);
    AppendSunvisorBrow(mesh.get(), xFront + 0.30f, xFront + 0.54f, windTopY + 0.02f, windTopY + 0.19f, cabHW, crimsonPaint, glossBlack, false);

    for (float zSpot : {-0.78f, 0.78f}) {
        AppendCurvedPanelX(mesh.get(), xFront + 0.25f, 0.095f, windTopY + 0.15f, windTopY + 0.24f, windTopY + 0.15f, windTopY + 0.24f, zSpot - 0.12f, zSpot + 0.12f, cabHW, 0.09f, 2, ledWhite, 0.008f);
    }
    ChassisComponents::AppendClosedCylinderY(mesh.get(), xFront + 0.68f, -0.78f, cabTopY, cabTopY + 0.42f, 0.012f, 8, glossBlack);
    ChassisComponents::AppendClosedCylinderY(mesh.get(), xFront + 0.68f,  0.78f, cabTopY, cabTopY + 0.42f, 0.012f, 8, glossBlack);

    // Swept rear lounge window aligned cleanly with panoramic door glass height + dark-crimson graphic ribbons
    float winSx0 = doorX1 + 0.20f;
    float winSx1 = cabBackX - 0.46f;
    float winSy0 = doorBeltY + 0.08f;
    float winSy1 = windTopY - 0.04f;
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(winSx0 - 0.03f, winSy0 - 0.03f, s > 0 ? cabHW - 0.01f : -cabHW - 0.012f), Vec3(winSx1 + 0.03f, winSy1 + 0.03f, s > 0 ? cabHW + 0.012f : -cabHW + 0.01f), glossBlack);
        AppendSideRibbonZ(mesh.get(), cabHW * s, 0.014f, side > 0, doorX1 + 0.14f, cabClearY + 0.56f, cabBackX - 0.04f, cabClearY + 0.88f, 0.075f, darkRedGraphic);
        RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.04f, cabBaseY + 0.36f, s > 0 ? cabHW - 0.05f : -cabHW), Vec3(cabBackX + 0.22f, cabTopY - 0.10f, s > 0 ? cabHW : -cabHW + 0.05f), crimsonPaint);
    }

    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.15f, false, false, crimsonPaint, glossBlack, sideGlass, doorBeltY, def.is_rhd, true);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.15f, false, false, crimsonPaint, glossBlack, sideGlass, doorBeltY, !def.is_rhd, true);
    CabComponents::AppendCantileverCoachMirrors(mesh.get(), xFront + 0.46f, cabTopY - 0.34f, cabHW, glossBlack);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.34f, cabTopY - 0.18f, cabHW, glossBlack);

    float skirtStart = cabBackX + 0.18f;
    float skirtEnd   = std::max(skirtStart + 0.50f, driveCenterX - 0.68f - archHalfLen - 0.10f);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f, -hw + 0.03f), Vec3(skirtEnd, frameTopY + 0.02f, -hw + 0.10f), crimsonPaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f,  hw - 0.10f), Vec3(skirtEnd, frameTopY + 0.02f,  hw - 0.03f), crimsonPaint);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.12f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.66f, frameTopY + 0.04f, 0.45f), alloyCol);

    // Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), windFrontX, windBrowX, windBotY, windTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.15f, false, true, crimsonPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.15f, false, true, crimsonPaint, glossBlack, sideGlass, doorBeltY);
    RunningGear::AppendBox(mesh.get(), Vec3(winSx0, winSy0, -cabHW - 0.010f), Vec3(winSx1, winSy1, -cabHW + 0.004f), sideGlass);
    RunningGear::AppendBox(mesh.get(), Vec3(winSx0, winSy0,  cabHW - 0.004f), Vec3(winSx1, winSy1,  cabHW + 0.010f), sideGlass);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
