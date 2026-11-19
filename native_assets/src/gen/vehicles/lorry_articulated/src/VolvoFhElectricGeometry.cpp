#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/images (13).jpeg & volvo-fh-electric-cgi-exterior-1.webp
// Signature features: Emerald-teal Globetrotter XL cab with smoothly carved curved corners &
// rounded roof crown, panoramic FH windshield aligned to steep kerb-dipped doors, MirrorCam pods,
// curved VOLVO letter collar + dual-tier emerald grilles, continuous curved diagonal chrome Iron Mark
// sash & badge, carved V-shaped Thor's Hammer LED corner headlamps & emerald battery side skirts.
std::shared_ptr<Mesh> GenerateVolvoFhElectricMesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 5.4f, 8.8f);
    const float W = std::clamp(def.dimensions.y, 2.38f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.25f, 4.10f);
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
    const float cabBackX = xFront + 2.52f;
    const float cabTopY = std::clamp(def.cab_height_m, 3.35f, 4.02f);

    const float frontAxleX = xFront + 1.42f;
    const float driveCenterX = std::min(xRear - 0.95f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX = std::clamp(def.fifth_wheel_offset_m, 0.85f, xRear - 0.95f);

    const Vec4 tealPaint = def.base_color;
    const Vec4 evCyan = def.accent_color;
    const Vec4 frameDark(0.13f, 0.15f, 0.18f, 1.0f);
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 darkTrim(0.11f, 0.12f, 0.14f, 1.0f);
    const Vec4 chromeCol(0.88f, 0.91f, 0.96f, 1.0f);
    const Vec4 alloyCol(0.78f, 0.81f, 0.86f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 interiorSeat(0.18f, 0.20f, 0.24f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.15f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);
    const Vec4 amberWarn(0.95f, 0.72f, 0.10f, 1.0f);
    const Vec4 ledWhite = def.headlights_on ? Vec4(0.96f, 0.99f, 1.00f, 1.0f) : Vec4(0.76f, 0.82f, 0.88f, 1.0f);

    // 1. Chassis & axles
    AppendTractorChassisBase(
        mesh.get(), def, xFront, xRear, frontAxleX, driveCenterX, fifthWheelX,
        wheelR, tireW, hw, cabHW, frameBotY, frameTopY, archTopY, archHalfLen,
        frameDark, darkTrim, linerDark, alloyCol
    );

    const float windFrontX = xFront + 0.14f;
    const float windBrowX  = xFront + 0.36f;
    const float windBotY   = cabClearY + 1.16f;
    const float windTopY   = std::min(cabTopY - 0.52f, windBotY + 0.84f);
    const float doorX0     = xFront + 0.44f;
    const float doorX1     = cabBackX - 0.72f;
    const float doorTopY   = windTopY + 0.05f;
    const float doorBeltY  = windBotY - 0.02f;

    // 2. Volvo FH Electric Interior (LHD/RHD mirrored cockpit + ambient cyan glow)
    CabComponents::AppendVolvoCabInterior(
        mesh.get(), xFront + 0.32f, cabBackX - 0.08f,
        cabClearY + 0.06f, windBotY, cabTopY - 0.16f,
        cabHW, true, def.is_rhd, def.steer_angle_deg, def.headlights_on,
        dashDark, interiorSeat, chromeCol, evCyan
    );

    // 3. Grounded Front Bumper Apron + Carved Lower Nose & Tight Semicircular Wheel Arches
    const float bumperBotY = std::clamp(def.ground_clearance_m, 0.24f, 0.36f);
    const float bumperEndX = frontAxleX - archHalfLen + 0.02f;
    std::vector<HullStationX> evBumper = {
        {xFront + 0.10f, bumperBotY, cabClearY + 0.02f, cabHW - 0.14f, 0.10f, 0.10f, 0.00f, 0.08f},
        {xFront + 0.26f, bumperBotY, cabClearY + 0.04f, cabHW - 0.02f, 0.10f, 0.08f, 0.00f, 0.04f},
        {bumperEndX,     bumperBotY, cabClearY + 0.04f, cabHW,         0.08f, 0.08f, 0.00f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), evBumper, tealPaint, 6);
    AppendCarvedLowerNose(mesh.get(), xFront, xFront + 0.52f, cabClearY, windBotY, cabHW, tealPaint);
    for (int side : {-1, 1}) {
        bool rightSide = (side > 0);
        float sideZ = rightSide ? cabHW : -cabHW;
        CabComponents::AppendSemicircularWheelArchSkirt(
            mesh.get(), frontAxleX, wheelR, wheelR + 0.068f, archHalfLen,
            bumperBotY, cabClearY + 0.04f, sideZ, rightSide, 0.042f, tealPaint, glossBlack
        );
        CabComponents::AppendWheelArchFender(
            mesh.get(), frontAxleX, wheelR, wheelR + 0.072f, sideZ, rightSide, 0.042f, tealPaint
        );
    }
    AppendCabEntrySteps(mesh.get(), frontAxleX + archHalfLen, std::min(doorX1, cabBackX - 0.04f), bumperBotY + 0.06f, cabClearY + 0.02f, cabHW, glossBlack, alloyCol);

    // Upper Curved Gloss-Black VOLVO Collar Directly Beneath Windshield + Silver VOLVO Wordmark
    AppendTrapezoidGrillePocketX(mesh.get(), xFront - 0.002f, xFront + 0.105f, windBotY - 0.22f, windBotY - 0.01f, cabHW * 0.74f, cabHW * 0.82f, 0.024f, glossBlack, glossBlack, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.026f, windBotY - 0.14f, windBotY - 0.08f, windBotY - 0.14f, windBotY - 0.08f, -0.34f, 0.34f, cabHW, 0.09f, 4, chromeCol, 0.006f);

    // Upper Perforated Emerald Grille + Black Intake Slot
    float upGrBot = cabClearY + 0.34f;
    float upGrTop = windBotY - 0.26f;
    AppendTrapezoidGrillePocketX(mesh.get(), xFront - 0.004f, xFront + 0.102f, upGrBot, upGrTop, cabHW * 0.54f, cabHW * 0.62f, 0.022f, glossBlack, glossBlack, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.005f, upGrBot + 0.06f, upGrTop - 0.02f, cabHW * 0.51f, cabHW * 0.58f, 4, tealPaint, glossBlack, -1, chromeCol, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.008f, 0.024f, upGrBot + 0.02f, upGrBot + 0.07f, upGrBot + 0.02f, upGrBot + 0.07f, -0.44f, 0.44f, cabHW, 0.09f, 4, glossBlack, 0.005f);

    // Lower Emerald Bumper Grille + Twin Brushed Chrome Bars + Smooth Diagonal Chrome Volvo Iron Mark Sash
    float lowGrBot = cabBaseY - 0.02f;
    float lowGrTop = upGrBot - 0.03f;
    AppendTrapezoidGrillePocketX(mesh.get(), xFront - 0.004f, xFront + 0.102f, lowGrBot, lowGrTop, cabHW * 0.46f, cabHW * 0.54f, 0.025f, glossBlack, glossBlack, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.005f, lowGrBot + 0.02f, lowGrTop - 0.02f, cabHW * 0.44f, cabHW * 0.51f, 3, chromeCol, glossBlack, -1, chromeCol, cabHW, 0.09f);

    AppendSlantedRibbonX(mesh.get(), xFront - 0.012f, 0.022f, lowGrBot + 0.04f, -cabHW * 0.48f, upGrTop - 0.06f, cabHW * 0.54f, 0.026f, chromeCol, cabHW, 0.09f);
    AppendFrontDiscX(mesh.get(), xFront - 0.016f, (lowGrBot + upGrTop) * 0.5f, 0.0f, 0.112f, 0.024f, 22, glossBlack, chromeCol);

    float warnX = CurvedNoseXAtZ(-cabHW * 0.70f, xFront - 0.006f, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.008f, 0.026f, windBotY - 0.36f, windBotY - 0.27f, windBotY - 0.36f, windBotY - 0.27f, -cabHW * 0.72f, -cabHW * 0.62f, cabHW, 0.09f, 2, amberWarn, 0.006f);
    (void)warnX;

    // Carved Vertical V-Shaped "Thor's Hammer" Corner Headlamps hugging curved bumper cheeks
    for (int side : {-1, 1}) {
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.002f, cabHW, 0.09f,
            cabBaseY + 0.00f, cabClearY + 0.32f,
            cabHW * 0.58f, cabHW - 0.08f, side,
            0.030f, 0.050f, glossBlack, ledWhite, ledWhite, amberWarn,
            false, false
        );
        float s = static_cast<float>(side);
        float zBotTip = s * (cabHW * 0.60f);
        float zTopTip = s * (cabHW - 0.10f);
        AppendSlantedRibbonX(mesh.get(), xFront - 0.012f, 0.018f, cabBaseY + 0.04f, zBotTip, cabClearY + 0.28f, zTopTip, 0.020f, ledWhite, cabHW, 0.09f);
    }

    // Lower windshield cowl & wipers
    AppendTruckCowlAndWipersX(mesh.get(), xFront, cabHW, windBotY, glossBlack);

    // 4. Globetrotter XL High Roof, Carved Sleeper, Black ELECTRIC Visor & Dual-Hump Roof Aero Kit
    AppendCarvedRoofAndSleeper(mesh.get(), xFront, cabBackX, doorX1, cabClearY, windBotY, windTopY, cabTopY, cabHW, tealPaint, glossBlack);
    AppendSunvisorBrow(mesh.get(), xFront + 0.30f, xFront + 0.54f, windTopY + 0.02f, windTopY + 0.19f, cabHW, glossBlack, Vec4(0.88f, 0.96f, 1.0f, 1.0f), true);

    std::vector<HullStationX> roofPod = {
        {xFront + 0.92f, cabTopY - 0.08f, cabTopY + 0.02f, cabHW - 0.12f, 0.04f, 0.12f, 0.03f, 0.06f},
        {cabBackX,       cabTopY - 0.08f, std::min(H - 0.02f, cabTopY + 0.24f), cabHW - 0.04f, 0.04f, 0.16f, 0.05f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), roofPod, tealPaint, 4);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.04f, cabBaseY + 0.35f, s > 0 ? cabHW - 0.05f : -cabHW), Vec3(cabBackX + 0.28f, cabTopY - 0.12f, s > 0 ? cabHW : -cabHW + 0.05f), tealPaint);
        for (int lv = 0; lv < 5; ++lv) {
            float ly = windTopY - 0.42f + static_cast<float>(lv) * 0.065f;
            float lw = 0.22f - static_cast<float>(lv) * 0.02f;
            RunningGear::AppendBox(mesh.get(), Vec3(doorX1 + 0.03f, ly, s > 0 ? cabHW - 0.01f : -cabHW - 0.014f), Vec3(doorX1 + 0.03f + lw, ly + 0.038f, s > 0 ? cabHW + 0.014f : -cabHW + 0.01f), glossBlack);
        }
        AppendSideRibbonZ(mesh.get(), cabHW * s, 0.014f, side > 0, xFront + 0.48f, cabClearY + 0.56f, cabBackX - 0.06f, cabClearY + 0.72f, 0.025f, frameDark);
    }

    // 5. MirrorCam Pods + Kerb-Dipped Doors aligned to panoramic windshield + Skirts
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.16f, false, false, tealPaint, glossBlack, sideGlass, doorBeltY, def.is_rhd, true);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.16f, false, false, tealPaint, glossBlack, sideGlass, doorBeltY, !def.is_rhd, true);
    AppendMirrorCamWings(mesh.get(), xFront + 0.46f, windTopY - 0.18f, cabHW, tealPaint, glossBlack, chromeCol);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.36f, windTopY - 0.14f, -cabHW, glossBlack);
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.46f, cabTopY - 0.10f, cabHW * 0.85f, def.headlights_on);

    float skirtStart = cabBackX + 0.24f;
    float skirtEnd   = std::max(skirtStart + 0.60f, driveCenterX - archHalfLen - 0.10f);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f, -hw + 0.03f), Vec3(skirtEnd, frameTopY + 0.02f, -hw + 0.10f), tealPaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f,  hw - 0.10f), Vec3(skirtEnd, frameTopY + 0.02f,  hw - 0.03f), tealPaint);
    for (float ly : {frameBotY - 0.14f, frameBotY + 0.04f}) {
        RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.06f, ly, -hw + 0.01f), Vec3(skirtStart + 0.28f, ly + 0.09f, -hw + 0.04f), chromeCol);
        RunningGear::AppendBox(mesh.get(), Vec3(skirtStart + 0.06f, ly,  hw - 0.04f), Vec3(skirtStart + 0.28f, ly + 0.09f,  hw - 0.01f), chromeCol);
    }
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.12f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.66f, frameTopY + 0.04f, 0.45f), alloyCol);

    // 6. Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), windFrontX, windBrowX, windBotY, windTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.16f, false, true, tealPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.16f, false, true, tealPaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
