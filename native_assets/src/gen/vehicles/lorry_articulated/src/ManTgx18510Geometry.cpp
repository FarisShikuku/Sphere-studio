#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/images (18).jpeg
// Signature features: Signal-orange MAN TGX 18.510 TG3 flagship tractor with smoothly carved
// corners & curved front prow, panoramic TG3 windshield proportions matching split 2-pane doors,
// curved gloss-black shield topped by brushed-chrome Lion brow bar & MAN wordmark, 4-slat curved
// lower grille, carved corner headlamps with hockey-stick white LED DRLs & fog lamps, smoked
// curved sunvisor, dual whip antennas, side chrome air horns, and signal-orange aero side skirts.
std::shared_ptr<Mesh> GenerateManTgx18510Mesh(const VehicleDef& def) {
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
    const float cabTopY = std::clamp(def.cab_height_m, 3.38f, 4.00f);

    const float frontAxleX = xFront + 1.40f;
    const float driveCenterX = std::min(xRear - 0.95f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX = std::clamp(def.fifth_wheel_offset_m, 0.85f, xRear - 0.95f);

    const Vec4 orangePaint = def.base_color;
    const Vec4 darkTrim = def.accent_color;
    const Vec4 frameDark(0.13f, 0.15f, 0.18f, 1.0f);
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 chromeCol(0.88f, 0.91f, 0.96f, 1.0f);
    const Vec4 alloyCol(0.78f, 0.81f, 0.86f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 interiorSeat(0.18f, 0.20f, 0.24f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.15f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);
    const Vec4 amberCol(0.96f, 0.62f, 0.08f, 1.0f);
    const Vec4 ledWhite = def.headlights_on ? Vec4(0.96f, 0.99f, 1.00f, 1.0f) : chromeCol;

    // 1. Chassis & axles
    AppendTractorChassisBase(
        mesh.get(), def, xFront, xRear, frontAxleX, driveCenterX, fifthWheelX,
        wheelR, tireW, hw, cabHW, frameBotY, frameTopY, archTopY, archHalfLen,
        frameDark, orangePaint, linerDark, alloyCol
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
        cabClearY + 0.06f, cabTopY - 0.16f, cabHW, true, dashDark, interiorSeat, orangePaint,
        def.is_rhd, windBotY, def.steer_angle_deg
    );

    // 3. Smoothly Carved Curved Lower Nose & Rounded Corners
    AppendCarvedLowerNose(mesh.get(), xFront, xFront + 0.48f, cabBaseY, windBotY, cabHW, orangePaint);

    // Signature MAN TG3 Curved Gloss-Black Shield Grille + Brushed-Chrome Lion Top Brow + Centered MAN Bar
    float shBot = cabClearY + 0.54f;
    float shTop = windBotY - 0.08f;
    AppendTrapezoidGrillePocketX(mesh.get(), xFront - 0.005f, xFront + 0.105f, shBot, shTop, cabHW * 0.42f, cabHW * 0.58f, 0.024f, glossBlack, glossBlack, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.032f, shTop - 0.078f, shTop + 0.005f, shTop - 0.078f, shTop + 0.005f, -cabHW * 0.54f, cabHW * 0.54f, cabHW, 0.09f, 6, chromeCol, 0.008f);
    AppendFrontDiscX(mesh.get(), xFront - 0.014f, shTop - 0.035f, 0.0f, 0.048f, 0.016f, 16, glossBlack, chromeCol);
    AppendCurvedPanelX(mesh.get(), xFront - 0.014f, 0.028f, (shBot + shTop) * 0.5f - 0.042f, (shBot + shTop) * 0.5f + 0.042f,
                       (shBot + shTop) * 0.5f - 0.042f, (shBot + shTop) * 0.5f + 0.042f, -0.25f, 0.25f, cabHW, 0.09f, 4, chromeCol, 0.008f);

    // Wide 4-Slat Curved Lower Trapezoidal Grille + Euro Plate + Chin Air Dam
    float lowGrBot = cabBaseY + 0.02f;
    float lowGrTop = cabClearY + 0.48f;
    AppendTrapezoidGrillePocketX(mesh.get(), xFront - 0.004f, xFront + 0.105f, lowGrBot, lowGrTop, cabHW * 0.42f, cabHW * 0.50f, 0.026f, glossBlack, glossBlack, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.004f, lowGrBot + 0.05f, lowGrTop - 0.03f, cabHW * 0.39f, cabHW * 0.47f, 4, chromeCol, glossBlack, -1, chromeCol, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.025f, lowGrBot - 0.01f, lowGrBot + 0.055f, lowGrBot - 0.01f, lowGrBot + 0.055f, -0.22f, 0.22f, cabHW, 0.09f, 4, Vec4(0.95f, 0.96f, 0.98f, 1.0f), 0.006f);
    AppendCurvedPanelX(mesh.get(), xFront + 0.004f, 0.090f, cabBaseY - 0.12f, cabBaseY - 0.02f, cabBaseY - 0.12f, cabBaseY - 0.02f, -(cabHW * 0.46f), cabHW * 0.46f, cabHW, 0.09f, 6, glossBlack, 0.012f);

    // Carved Corner MAN TG3 Headlamps (curving around cab corners with swept upper slope & hockey-stick DRL)
    for (int side : {-1, 1}) {
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.002f, cabHW, 0.09f,
            cabClearY + 0.20f, cabClearY + 0.42f,
            cabHW * 0.56f, cabHW - 0.08f, side,
            0.045f, 0.015f, glossBlack, ledWhite, ledWhite, amberCol,
            true, false
        );
        // Separate curved lower bumper fog & cornering lamp pod
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.002f, cabHW, 0.09f,
            cabBaseY - 0.04f, cabBaseY + 0.10f,
            cabHW * 0.58f, cabHW - 0.10f, side,
            0.018f, 0.012f, glossBlack, ledWhite, ledWhite, amberCol,
            false, false
        );
        // Flush corner aero air-curtain strake beside upper shield
        float s = static_cast<float>(side);
        float zFin0 = s > 0 ? (cabHW - 0.09f) : -(cabHW - 0.03f);
        float zFin1 = s > 0 ? (cabHW - 0.03f) : -(cabHW - 0.09f);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.13f, shBot + 0.06f, zFin0), Vec3(xFront + 0.24f, shTop - 0.04f, zFin1), glossBlack);
    }

    // Lower windshield cowl & wipers
    AppendTruckCowlAndWipersX(mesh.get(), xFront, cabHW, windBotY, glossBlack);

    // 4. Signal-Orange Highline Roof, Curved Smoked Sunvisor, Dual Antennas, Air Horns & Aero Spoiler
    AppendCarvedRoofAndSleeper(mesh.get(), xFront, cabBackX, doorX1, cabClearY, windBotY, windTopY, cabTopY, cabHW, orangePaint, glossBlack);
    AppendSunvisorBrow(mesh.get(), xFront + 0.30f, xFront + 0.54f, windTopY + 0.02f, windTopY + 0.19f, cabHW, glossBlack, chromeCol, false);

    std::vector<HullStationX> roofPod = {
        {xFront + 0.90f, cabTopY - 0.08f, cabTopY + 0.02f, cabHW - 0.12f, 0.04f, 0.12f, 0.03f, 0.06f},
        {cabBackX,       cabTopY - 0.08f, std::min(H - 0.02f, cabTopY + 0.26f), cabHW - 0.04f, 0.04f, 0.16f, 0.05f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), roofPod, orangePaint, 4);
    ChassisComponents::AppendClosedCylinderY(mesh.get(), xFront + 0.66f, -0.52f, cabTopY, cabTopY + 0.44f, 0.012f, 8, glossBlack);
    ChassisComponents::AppendClosedCylinderY(mesh.get(), xFront + 0.66f,  0.52f, cabTopY, cabTopY + 0.44f, 0.012f, 8, glossBlack);
    AppendRoofAirHorns(mesh.get(), xFront + 0.68f, cabTopY + 0.02f, 0.72f, chromeCol);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.04f, cabBaseY + 0.34f, s > 0 ? cabHW - 0.05f : -cabHW), Vec3(cabBackX + 0.24f, cabTopY - 0.12f, s > 0 ? cabHW : -cabHW + 0.05f), orangePaint);
        RunningGear::AppendBox(mesh.get(), Vec3(doorX0 + 0.04f, cabClearY + 0.04f, s > 0 ? cabHW - 0.01f : -cabHW - 0.012f), Vec3(doorX1 - 0.04f, cabClearY + 0.10f, s > 0 ? cabHW + 0.012f : -cabHW + 0.01f), chromeCol);
        AppendSideRibbonZ(mesh.get(), cabHW * s, 0.012f, side > 0, doorX0 + 0.06f, cabClearY + 0.72f, cabBackX - 0.06f, cabClearY + 0.72f, 0.016f, glossBlack);
    }

    // 5. Split 2-Pane MAN TG3 Doors aligned to panoramic windshield + Cantilever Mirrors + Side Skirts
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.06f, true, false, orangePaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.06f, true, false, orangePaint, glossBlack, sideGlass, doorBeltY);
    CabComponents::AppendCantileverCoachMirrors(mesh.get(), xFront + 0.46f, cabTopY - 0.34f, cabHW, darkTrim);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.34f, windTopY - 0.14f, -cabHW, darkTrim);
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.46f, cabTopY - 0.08f, cabHW * 0.85f, def.headlights_on);

    float skirtStart = cabBackX + 0.22f;
    float skirtEnd   = std::max(skirtStart + 0.55f, driveCenterX - archHalfLen - 0.10f);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f, -hw + 0.03f), Vec3(skirtEnd, frameTopY + 0.02f, -hw + 0.10f), orangePaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f,  hw - 0.10f), Vec3(skirtEnd, frameTopY + 0.02f,  hw - 0.03f), orangePaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.08f, frameBotY - 0.12f, -hw + 0.01f), Vec3(skirtStart + 0.28f, frameTopY - 0.02f, -hw + 0.04f), glossBlack);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.08f, frameBotY - 0.12f,  hw - 0.04f), Vec3(skirtStart + 0.28f, frameTopY - 0.02f,  hw - 0.01f), glossBlack);
    RunningGear::AppendBox(mesh.get(), Vec3((skirtStart + skirtEnd) * 0.5f - 0.06f, frameBotY - 0.16f, -hw + 0.01f), Vec3((skirtStart + skirtEnd) * 0.5f + 0.06f, frameBotY - 0.11f, -hw + 0.04f), amberCol);
    RunningGear::AppendBox(mesh.get(), Vec3((skirtStart + skirtEnd) * 0.5f - 0.06f, frameBotY - 0.16f,  hw - 0.04f), Vec3((skirtStart + skirtEnd) * 0.5f + 0.06f, frameBotY - 0.11f,  hw - 0.01f), amberCol);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.12f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.66f, frameTopY + 0.04f, 0.45f), alloyCol);

    // 6. Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), windFrontX, windBrowX, windBotY, windTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.06f, true, true, orangePaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.06f, true, true, orangePaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
