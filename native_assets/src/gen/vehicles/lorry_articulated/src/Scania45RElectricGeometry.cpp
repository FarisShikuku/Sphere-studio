#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/Electric-Scania-Truck.jpg.jpeg
// Signature features: Glacier-white Scania Next-Gen R BEV Highline tractor with carved corners,
// panoramic windshield ratio aligned with kerb-dipped doors, curved gloss-black V-grille mask with
// SCANIA bar + Griffin shield, electric-blue EV trim bar, carved horizontal corner LED headlamps w/
// lower white DRL & amber corners, twin corner roof brow spotlamps, & flush white battery skirts.
std::shared_ptr<Mesh> GenerateScania45RElectricMesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 5.4f, 8.6f);
    const float W = std::clamp(def.dimensions.y, 2.38f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.25f, 4.05f);
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
    const float cabTopY = std::clamp(def.cab_height_m, 3.35f, 3.95f);

    const float frontAxleX = xFront + 1.42f;
    const float driveCenterX = std::min(xRear - 0.95f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX = std::clamp(def.fifth_wheel_offset_m, 0.85f, xRear - 0.95f);

    const Vec4 whitePaint = def.base_color;
    const Vec4 evBlue = def.accent_color;
    const Vec4 frameDark(0.13f, 0.15f, 0.18f, 1.0f);
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 chromeCol(0.86f, 0.89f, 0.94f, 1.0f);
    const Vec4 alloyCol(0.78f, 0.81f, 0.86f, 1.0f);
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
        frameDark, glossBlack, linerDark, alloyCol
    );

    const float windFrontX = xFront + 0.14f;
    const float windBrowX  = xFront + 0.34f;
    const float windBotY   = cabClearY + 1.20f;
    const float windTopY   = std::min(cabTopY - 0.50f, windBotY + 0.84f);
    const float doorX0     = xFront + 0.44f;
    const float doorX1     = cabBackX - 0.70f;
    const float doorTopY   = windTopY + 0.05f;
    const float doorBeltY  = windBotY - 0.02f;

    CabComponents::AppendTruckCabInterior(
        mesh.get(), xFront + 0.32f, cabBackX - 0.08f,
        cabClearY + 0.06f, cabTopY - 0.16f, cabHW, true, dashDark, interiorSeat, evBlue
    );

    // Smoothly Carved Lower Cab Nose & Rounded Corners
    AppendCarvedLowerNose(mesh.get(), xFront, xFront + 0.48f, cabBaseY, windBotY, cabHW, whitePaint);

    // Curved Gloss-Black Scania Next-Gen R BEV Upper Collar + T-Grille + Electric Blue Lower Bar
    AppendCurvedPanelX(mesh.get(), xFront - 0.002f, 0.100f, windBotY - 0.21f, windBotY - 0.03f, windBotY - 0.21f, windBotY - 0.03f, -cabHW * 0.72f, cabHW * 0.72f, cabHW, 0.09f, 6, glossBlack, 0.015f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.024f, windBotY - 0.15f, windBotY - 0.08f, windBotY - 0.15f, windBotY - 0.08f, -0.36f, 0.36f, cabHW, 0.09f, 4, chromeCol, 0.006f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.004f, cabClearY + 0.44f, windBotY - 0.23f, cabHW * 0.56f, cabHW * 0.66f, 3, frameDark, glossBlack, -1, chromeCol, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.004f, cabBaseY + 0.04f,  cabClearY + 0.38f, cabHW * 0.50f, cabHW * 0.56f, 3, frameDark, glossBlack, 2, evBlue, cabHW, 0.09f);
    float crestX = CurvedNoseXAtZ(cabHW * 0.50f, xFront - 0.010f, cabHW, 0.09f);
    AppendFrontDiscX(mesh.get(), crestX, cabClearY + 0.66f, cabHW * 0.50f, 0.045f, 0.014f, 14, chromeCol, Vec4(0.84f, 0.16f, 0.16f, 1.0f));
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.026f, cabBaseY - 0.07f, cabBaseY + 0.01f, cabBaseY - 0.07f, cabBaseY + 0.01f, -0.26f, 0.26f, cabHW, 0.09f, 4, Vec4(0.94f, 0.96f, 0.98f, 1.0f), 0.006f);

    // Carved Scania Corner LED Headlamps + Curved Lower Bumper DRL Bar
    for (int side : {-1, 1}) {
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.002f, cabHW, 0.09f,
            cabClearY + 0.08f, cabClearY + 0.28f,
            cabHW * 0.58f, cabHW - 0.08f, side,
            0.032f, 0.015f, glossBlack, ledWhite, ledWhite, amberCol,
            true, false
        );
        float s = static_cast<float>(side);
        float zA = s * (cabHW * 0.59f), zB = s * (cabHW - 0.10f);
        AppendCurvedPanelX(mesh.get(), xFront - 0.002f, 0.080f, cabBaseY + 0.01f, cabBaseY + 0.075f, cabBaseY + 0.01f, cabBaseY + 0.075f,
                           std::min(zA, zB), std::max(zA, zB), cabHW, 0.09f, 3, ledWhite, 0.008f);
    }

    // Lower windshield cowl & wipers + carved roof & sleek A-pillars + corner roof brow LED spotlamps
    AppendTruckCowlAndWipersX(mesh.get(), xFront, cabHW, windBotY, glossBlack);
    AppendCarvedRoofAndSleeper(mesh.get(), xFront, cabBackX, doorX1, cabClearY, windBotY, windTopY, cabTopY, cabHW, whitePaint, glossBlack);

    for (float zSpot : {-0.80f, 0.80f}) {
        AppendCurvedPanelX(mesh.get(), xFront + 0.25f, 0.095f, windTopY + 0.14f, windTopY + 0.23f, windTopY + 0.14f, windTopY + 0.23f,
                           zSpot - 0.12f, zSpot + 0.12f, cabHW, 0.09f, 2, ledWhite, 0.008f);
    }

    std::vector<HullStationX> roofPod = {
        {xFront + 0.90f, cabTopY - 0.08f, cabTopY + 0.02f, cabHW - 0.12f, 0.04f, 0.12f, 0.03f, 0.05f},
        {cabBackX,       cabTopY - 0.08f, std::min(H - 0.02f, cabTopY + 0.20f), cabHW - 0.04f, 0.04f, 0.14f, 0.04f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), roofPod, whitePaint, 4);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.04f, cabBaseY + 0.34f, s > 0 ? cabHW - 0.05f : -cabHW), Vec3(cabBackX + 0.24f, cabTopY - 0.12f, s > 0 ? cabHW : -cabHW + 0.05f), whitePaint);
    }

    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.15f, false, false, whitePaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.15f, false, false, whitePaint, glossBlack, sideGlass, doorBeltY);
    CabComponents::AppendCantileverCoachMirrors(mesh.get(), xFront + 0.46f, cabTopY - 0.34f, cabHW, glossBlack);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.34f, cabTopY - 0.18f, cabHW, glossBlack);

    float skirtStart = cabBackX + 0.22f;
    float skirtEnd   = std::max(skirtStart + 0.55f, driveCenterX - archHalfLen - 0.10f);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f, -hw + 0.03f), Vec3(skirtEnd, frameTopY + 0.02f, -hw + 0.10f), whitePaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f,  hw - 0.10f), Vec3(skirtEnd, frameTopY + 0.02f,  hw - 0.03f), whitePaint);
    for (float dx : {skirtStart + 0.22f, skirtStart + 0.44f}) {
        if (dx + 0.08f < skirtEnd) {
            RunningGear::AppendBox(mesh.get(), Vec3(dx, frameBotY - 0.16f, -hw + 0.01f), Vec3(dx + 0.06f, frameTopY - 0.04f, -hw + 0.04f), glossBlack);
            RunningGear::AppendBox(mesh.get(), Vec3(dx, frameBotY - 0.16f,  hw - 0.04f), Vec3(dx + 0.06f, frameTopY - 0.04f,  hw - 0.01f), glossBlack);
        }
    }
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.04f, frameBotY - 0.08f, -hw + 0.02f), Vec3(skirtEnd - 0.04f, frameBotY - 0.03f, -hw + 0.04f), evBlue);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.04f, frameBotY - 0.08f,  hw - 0.04f), Vec3(skirtEnd - 0.04f, frameBotY - 0.03f,  hw - 0.02f), evBlue);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.12f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.66f, frameTopY + 0.04f, 0.45f), alloyCol);

    // Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), windFrontX, windBrowX, windBotY, windTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.15f, false, true, whitePaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.15f, false, true, whitePaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
