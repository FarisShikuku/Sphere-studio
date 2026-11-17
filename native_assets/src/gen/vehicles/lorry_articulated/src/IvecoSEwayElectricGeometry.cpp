#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/images (21).jpeg
// Signature features: Silver-metallic Iveco S-eWay AS cab with carved corners & curved prow,
// panoramic windshield ratio aligned with kerb-dipped doors, curved gloss-black sunvisor with
// cyan ELECTRIC bar, IVECO wordmark above massive curved inverted-trapezoid 6-slat grille outlined
// in continuous EV-blue V-frame, carved raked corner LED headlamps & A-pillar aero corner fins.
std::shared_ptr<Mesh> GenerateIvecoSEwayElectricMesh(const VehicleDef& def) {
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
    const float cabBackX = xFront + 2.50f;
    const float cabTopY = std::clamp(def.cab_height_m, 3.38f, 4.00f);

    const float frontAxleX = xFront + 1.42f;
    const float driveCenterX = std::min(xRear - 0.95f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX = std::clamp(def.fifth_wheel_offset_m, 0.85f, xRear - 0.95f);

    const Vec4 silverPaint = def.base_color;
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
    const Vec4 ledWhite = def.headlights_on ? Vec4(0.96f, 0.99f, 1.00f, 1.0f) : Vec4(0.76f, 0.82f, 0.88f, 1.0f);
    const Vec4 cyanGlow(0.30f, 0.90f, 1.00f, 1.0f);
    const Vec4 amberCol(0.96f, 0.62f, 0.08f, 1.0f);

    // 1. Chassis & axles
    AppendTractorChassisBase(
        mesh.get(), def, xFront, xRear, frontAxleX, driveCenterX, fifthWheelX,
        wheelR, tireW, hw, cabHW, frameBotY, frameTopY, archTopY, archHalfLen,
        frameDark, glossBlack, linerDark, alloyCol
    );

    const float windFrontX = xFront + 0.14f;
    const float windBrowX  = xFront + 0.34f;
    const float windBotY   = cabClearY + 1.22f;
    const float windTopY   = std::min(cabTopY - 0.52f, windBotY + 0.86f);
    const float doorX0     = xFront + 0.44f;
    const float doorX1     = cabBackX - 0.70f;
    const float doorTopY   = windTopY + 0.05f;
    const float doorBeltY  = windBotY - 0.02f;

    // 2. Interior
    CabComponents::AppendTruckCabInterior(
        mesh.get(), xFront + 0.32f, cabBackX - 0.08f,
        cabClearY + 0.06f, cabTopY - 0.16f, cabHW, true, dashDark, interiorSeat, evBlue
    );

    // 3. Smoothly Carved Curved Lower Prow & Corners
    AppendCarvedLowerNose(mesh.get(), xFront, xFront + 0.48f, cabBaseY, windBotY, cabHW, silverPaint);

    // Centered curved dark IVECO wordmark bar + front license plate
    AppendCurvedPanelX(mesh.get(), xFront - 0.004f, 0.045f, windBotY - 0.15f, windBotY - 0.08f, windBotY - 0.15f, windBotY - 0.08f, -0.26f, 0.26f, cabHW, 0.09f, 4, frameDark, 0.008f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.008f, 0.032f, cabBaseY - 0.12f, cabBaseY - 0.05f, cabBaseY - 0.12f, cabBaseY - 0.05f, -0.22f, 0.22f, cabHW, 0.09f, 4, glossBlack, 0.006f);

    // Signature Massive Curved Inverted-Trapezoid V-Grille (6 slats + Continuous EV-Blue V-Outline)
    float grBot = cabBaseY - 0.01f;
    float grTop = windBotY - 0.20f;
    float hwBot = cabHW * 0.36f;
    float hwTop = cabHW * 0.68f;
    AppendTrapezoidGrillePocketX(mesh.get(), xFront - 0.004f, xFront + 0.105f, grBot, grTop, hwBot, hwTop, 0.026f, glossBlack, glossBlack, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.005f, grBot + 0.04f, grTop - 0.03f, hwBot - 0.02f, hwTop - 0.03f, 6, chromeCol, glossBlack, -1, chromeCol, cabHW, 0.09f);

    AppendSlantedRibbonX(mesh.get(), xFront - 0.012f, 0.022f, grBot, -hwBot, grTop, -hwTop, 0.024f, evBlue, cabHW, 0.09f);
    AppendSlantedRibbonX(mesh.get(), xFront - 0.012f, 0.022f, grBot,  hwBot, grTop,  hwTop, 0.024f, evBlue, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.014f, 0.024f, grBot - 0.022f, grBot + 0.022f, grBot - 0.022f, grBot + 0.022f, -hwBot, hwBot, cabHW, 0.09f, 6, evBlue, 0.006f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.014f, 0.024f, grTop - 0.020f, grTop + 0.020f, grTop - 0.020f, grTop + 0.020f, -hwTop, hwTop, cabHW, 0.09f, 6, evBlue, 0.006f);

    // Carved Raked Corner LED Headlamps hugging curved bumper cheeks + lower corner splitter pocket
    for (int side : {-1, 1}) {
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.002f, cabHW, 0.09f,
            cabBaseY + 0.04f, cabClearY + 0.30f,
            hwBot + 0.10f, cabHW - 0.08f, side,
            0.015f, 0.055f, glossBlack, ledWhite, ledWhite, amberCol,
            true, false
        );
        float s = static_cast<float>(side);
        float zA = s * (hwBot + 0.09f), zB = s * (cabHW - 0.10f);
        AppendCurvedPanelX(mesh.get(), xFront + 0.002f, 0.095f, cabBaseY - 0.12f, cabBaseY - 0.01f, cabBaseY - 0.12f, cabBaseY - 0.01f,
                           std::min(zA, zB), std::max(zA, zB), cabHW, 0.09f, 3, glossBlack, 0.012f);
        float zFin0 = s > 0 ? (cabHW - 0.10f) : -(cabHW - 0.03f);
        float zFin1 = s > 0 ? (cabHW - 0.03f) : -(cabHW - 0.10f);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.13f, cabClearY + 0.36f, zFin0), Vec3(xFront + 0.25f, windBotY - 0.12f, zFin1), glossBlack);
    }

    // Lower windshield cowl & wipers
    AppendTruckCowlAndWipersX(mesh.get(), xFront, cabHW, windBotY, glossBlack);

    // 4. Highline S-eWay Roof, Carved Sleeper, Curved Cyan ELECTRIC Visor & B-Pillar Louvers
    AppendCarvedRoofAndSleeper(mesh.get(), xFront, cabBackX, doorX1, cabClearY, windBotY, windTopY, cabTopY, cabHW, silverPaint, glossBlack);
    AppendSunvisorBrow(mesh.get(), xFront + 0.30f, xFront + 0.54f, windTopY + 0.02f, windTopY + 0.19f, cabHW, glossBlack, cyanGlow, true);

    std::vector<HullStationX> roofPod = {
        {xFront + 0.92f, cabTopY - 0.08f, cabTopY + 0.02f, cabHW - 0.12f, 0.04f, 0.12f, 0.03f, 0.06f},
        {cabBackX,       cabTopY - 0.08f, std::min(H - 0.02f, cabTopY + 0.22f), cabHW - 0.04f, 0.04f, 0.16f, 0.05f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), roofPod, silverPaint, 4);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.04f, cabBaseY + 0.34f, s > 0 ? cabHW - 0.05f : -cabHW), Vec3(cabBackX + 0.24f, cabTopY - 0.12f, s > 0 ? cabHW : -cabHW + 0.05f), silverPaint);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.42f, windBotY - 0.02f, s > 0 ? cabHW - 0.01f : -cabHW - 0.012f), Vec3(doorX1 + 0.14f, windBotY + 0.03f, s > 0 ? cabHW + 0.012f : -cabHW + 0.01f), evBlue);
        RunningGear::AppendBox(mesh.get(), Vec3(doorX1 + 0.02f, windBotY + 0.10f, s > 0 ? cabHW - 0.01f : -cabHW - 0.012f), Vec3(doorX1 + 0.22f, windTopY - 0.12f, s > 0 ? cabHW + 0.012f : -cabHW + 0.01f), glossBlack);
    }

    // 5. Kerb-Dipped Doors aligned to panoramic windshield + Mirrors + Battery Skirts
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.10f, false, false, silverPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.10f, false, false, silverPaint, glossBlack, sideGlass, doorBeltY);
    CabComponents::AppendCantileverCoachMirrors(mesh.get(), xFront + 0.46f, cabTopY - 0.34f, cabHW, glossBlack);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.34f, windTopY - 0.14f, -cabHW, glossBlack);
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.46f, cabTopY - 0.08f, cabHW * 0.85f, def.headlights_on);

    float skirtStart = cabBackX + 0.22f;
    float skirtEnd   = std::max(skirtStart + 0.55f, driveCenterX - archHalfLen - 0.10f);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f, -hw + 0.03f), Vec3(skirtEnd, frameTopY + 0.02f, -hw + 0.10f), silverPaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f,  hw - 0.10f), Vec3(skirtEnd, frameTopY + 0.02f,  hw - 0.03f), silverPaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.04f, frameBotY - 0.08f, -hw + 0.01f), Vec3(skirtEnd - 0.04f, frameBotY - 0.03f, -hw + 0.04f), evBlue);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.04f, frameBotY - 0.08f,  hw - 0.04f), Vec3(skirtEnd - 0.04f, frameBotY - 0.03f,  hw - 0.01f), evBlue);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.12f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.66f, frameTopY + 0.04f, 0.45f), alloyCol);

    // 6. Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), windFrontX, windBrowX, windBotY, windTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.10f, false, true, silverPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.10f, false, true, silverPaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
