#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/images (15).jpeg & images (16).jpeg
// Signature features: Extended smooth sealed ProCabin aero nose panel (+60mm carved prow),
// panoramic windshield & GigaSpace roof brow, crisp 3-pointed Mercedes Star medallion on sealed
// curved face, arched black mustache intake slit, curved hexagonal lower grille framing license plate,
// curved lime-green splitter lip, carved corner LED headlamps, twin horns, strobes, & MirrorCam.
std::shared_ptr<Mesh> GenerateMercedesEActros600Mesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 5.5f, 8.8f);
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
    const float cabClearY = std::max(cabBaseY + 0.22f, archTopY + 0.02f);
    const float cabHW = hw - 0.03f;
    const float cabBackX = xFront + 2.58f;
    const float cabTopY = std::clamp(def.cab_height_m, 3.45f, 4.05f);

    const float frontAxleX = xFront + 1.48f;
    const float driveCenterX = std::min(xRear - 0.95f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX = std::clamp(def.fifth_wheel_offset_m, 0.85f, xRear - 0.95f);

    const Vec4 bluePaint = def.base_color;
    const Vec4 limeAccent = def.accent_color;
    const Vec4 frameDark(0.13f, 0.15f, 0.18f, 1.0f);
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 chromeCol(0.88f, 0.91f, 0.96f, 1.0f);
    const Vec4 plateWhite(0.93f, 0.95f, 0.97f, 1.0f);
    const Vec4 alloyCol(0.78f, 0.81f, 0.86f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 interiorSeat(0.18f, 0.20f, 0.24f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.15f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);
    const Vec4 ledWhite = def.headlights_on ? Vec4(0.96f, 0.99f, 1.00f, 1.0f) : Vec4(0.76f, 0.82f, 0.88f, 1.0f);
    const Vec4 amberStrobe(0.98f, 0.64f, 0.10f, 1.0f);

    // 1. Chassis & axles
    AppendTractorChassisBase(
        mesh.get(), def, xFront, xRear, frontAxleX, driveCenterX, fifthWheelX,
        wheelR, tireW, hw, cabHW, frameBotY, frameTopY, archTopY, archHalfLen,
        frameDark, glossBlack, linerDark, alloyCol
    );

    const float windFrontX = xFront + 0.16f;
    const float windBrowX  = xFront + 0.36f;
    const float windBotY   = cabClearY + 1.24f;
    const float windTopY   = std::min(cabTopY - 0.54f, windBotY + 0.86f);
    const float doorX0     = xFront + 0.50f;
    const float doorX1     = cabBackX - 0.74f;
    const float doorTopY   = windTopY + 0.05f;
    const float doorBeltY  = windBotY - 0.02f;

    CabComponents::AppendTruckCabInterior(
        mesh.get(), xFront + 0.36f, cabBackX - 0.08f,
        cabClearY + 0.06f, cabTopY - 0.16f, cabHW, true, dashDark, interiorSeat, limeAccent
    );

    // 2. Signature Ultra-Smooth Extended Carved Aero ProCabin Nose (extended -60mm cleanly forward)
    const float proTipX = xFront - 0.06f;
    AppendCarvedLowerNose(mesh.get(), proTipX, xFront + 0.52f, cabBaseY, windBotY, cabHW, bluePaint);

    // Centered Three-Pointed Star Medallion on Smooth Sealed ProCabin Panel
    float starY = cabClearY + 0.74f;
    AppendMercedesStarX(mesh.get(), proTipX - 0.008f, starY, 0.0f, 0.160f, glossBlack, chromeCol, chromeCol);

    // Curved Gloss-Black "Mustache" Cooling Mouth Slit above lower bumper
    float mBot = cabClearY + 0.28f;
    float mTop = cabClearY + 0.40f;
    AppendTrapezoidGrillePocketX(mesh.get(), proTipX - 0.005f, proTipX + 0.085f, mBot, mTop, 0.58f, 0.44f, 0.024f, glossBlack, glossBlack, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), proTipX - 0.006f, mBot + 0.01f, mTop - 0.01f, 0.54f, 0.40f, 2, frameDark, glossBlack, -1, chromeCol, cabHW, 0.09f);

    // Wide Curved Hexagonal Lower Bumper Intake + Paint-Matched Chin Beam + Lime-Green Aero Splitter Lip
    float bTop = cabClearY + 0.21f;
    float bBot = cabBaseY - 0.06f;
    AppendTrapezoidGrillePocketX(mesh.get(), proTipX - 0.005f, proTipX + 0.085f, bBot, bTop, cabHW * 0.70f, cabHW * 0.78f, 0.026f, chromeCol, glossBlack, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), proTipX - 0.006f, bBot + 0.02f, bTop - 0.02f, cabHW * 0.66f, cabHW * 0.74f, 4, frameDark, glossBlack, -1, chromeCol, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), proTipX - 0.010f, 0.036f, cabBaseY - 0.06f, cabBaseY + 0.04f, cabBaseY - 0.06f, cabBaseY + 0.04f, -cabHW * 0.58f, cabHW * 0.58f, cabHW, 0.09f, 6, bluePaint, 0.010f);
    AppendCurvedPanelX(mesh.get(), proTipX - 0.016f, 0.024f, cabBaseY - 0.04f, cabBaseY + 0.03f, cabBaseY - 0.04f, cabBaseY + 0.03f, -0.26f, 0.26f, cabHW, 0.09f, 4, plateWhite, 0.006f);
    AppendCurvedPanelX(mesh.get(), proTipX - 0.014f, 0.085f, cabBaseY - 0.11f, cabBaseY - 0.06f, cabBaseY - 0.11f, cabBaseY - 0.06f, -cabHW * 0.84f, cabHW * 0.84f, cabHW, 0.09f, 6, limeAccent, 0.010f);

    // Carved Vertical Chrome & C-Trimmed Corner Bumper Headlamps
    for (int side : {-1, 1}) {
        AppendCarvedHeadlampClusterX(
            mesh.get(), proTipX - 0.002f, cabHW, 0.09f,
            cabBaseY - 0.02f, bTop + 0.04f,
            cabHW - 0.34f, cabHW - 0.08f, side,
            0.035f, 0.018f, glossBlack, ledWhite, ledWhite, amberStrobe,
            false, false
        );
    }

    // Lower windshield cowl & wipers
    AppendTruckCowlAndWipersX(mesh.get(), xFront + 0.02f, cabHW, windBotY, glossBlack);

    // 3. GigaSpace High Roof, Carved Sleeper, Visor, Air Horns & Dual Amber Beacons
    AppendCarvedRoofAndSleeper(mesh.get(), xFront + 0.02f, cabBackX, doorX1, cabClearY, windBotY, windTopY, cabTopY, cabHW, bluePaint, glossBlack);
    AppendSunvisorBrow(mesh.get(), xFront + 0.32f, xFront + 0.56f, windTopY + 0.02f, windTopY + 0.19f, cabHW, glossBlack, chromeCol, true);
    AppendRoofAirHorns(mesh.get(), xFront + 0.72f, cabTopY + 0.03f, 0.64f, chromeCol);
    for (float bz : {-0.96f, 0.96f}) {
        ChassisComponents::AppendClosedCylinderY(mesh.get(), xFront + 0.64f, bz, cabTopY - 0.04f, cabTopY + 0.09f, 0.065f, 12, amberStrobe);
    }

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.04f, cabBaseY + 0.32f, s > 0 ? cabHW - 0.05f : -cabHW), Vec3(cabBackX + 0.28f, cabTopY - 0.10f, s > 0 ? cabHW : -cabHW + 0.05f), bluePaint);
    }

    // 4. MirrorCam Pods + 2-Pane Split Doors aligned to panoramic windshield + Lime-Trimmed Skirts
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.04f, true, false, bluePaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.04f, true, false, bluePaint, glossBlack, sideGlass, doorBeltY);
    AppendMirrorCamWings(mesh.get(), xFront + 0.50f, windTopY - 0.16f, cabHW, chromeCol, glossBlack, chromeCol);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.38f, windTopY - 0.14f, -cabHW, glossBlack);

    float skirtStart = cabBackX + 0.24f;
    float skirtEnd   = std::max(skirtStart + 0.60f, driveCenterX - archHalfLen - 0.10f);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f, -hw + 0.03f), Vec3(skirtEnd, frameTopY + 0.02f, -hw + 0.10f), bluePaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.24f,  hw - 0.10f), Vec3(skirtEnd, frameTopY + 0.02f,  hw - 0.03f), bluePaint);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.26f, -hw + 0.02f), Vec3(skirtEnd, frameBotY - 0.21f, -hw + 0.06f), limeAccent);
    RunningGear::AppendBox(mesh.get(), Vec3(skirtStart, frameBotY - 0.26f,  hw - 0.06f), Vec3(skirtEnd, frameBotY - 0.21f,  hw - 0.02f), limeAccent);
    RunningGear::AppendBox(mesh.get(), Vec3(cabBackX + 0.12f, frameTopY, -0.45f), Vec3(fifthWheelX - 0.66f, frameTopY + 0.04f, 0.45f), alloyCol);

    // 5. Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), windFrontX, windBrowX, windBotY, windTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.04f, true, true, bluePaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.04f, true, true, bluePaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
