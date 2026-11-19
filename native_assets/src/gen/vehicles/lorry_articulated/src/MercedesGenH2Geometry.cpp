#include "BespokeTractors.h"
#include "TruckMeshPrimitives.h"
#include <algorithm>
#include <vector>

namespace sphere::vehicles::trucks {

// Reference: /reference truck designs/Eine-Welt-viele-Loesungen_Buehne.jpg
// Signature features: Carved metallic-silver Actros L GenH2 FCEV cab, panoramic windshield ratio
// aligned to split 2-pane doors, centered 3-Pointed Star across 4 curved brushed-silver V-grille
// bars, glowing curved cyan H2 bar sweeping into carved L-eyebrow corner headlamps, MirrorCam wings,
// 9 vertical sleeper cryo-vent louvers, and massive stainless-steel LH2 cryogenic side & cage tanks.
std::shared_ptr<Mesh> GenerateMercedesGenH2Mesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 5.8f, 9.0f);
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
    const float cabBackX = xFront + 2.48f;
    const float cabTopY = std::clamp(def.cab_height_m, 3.40f, 4.02f);

    const float frontAxleX = xFront + 1.42f;
    const float driveCenterX = std::min(xRear - 0.95f, frontAxleX + def.wheelbase_m);
    const float fifthWheelX = std::clamp(def.fifth_wheel_offset_m, 0.85f, xRear - 0.95f);

    const Vec4 silverPaint = def.base_color;
    const Vec4 h2Cyan = def.accent_color;
    const Vec4 frameDark(0.13f, 0.15f, 0.18f, 1.0f);
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 chromeCol(0.90f, 0.92f, 0.96f, 1.0f);
    const Vec4 stainlessSteel(0.84f, 0.87f, 0.91f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 interiorSeat(0.18f, 0.20f, 0.24f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.15f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);
    const Vec4 ledWhite = def.headlights_on ? Vec4(0.96f, 0.99f, 1.00f, 1.0f) : Vec4(0.76f, 0.82f, 0.88f, 1.0f);

    // 1. Chassis & axles
    AppendTractorChassisBase(
        mesh.get(), def, xFront, xRear, frontAxleX, driveCenterX, fifthWheelX,
        wheelR, tireW, hw, cabHW, frameBotY, frameTopY, archTopY, archHalfLen,
        frameDark, glossBlack, linerDark, stainlessSteel
    );

    const float windFrontX = xFront + 0.14f;
    const float windBrowX  = xFront + 0.34f;
    const float windBotY   = cabClearY + 1.22f;
    const float windTopY   = std::min(cabTopY - 0.52f, windBotY + 0.86f);
    const float doorX0     = xFront + 0.44f;
    const float doorX1     = cabBackX - 0.70f;
    const float doorTopY   = windTopY + 0.05f;
    const float doorBeltY  = windBotY - 0.02f;

    CabComponents::AppendTruckCabInterior(
        mesh.get(), xFront + 0.32f, cabBackX - 0.08f,
        cabClearY + 0.06f, cabTopY - 0.16f, cabHW, true, dashDark, interiorSeat, h2Cyan,
        def.is_rhd, windBotY, def.steer_angle_deg
    );

    // 2. Smoothly Carved Actros L GenH2 Silver Nose & Corners
    AppendCarvedLowerNose(mesh.get(), xFront, xFront + 0.48f, cabBaseY, windBotY, cabHW, silverPaint);

    // Wide Curved 4-Slat Brushed-Silver Actros Grille + Lower Bumper Mouth + Cyan H2 Bar + Star
    AppendVGrilleSlats(mesh.get(), xFront - 0.005f, cabClearY + 0.22f, windBotY - 0.18f, cabHW * 0.54f, cabHW * 0.68f, 4, chromeCol, glossBlack, -1, chromeCol, cabHW, 0.09f);
    AppendVGrilleSlats(mesh.get(), xFront - 0.004f, cabBaseY - 0.04f,  cabClearY + 0.12f, cabHW * 0.44f, cabHW * 0.50f, 2, glossBlack, glossBlack, -1, chromeCol, cabHW, 0.09f);
    AppendCurvedPanelX(mesh.get(), xFront - 0.012f, 0.024f, cabClearY + 0.15f, cabClearY + 0.20f, cabClearY + 0.15f, cabClearY + 0.20f, -cabHW * 0.62f, cabHW * 0.62f, cabHW, 0.09f, 6, h2Cyan, 0.006f);
    AppendMercedesStarX(mesh.get(), xFront - 0.016f, cabClearY + 0.66f, 0.0f, 0.155f, glossBlack, chromeCol, chromeCol);

    // Carved Swept Corner Actros Headlamps with Signature Cyan H2 Upper Eyebrow DRL + Lower Fog Lamp
    for (int side : {-1, 1}) {
        AppendCarvedHeadlampClusterX(
            mesh.get(), xFront - 0.002f, cabHW, 0.09f,
            cabBaseY + 0.10f, cabClearY + 0.34f,
            cabHW * 0.58f, cabHW - 0.08f, side,
            0.035f, 0.015f, glossBlack, ledWhite, h2Cyan, h2Cyan,
            false, true
        );
        float s = static_cast<float>(side);
        float zA = s * (cabHW * 0.60f), zB = s * (cabHW - 0.10f);
        AppendCurvedPanelX(mesh.get(), xFront - 0.004f, 0.065f, cabBaseY - 0.05f, cabBaseY + 0.03f, cabBaseY - 0.05f, cabBaseY + 0.03f,
                           std::min(zA, zB), std::max(zA, zB), cabHW, 0.09f, 3, ledWhite, 0.010f);
    }

    // Lower windshield cowl & wipers
    AppendTruckCowlAndWipersX(mesh.get(), xFront, cabHW, windBotY, glossBlack);

    // 3. Carved Roof, Sleeper, MirrorCam & Cyan Sunvisor
    AppendCarvedRoofAndSleeper(mesh.get(), xFront, cabBackX, doorX1, cabClearY, windBotY, windTopY, cabTopY, cabHW, silverPaint, glossBlack);
    AppendSunvisorBrow(mesh.get(), xFront + 0.30f, xFront + 0.54f, windTopY + 0.02f, windTopY + 0.19f, cabHW, glossBlack, h2Cyan, true);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        for (int lv = 0; lv < 9; ++lv) {
            float ly = cabClearY + 0.78f + static_cast<float>(lv) * 0.105f;
            RunningGear::AppendBox(mesh.get(), Vec3(cabBackX - 0.22f, ly, s > 0 ? cabHW - 0.01f : -cabHW - 0.012f), Vec3(cabBackX - 0.04f, ly + 0.040f, s > 0 ? cabHW + 0.012f : -cabHW + 0.01f), glossBlack);
        }
    }

    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.04f, true, false, silverPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.04f, true, false, silverPaint, glossBlack, sideGlass, doorBeltY);
    AppendMirrorCamWings(mesh.get(), xFront + 0.46f, windTopY - 0.16f, cabHW, chromeCol, glossBlack, h2Cyan);
    AppendFrontKerbMirror(mesh.get(), xFront + 0.36f, windTopY - 0.14f, -cabHW, glossBlack);
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.46f, cabTopY - 0.08f, cabHW * 0.85f, def.headlights_on);

    // 4. Mercedes-Benz GenH2 Exposed Stainless-Steel LH2 Cryogenic Tanks + Rear Cage
    const float cryoX0 = cabBackX + 0.06f;
    const float cryoX1 = std::min(fifthWheelX - 0.62f, cryoX0 + 0.72f);
    RunningGear::AppendBox(mesh.get(), Vec3(cryoX0, frameTopY, -hw + 0.08f), Vec3(cryoX0 + 0.06f, cabTopY - 0.20f, -hw + 0.15f), frameDark);
    RunningGear::AppendBox(mesh.get(), Vec3(cryoX0, frameTopY,  hw - 0.15f), Vec3(cryoX0 + 0.06f, cabTopY - 0.20f,  hw - 0.08f), frameDark);
    RunningGear::AppendBox(mesh.get(), Vec3(cryoX1 - 0.06f, frameTopY, -hw + 0.08f), Vec3(cryoX1, cabTopY - 0.20f, -hw + 0.15f), frameDark);
    RunningGear::AppendBox(mesh.get(), Vec3(cryoX1 - 0.06f, frameTopY,  hw - 0.15f), Vec3(cryoX1, cabTopY - 0.20f,  hw - 0.08f), frameDark);
    RunningGear::AppendBox(mesh.get(), Vec3(cryoX0, cabTopY - 0.24f, -hw + 0.08f), Vec3(cryoX1, cabTopY - 0.18f, hw - 0.08f), frameDark);

    for (float ty : {frameTopY + 0.46f, frameTopY + 1.18f}) {
        ChassisComponents::AppendClosedCylinderX(mesh.get(), cryoX0 + 0.05f, cryoX1 - 0.05f, ty, -0.44f, 0.30f, 20, stainlessSteel, chromeCol, 0.08f);
        ChassisComponents::AppendClosedCylinderX(mesh.get(), cryoX0 + 0.05f, cryoX1 - 0.05f, ty,  0.44f, 0.30f, 20, stainlessSteel, chromeCol, 0.08f);
    }

    float tankStart = std::max(cabBackX + 0.18f, frontAxleX + archHalfLen + 0.12f);
    float tankEnd   = std::max(tankStart + 0.70f, driveCenterX - archHalfLen - 0.12f);
    float tankY = frameBotY + 0.04f;
    for (float sideSign : {-1.0f, 1.0f}) {
        float tz = sideSign * (hw - 0.34f);
        ChassisComponents::AppendClosedCylinderX(mesh.get(), tankStart, tankEnd, tankY, tz, 0.34f, 24, stainlessSteel, chromeCol, 0.12f);
        float spanT = tankEnd - tankStart;
        for (float f : {0.26f, 0.74f}) {
            float bx = tankStart + spanT * f;
            RunningGear::AppendBox(mesh.get(), Vec3(bx - 0.03f, tankY - 0.35f, tz - 0.35f), Vec3(bx + 0.03f, tankY + 0.35f, tz + 0.35f), frameDark);
        }
        RunningGear::AppendBox(mesh.get(), Vec3(tankStart + spanT * 0.40f, tankY - 0.04f, sideSign > 0 ? tz + 0.33f : tz - 0.35f), Vec3(tankStart + spanT * 0.60f, tankY + 0.04f, sideSign > 0 ? tz + 0.355f : tz - 0.325f), h2Cyan);
    }

    // 5. Translucent Glass Pass
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), windFrontX, windBrowX, windBotY, windTopY, cabHW - 0.12f, 0.08f, windshieldGlass);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY, -cabHW, false, def.door_open_ratio, 0.04f, true, true, silverPaint, glossBlack, sideGlass, doorBeltY);
    AppendSteppedCabDoor(mesh.get(), doorX0, doorX1, cabClearY + 0.02f, doorTopY,  cabHW, true,  def.door_open_ratio, 0.04f, true, true, silverPaint, glossBlack, sideGlass, doorBeltY);

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::trucks
