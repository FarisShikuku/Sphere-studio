#include "CityBus.h"
#include "CityBusDesigns.h"
#include "CabComponents.h"
#include "ChassisComponents.h"
#include "RunningGear.h"
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace sphere::vehicles {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::cab::CabComponents;
using sphere::vehicles::cab::HullStationX;
using sphere::vehicles::chassis::ChassisComponents;
using sphere::vehicles::city::CityBusStyle;

namespace {
void AppendClippedBeltRibbon(
    Mesh* mesh,
    float xStart, float xEnd,
    float winBot, float winTop,
    float halfWidth,
    int bays,
    const Vec4& glassCol,
    const Vec4& seamCol,
    bool emitGlassOnly,
    const std::vector<std::pair<float, float>>& rightSkipXRanges
) {
    float span = xEnd - xStart;
    if (span < 0.8f || bays < 1) return;

    auto emitBoxClippedRight = [&](float y0, float y1, float zMin, float zMax, const Vec4& col) {
        std::vector<std::pair<float, float>> sorted = rightSkipXRanges;
        std::sort(sorted.begin(), sorted.end());
        float cur = xStart;
        for (const auto& r : sorted) {
            float r0 = std::max(cur, r.first);
            float r1 = std::min(xEnd, r.second);
            if (r0 > cur + 0.03f) {
                RunningGear::AppendBox(mesh, Vec3(cur, y0, zMin), Vec3(r0, y1, zMax), col);
            }
            cur = std::max(cur, r1);
        }
        if (xEnd > cur + 0.03f) {
            RunningGear::AppendBox(mesh, Vec3(cur, y0, zMin), Vec3(xEnd, y1, zMax), col);
        }
    };

    if (emitGlassOnly) {
        RunningGear::AppendBox(mesh, Vec3(xStart, winBot, -halfWidth - 0.012f), Vec3(xEnd, winTop, -halfWidth + 0.005f), glassCol);
        emitBoxClippedRight(winBot, winTop, halfWidth - 0.005f, halfWidth + 0.012f, glassCol);
    } else {
        RunningGear::AppendBox(mesh, Vec3(xStart, winBot - 0.02f, -halfWidth - 0.014f), Vec3(xEnd, winBot + 0.01f, -halfWidth + 0.02f), seamCol);
        RunningGear::AppendBox(mesh, Vec3(xStart, winTop - 0.01f, -halfWidth - 0.014f), Vec3(xEnd, winTop + 0.02f, -halfWidth + 0.02f), seamCol);
        emitBoxClippedRight(winBot - 0.02f, winBot + 0.01f, halfWidth - 0.02f, halfWidth + 0.014f, seamCol);
        emitBoxClippedRight(winTop - 0.01f, winTop + 0.02f, halfWidth - 0.02f, halfWidth + 0.014f, seamCol);

        for (int i = 1; i < bays; ++i) {
            float px = xStart + (span * static_cast<float>(i)) / static_cast<float>(bays);
            RunningGear::AppendBox(mesh, Vec3(px - 0.016f, winBot, -halfWidth - 0.015f), Vec3(px + 0.016f, winTop, -halfWidth + 0.01f), seamCol);
            bool skipR = false;
            for (const auto& r : rightSkipXRanges) {
                if (px >= r.first - 0.03f && px <= r.second + 0.03f) {
                    skipR = true;
                    break;
                }
            }
            if (!skipR) {
                RunningGear::AppendBox(mesh, Vec3(px - 0.016f, winBot, halfWidth - 0.01f), Vec3(px + 0.016f, winTop, halfWidth + 0.015f), seamCol);
            }
        }
    }
}

void AppendTransitDoubleLeafDoor(
    Mesh* mesh,
    float doorX0, float doorX1,
    float floorY, float topY,
    float sideZ,
    float openRatio,
    bool emitGlassPass,
    const Vec4& paintCol,
    const Vec4& trimCol,
    const Vec4& glassCol
) {
    float mid = (doorX0 + doorX1) * 0.5f;
    float gap = 0.015f;
    float slide = std::clamp(openRatio, 0.0f, 1.0f) * ((doorX1 - doorX0) * 0.42f);
    float zIn  = sideZ - 0.025f;
    float zOut = sideZ + 0.015f + (openRatio > 0.01f ? 0.05f : 0.0f);

    float l0 = doorX0 - slide, l1 = mid - gap - slide;
    float r0 = mid + gap + slide, r1 = doorX1 + slide;

    auto emitLeaf = [&](float x0, float x1) {
        float stile = 0.055f;
        float winBot = floorY + (topY - floorY) * 0.24f;
        float winTop = topY - 0.06f;
        if (!emitGlassPass) {
            RunningGear::AppendBox(mesh, Vec3(x0, floorY, zIn), Vec3(x1, winBot, zOut), paintCol);
            RunningGear::AppendBox(mesh, Vec3(x0, winBot, zIn), Vec3(x0 + stile, topY, zOut), trimCol);
            RunningGear::AppendBox(mesh, Vec3(x1 - stile, winBot, zIn), Vec3(x1, topY, zOut), trimCol);
            RunningGear::AppendBox(mesh, Vec3(x0 + stile, winTop, zIn), Vec3(x1 - stile, topY, zOut), trimCol);
        } else {
            float gz = (zIn + zOut) * 0.5f;
            RunningGear::AppendBox(mesh, Vec3(x0 + stile, winBot, gz - 0.005f), Vec3(x1 - stile, winTop, gz + 0.005f), glassCol);
        }
    };
    emitLeaf(l0, l1);
    emitLeaf(r0, r1);
}
} // namespace

CityBus::CityBus(VehicleID id) : m_Def(CreateCityBusDefaultDef(id)) {}

std::shared_ptr<Mesh> CityBus::GenerateMesh(uint32_t lod) {
    (void)lod;
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(m_Def.dimensions.x, 9.0f, 19.5f);
    const float W = std::clamp(m_Def.dimensions.y, 2.35f, 2.60f);
    const float H = std::clamp(m_Def.dimensions.z, 2.85f, 4.55f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(m_Def.wheel_radius_m, 0.42f, 0.55f);
    const float tireW = std::clamp(m_Def.tire_width_m, 0.25f, 0.34f);

    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = hw - tireW * 1.06f - 0.025f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float floorY = std::clamp(m_Def.ground_clearance_m + 0.04f, 0.24f, 0.46f);
    const float archTopY = wheelR * 2.0f + 0.13f;
    const float archHalfLen = wheelR + 0.11f;

    const Vec4 bodyPaint = m_Def.base_color;
    const Vec4 accent = m_Def.accent_color;
    const Vec4 glossBlack(0.06f, 0.07f, 0.09f, 1.0f);
    const Vec4 darkTrim(0.10f, 0.11f, 0.13f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 chromeCol(0.84f, 0.87f, 0.92f, 1.0f);
    const Vec4 seatCol(0.18f, 0.36f, 0.58f, 1.0f);
    const Vec4 floorCol(0.18f, 0.19f, 0.22f, 1.0f);
    const Vec4 destAmber(0.98f, 0.66f, 0.10f, 1.0f);
    const Vec4 roofWhite(0.90f, 0.92f, 0.95f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);

    const CityBusStyle style = city::ResolveCityStyle(m_Def.id);
    const bool isDoubleDecker = (style == CityBusStyle::DOUBLE_DECKER);
    const bool isBendy        = (style == CityBusStyle::ARTICULATED_BRT);
    const bool isLowFloor     = (style == CityBusStyle::ELECTRIC_LOW_FLOOR);

    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.22f, xRear - 0.18f, floorY, floorY + 0.16f, 0.44f, darkTrim);

    float roofBotY = H - (isLowFloor ? 0.28f : 0.14f);
    float roofTopY = roofBotY - 0.04f;
    float lowerWinTop = isDoubleDecker ? 2.04f : (roofBotY - 0.28f);
    float noseTopY = std::max(floorY + 0.78f, archTopY - 0.02f);
    float xApillar = xFront + 0.48f;
    float winBeltBot = isDoubleDecker
        ? std::max(archTopY + 0.14f, 1.23f)
        : (isLowFloor
            ? std::max(archTopY + 0.08f, floorY + (roofBotY - floorY) * 0.29f)
            : std::max(archTopY + 0.16f, floorY + (roofBotY - floorY) * 0.35f));

    const float d1X0 = xFront + 0.62f;
    const float d1X1 = xFront + 1.56f;
    const float doorTopY = isDoubleDecker ? 2.04f : (roofBotY - 0.28f);

    if (isBendy) {
        const float jointFrontX = xFront + L * 0.57f;
        const float jointRearX  = jointFrontX + 1.35f;
        const float a1 = xFront + 2.35f;
        const float a2 = jointFrontX - 1.35f;
        const float a3 = xRear - 2.35f;
        const float d2X0 = xFront + 5.65f, d2X1 = xFront + 6.70f;
        const float d3X0 = jointRearX + 1.38f, d3X1 = jointRearX + 2.42f;
        const float winTop = lowerWinTop;
        const float xSwoopStart = xFront + 1.05f;
        const float xSwoopEnd = std::min(a1 - archHalfLen - 0.03f, xFront + 1.72f);

        // Axles & Wheel wells
        ChassisComponents::AppendVehicleAxle(mesh.get(), a1, wheelR, tireW, frontHalfTrack, false, m_Def.steer_angle_deg);
        ChassisComponents::AppendVehicleAxle(mesh.get(), a2, wheelR, tireW, rearHalfTrack, true, 0.0f);
        ChassisComponents::AppendVehicleAxle(mesh.get(), a3, wheelR, tireW, rearHalfTrack, true, -m_Def.steer_angle_deg * 0.45f);
        for (size_t idx = 0; idx < 3; ++idx) {
            float ax = (idx == 0) ? a1 : (idx == 1 ? a2 : a3);
            float inZ = (idx == 0) ? (hw - tireW - 0.14f) : (hw - tireW * 2.12f - 0.12f);
            float cutR = wheelR + 0.08f;
            CabComponents::AppendWheelWellLiner(mesh.get(), ax, archHalfLen, floorY, archTopY, hw, inZ, linerDark);
            CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutR, archHalfLen, floorY, winBeltBot, -hw, false, 0.07f, bodyPaint, linerDark);
            CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutR, archHalfLen, floorY, winBeltBot,  hw, true,  0.07f, bodyPaint, linerDark);
            CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutR, -hw, false, 0.032f, darkTrim);
            CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutR,  hw, true,  0.032f, darkTrim);
        }

        // Interiors inside both front & rear articulated hulls
        CabComponents::AppendBusInterior(
            mesh.get(),
            xFront + 0.48f, jointFrontX - 0.08f,
            floorY + 0.04f, winTop - 0.04f,
            hw, d1X0, d1X1, floorY,
            7, true, d2X0, d2X1,
            seatCol, floorCol, darkTrim
        );
        CabComponents::AppendBusInterior(
            mesh.get(),
            jointRearX + 0.08f, xRear - 0.46f,
            floorY + 0.04f, winTop - 0.04f,
            hw, d3X0, d3X1, floorY,
            4, false, 0.0f, 0.0f,
            seatCol, floorCol, darkTrim
        );

        // Smooth C1 compound-curved BRT front nose, A-pillars, dome, arched front/rear roof canopies & tail
        city::AppendCityFrontDesign(
            mesh.get(), style, xFront, xApillar, floorY, noseTopY, winTop, roofTopY, hw,
            m_Def.headlights_on, bodyPaint, accent, glossBlack, darkTrim, chromeCol, destAmber
        );
        city::AppendCityRoofCanopySmooth(mesh.get(), xFront, xFront + 0.96f, jointFrontX, winTop, roofTopY, hw, bodyPaint, false, false);
        city::AppendCityRoofCanopySmooth(mesh.get(), xFront, jointRearX, xRear, winTop, roofTopY, hw, bodyPaint, true, true);

        // Twin longitudinal CNG compressed-gas roof tank pods atop front BRT section + orange side stripes
        for (float cz : {-0.46f, 0.46f}) {
            ChassisComponents::AppendClosedCylinderX(mesh.get(), xFront + 2.20f, jointFrontX - 0.65f, roofBotY + 0.06f, cz, 0.16f, 12, roofWhite, accent, 0.12f);
        }
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 1.85f, roofBotY - 0.02f, -0.74f), Vec3(jointFrontX - 0.45f, roofBotY + 0.08f, -0.68f), accent);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 1.85f, roofBotY - 0.02f,  0.68f), Vec3(jointFrontX - 0.45f, roofBotY + 0.08f,  0.74f), accent);
        // Rear section compact HVAC roof unit
        std::vector<HullStationX> rearHvac = {
            {jointRearX + 0.70f, roofBotY - 0.04f, H - 0.02f, 0.64f, 0.03f, 0.06f, 0.02f, 0.04f},
            {xRear - 1.30f,      roofBotY - 0.04f, H - 0.02f, 0.64f, 0.03f, 0.06f, 0.02f, 0.00f},
        };
        CabComponents::AppendSculptedHullX(mesh.get(), rearHvac, roofWhite, 3);

        std::vector<HullStationX> rearTail = {
            {xRear - 0.44f, floorY,         winTop + 0.02f, hw,         0.08f, 0.08f, 0.0f, 0.0f},
            {xRear,         floorY + 0.08f, winTop + 0.02f, hw - 0.04f, 0.10f, 0.08f, 0.0f, 0.0f},
        };
        CabComponents::AppendSculptedHullX(mesh.get(), rearTail, bodyPaint, 4);

        // Front & rear hull side walls (skipping wheel wells + doors)
        std::vector<std::pair<float, float>> leftSkipFront = {{a1 - archHalfLen, a1 + archHalfLen}, {a2 - archHalfLen, a2 + archHalfLen}};
        std::vector<std::pair<float, float>> rightSkipFront = {{d1X0, d1X1}, {a1 - archHalfLen, a1 + archHalfLen}, {a2 - archHalfLen, a2 + archHalfLen}, {d2X0, d2X1}};
        std::vector<std::pair<float, float>> leftSkipRear = {{a3 - archHalfLen, a3 + archHalfLen}};
        std::vector<std::pair<float, float>> rightSkipRear = {{a3 - archHalfLen, a3 + archHalfLen}, {d3X0, d3X1}};

        CabComponents::AppendSkirtWithCutouts(mesh.get(), xApillar, xSwoopStart, floorY, noseTopY, -hw, false, 0.07f, leftSkipFront, bodyPaint);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), xSwoopStart, xSwoopEnd, floorY, noseTopY - 0.02f, -hw, false, 0.07f, leftSkipFront, bodyPaint);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), xSwoopEnd, jointFrontX - 0.08f, floorY, winBeltBot, -hw, false, 0.07f, leftSkipFront, bodyPaint);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), xApillar, jointFrontX - 0.08f, floorY, winBeltBot,  hw, true,  0.07f, rightSkipFront, bodyPaint);
        CabComponents::AppendSwoopWaistWallSide(mesh.get(), xSwoopStart, xSwoopEnd, noseTopY - 0.03f, noseTopY, winBeltBot, hw, 0.07f, bodyPaint, glossBlack, true);

        CabComponents::AppendSkirtWithCutouts(mesh.get(), jointRearX + 0.06f, xRear - 0.44f, floorY, winBeltBot, -hw, false, 0.07f, leftSkipRear, bodyPaint);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), jointRearX + 0.06f, xRear - 0.44f, floorY, winBeltBot,  hw, true,  0.07f, rightSkipRear, bodyPaint);

        // Bold orange BRT diagonal/belt accents along front & trailer walls
        CabComponents::AppendSkirtWithCutouts(mesh.get(), xSwoopEnd, jointFrontX - 0.10f, winBeltBot - 0.16f, winBeltBot - 0.04f, -hw - 0.009f, false, 0.02f, leftSkipFront, accent);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), xApillar, jointFrontX - 0.10f,  winBeltBot - 0.16f, winBeltBot - 0.04f,  hw + 0.009f, true,  0.02f, rightSkipFront, accent);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), jointRearX + 0.10f, xRear - 0.44f, winBeltBot - 0.16f, winBeltBot - 0.04f, -hw - 0.009f, false, 0.02f, leftSkipRear, accent);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), jointRearX + 0.10f, xRear - 0.44f, winBeltBot - 0.16f, winBeltBot - 0.04f,  hw + 0.009f, true,  0.02f, rightSkipRear, accent);

        // Articulated bellows
        RunningGear::AppendBox(mesh.get(), Vec3(jointFrontX, floorY + 0.08f, -hw + 0.12f), Vec3(jointRearX, roofBotY - 0.22f, hw - 0.12f), Vec4(0.14f, 0.15f, 0.17f, 1.0f));
        for (int b = 0; b < 7; ++b) {
            float bx = jointFrontX + 0.10f + static_cast<float>(b) * 0.185f;
            float zSwing = std::sin(m_Def.action_ratio * 1.57f) * 0.09f * std::sin(static_cast<float>(b) * 0.5f);
            RunningGear::AppendBox(mesh.get(), Vec3(bx - 0.045f, floorY + 0.06f, -hw + 0.06f + zSwing), Vec3(bx + 0.045f, roofBotY - 0.18f, hw - 0.06f + zSwing), darkTrim);
        }

        // Opaque window mullions + openable transit doors
        CabComponents::AppendSwoopingCoachGlassSide(mesh.get(), xApillar, xSwoopEnd, d2X0, noseTopY, winBeltBot, winTop, hw, 3, sideGlass, glossBlack, false, true, d1X0, d1X1, xSwoopStart);
        AppendClippedBeltRibbon(mesh.get(), d2X0, jointFrontX - 0.10f, winBeltBot, winTop, hw, 3, sideGlass, glossBlack, false, {{d2X0, d2X1}});
        AppendClippedBeltRibbon(mesh.get(), jointRearX + 0.10f, xRear - 0.44f, winBeltBot, winTop, hw, 4, sideGlass, glossBlack, false, {{d3X0, d3X1}});
        AppendTransitDoubleLeafDoor(mesh.get(), d1X0, d1X1, floorY + 0.05f, winTop - 0.02f, hw, m_Def.door_open_ratio, false, bodyPaint, darkTrim, sideGlass);
        AppendTransitDoubleLeafDoor(mesh.get(), d2X0, d2X1, floorY + 0.05f, winTop - 0.02f, hw, m_Def.door_open_ratio, false, bodyPaint, darkTrim, sideGlass);
        AppendTransitDoubleLeafDoor(mesh.get(), d3X0, d3X1, floorY + 0.05f, winTop - 0.02f, hw, m_Def.door_open_ratio, false, bodyPaint, darkTrim, sideGlass);

        city::AppendCityMirrorsForStyle(mesh.get(), style, xFront, winTop, hw, bodyPaint, darkTrim);
        city::AppendCityRearDesign(mesh.get(), style, xRear, floorY, winTop, winTop, hw, m_Def.headlights_on, bodyPaint, accent, glossBlack, darkTrim, chromeCol);

        // Glass Pass 2 (LAST)
        city::AppendCityCurvedWindshields(mesh.get(), style, xFront, xApillar, floorY, noseTopY, winTop, 0.0f, 0.0f, roofTopY, hw, windshieldGlass);
        CabComponents::AppendSwoopingCoachGlassSide(mesh.get(), xApillar, xSwoopEnd, d2X0, noseTopY, winBeltBot, winTop, hw, 3, sideGlass, glossBlack, true, false, d1X0, d1X1, xSwoopStart);
        AppendClippedBeltRibbon(mesh.get(), d2X0, jointFrontX - 0.10f, winBeltBot, winTop, hw, 3, sideGlass, glossBlack, true, {{d2X0, d2X1}});
        AppendTransitDoubleLeafDoor(mesh.get(), d1X0, d1X1, floorY + 0.05f, winTop - 0.02f, hw, m_Def.door_open_ratio, true, bodyPaint, darkTrim, sideGlass);
        AppendTransitDoubleLeafDoor(mesh.get(), d2X0, d2X1, floorY + 0.05f, winTop - 0.02f, hw, m_Def.door_open_ratio, true, bodyPaint, darkTrim, sideGlass);
        AppendClippedBeltRibbon(mesh.get(), jointRearX + 0.10f, xRear - 0.44f, winBeltBot, winTop, hw, 4, sideGlass, glossBlack, true, {{d3X0, d3X1}});
        AppendTransitDoubleLeafDoor(mesh.get(), d3X0, d3X1, floorY + 0.05f, winTop - 0.02f, hw, m_Def.door_open_ratio, true, bodyPaint, darkTrim, sideGlass);
    } else {
        const float frontAxleX = xFront + 2.35f;
        const float rearAxleX  = std::min(xRear - 2.10f, frontAxleX + m_Def.wheelbase_m);
        const bool hasTag      = (m_Def.drive_config == VehicleDriveConfig::DRIVE_6X2_TAG);
        const float tagAxleX   = rearAxleX + 1.32f;
        const float d2X0 = -0.18f, d2X1 = 0.88f;
        const float winEndX = xRear - 0.44f;
        const float singleDeckWinTop = lowerWinTop;
        const float upperDeckFloorY = 2.26f;
        const float upperWinBot = 2.58f;
        const float upperWinTop = roofBotY - 0.34f;
        const float topCutoutY = isDoubleDecker ? upperWinTop : singleDeckWinTop;
        const float xSwoopStart = xFront + 1.05f;
        const float xSwoopEnd = std::min(frontAxleX - archHalfLen - 0.03f, xFront + 1.72f);

        // Axles & recessed wheel wells
        ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, frontHalfTrack, false, m_Def.steer_angle_deg);
        ChassisComponents::AppendVehicleAxle(mesh.get(), rearAxleX,  wheelR, tireW, rearHalfTrack,  true,  0.0f);
        auto emitCityWheelBay = [&](float ax, float innerZ) {
            float cutR = wheelR + 0.08f;
            CabComponents::AppendWheelWellLiner(mesh.get(), ax, archHalfLen, floorY, archTopY, hw, innerZ, linerDark);
            CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutR, archHalfLen, floorY, winBeltBot, -hw, false, 0.07f, bodyPaint, linerDark);
            CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutR, archHalfLen, floorY, winBeltBot,  hw, true,  0.07f, bodyPaint, linerDark);
            CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutR, -hw, false, 0.032f, darkTrim);
            CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutR,  hw, true,  0.032f, darkTrim);
        };
        emitCityWheelBay(frontAxleX, hw - tireW - 0.14f);
        emitCityWheelBay(rearAxleX,  hw - tireW * 2.12f - 0.12f);
        if (hasTag) {
            ChassisComponents::AppendVehicleAxle(mesh.get(), tagAxleX, wheelR, tireW, frontHalfTrack, false, -m_Def.steer_angle_deg * 0.3f);
            emitCityWheelBay(tagAxleX, hw - tireW - 0.14f);
        }

        // Full 3D Interior (lower deck + optional upper deck + staircase for Double-Decker)
        CabComponents::AppendBusInterior(
            mesh.get(),
            xFront + 0.48f, winEndX - 0.04f,
            floorY + 0.04f, isDoubleDecker ? 2.12f : (singleDeckWinTop - 0.04f),
            hw, d1X0, d1X1, floorY,
            8, true, d2X0, d2X1,
            seatCol, floorCol, darkTrim
        );
        if (isDoubleDecker) {
            CabComponents::AppendBusInterior(
                mesh.get(),
                xFront + 0.56f, winEndX - 0.04f,
                upperDeckFloorY, upperWinTop - 0.04f,
                hw, xFront + 0.01f, xFront + 0.02f, upperDeckFloorY,
                9, false, 0.0f, 0.0f,
                seatCol, floorCol, darkTrim
            );
            // Interior passenger staircase connecting lower deck up to upper deck (-Z stairwell behind driver bulkhead)
            for (int st = 0; st < 7; ++st) {
                float sx0 = xFront + 2.45f + static_cast<float>(st) * 0.22f;
                float sy = floorY + 0.28f + static_cast<float>(st) * 0.28f;
                RunningGear::AppendBox(mesh.get(), Vec3(sx0, sy - 0.04f, -hw + 0.26f), Vec3(sx0 + 0.22f, sy + 0.02f, -hw + 0.88f), accent);
            }
            RunningGear::AppendBox(mesh.get(), Vec3(xFront + 2.40f, floorY + 0.20f, -hw + 0.86f), Vec3(xFront + 4.02f, upperDeckFloorY + 0.20f, -hw + 0.90f), chromeCol);
        }

        // Smooth C1 compound-curved front nose, dome & continuous arched roof canopy
        city::AppendCityFrontDesign(
            mesh.get(), style, xFront, xApillar, floorY, noseTopY, topCutoutY, roofTopY, hw,
            m_Def.headlights_on, bodyPaint, accent, glossBlack, darkTrim, chromeCol, destAmber
        );
        city::AppendCityRoofCanopySmooth(
            mesh.get(), xFront, xFront + 0.96f, xRear, topCutoutY, roofTopY, hw,
            isLowFloor ? glossBlack : bodyPaint, false, true
        );

        std::vector<HullStationX> rearTail = {
            {winEndX, floorY,         topCutoutY + 0.02f, hw,         0.08f, 0.08f, 0.0f, 0.0f},
            {xRear,   floorY + 0.08f, topCutoutY + 0.02f, hw - 0.04f, 0.10f, 0.08f, 0.0f, 0.0f},
        };
        CabComponents::AppendSculptedHullX(mesh.get(), rearTail, bodyPaint, 4);

        // Skirt & belt walls with real cutouts
        std::vector<std::pair<float, float>> leftSkip = {{frontAxleX - archHalfLen, frontAxleX + archHalfLen}, {rearAxleX - archHalfLen, rearAxleX + archHalfLen}};
        std::vector<std::pair<float, float>> rightSkip = {{d1X0, d1X1}, {frontAxleX - archHalfLen, frontAxleX + archHalfLen}, {d2X0, d2X1}, {rearAxleX - archHalfLen, rearAxleX + archHalfLen}};
        if (hasTag) {
            leftSkip.push_back({tagAxleX - archHalfLen, tagAxleX + archHalfLen});
            rightSkip.push_back({tagAxleX - archHalfLen, tagAxleX + archHalfLen});
        }

        if (isDoubleDecker) {
            CabComponents::AppendSkirtWithCutouts(mesh.get(), xApillar, winEndX, floorY, winBeltBot, -hw, false, 0.07f, leftSkip, bodyPaint);
            CabComponents::AppendSkirtWithCutouts(mesh.get(), xApillar, winEndX, floorY, winBeltBot,  hw, true,  0.07f, rightSkip, bodyPaint);
            RunningGear::AppendBox(mesh.get(), Vec3(xApillar, lowerWinTop, -hw), Vec3(winEndX + 0.02f, upperWinBot, -hw + 0.08f), bodyPaint);
            RunningGear::AppendBox(mesh.get(), Vec3(xApillar, lowerWinTop,  hw - 0.08f), Vec3(winEndX + 0.02f, upperWinBot,  hw), bodyPaint);
            RunningGear::AppendBox(mesh.get(), Vec3(xApillar + 0.08f, 2.14f, -hw - 0.012f), Vec3(xRear - 0.22f, 2.40f, -hw + 0.012f), accent);
            RunningGear::AppendBox(mesh.get(), Vec3(xApillar + 0.08f, 2.14f,  hw - 0.012f), Vec3(xRear - 0.22f, 2.40f,  hw + 0.012f), accent);

            AppendClippedBeltRibbon(mesh.get(), xFront + 0.56f, winEndX, winBeltBot, lowerWinTop, hw, 6, sideGlass, glossBlack, false, {{d1X0, d1X1}, {d2X0, d2X1}});
            AppendClippedBeltRibbon(mesh.get(), xFront + 0.50f, winEndX, upperWinBot, upperWinTop, hw, 7, sideGlass, glossBlack, false, {});
        } else {
            CabComponents::AppendSkirtWithCutouts(mesh.get(), xApillar, xSwoopStart, floorY, noseTopY, -hw, false, 0.07f, leftSkip, bodyPaint);
            CabComponents::AppendSkirtWithCutouts(mesh.get(), xSwoopStart, xSwoopEnd, floorY, noseTopY - 0.02f, -hw, false, 0.07f, leftSkip, bodyPaint);
            CabComponents::AppendSkirtWithCutouts(mesh.get(), xSwoopEnd, winEndX, floorY, winBeltBot, -hw, false, 0.07f, leftSkip, bodyPaint);
            CabComponents::AppendSkirtWithCutouts(mesh.get(), xApillar, winEndX, floorY, winBeltBot,  hw, true,  0.07f, rightSkip, bodyPaint);
            CabComponents::AppendSwoopWaistWallSide(mesh.get(), xSwoopStart, xSwoopEnd, noseTopY - 0.03f, noseTopY, winBeltBot, hw, 0.07f, bodyPaint, glossBlack, true);

            CabComponents::AppendSkirtWithCutouts(mesh.get(), xSwoopEnd, winEndX, winBeltBot - 0.20f, winBeltBot - 0.04f, -hw - 0.009f, false, 0.02f, leftSkip, accent);
            CabComponents::AppendSkirtWithCutouts(mesh.get(), xApillar, winEndX, winBeltBot - 0.20f, winBeltBot - 0.04f,  hw + 0.009f, true,  0.02f, rightSkip, accent);

            CabComponents::AppendSwoopingCoachGlassSide(mesh.get(), xApillar, xSwoopEnd, d2X0, noseTopY, winBeltBot, singleDeckWinTop, hw, 3, sideGlass, glossBlack, false, true, d1X0, d1X1, xSwoopStart);
            AppendClippedBeltRibbon(mesh.get(), d2X0, winEndX, winBeltBot, singleDeckWinTop, hw, 4, sideGlass, glossBlack, false, {{d2X0, d2X1}});
        }

        // Openable twin-leaf sliding boarding & exit doors (opaque frame pass)
        AppendTransitDoubleLeafDoor(mesh.get(), d1X0, d1X1, floorY + 0.05f, doorTopY, hw, m_Def.door_open_ratio, false, bodyPaint, darkTrim, sideGlass);
        AppendTransitDoubleLeafDoor(mesh.get(), d2X0, d2X1, floorY + 0.05f, doorTopY, hw, m_Def.door_open_ratio, false, bodyPaint, darkTrim, sideGlass);

        // Model-specific roof & wheel-arch equipment
        if (isLowFloor) {
            std::vector<HullStationX> eRoof = {
                {xFront + 0.90f, roofBotY - 0.04f, H - 0.06f, hw - 0.28f, 0.04f, 0.10f, 0.03f, 0.10f},
                {xFront + 1.55f, roofBotY - 0.04f, H - 0.02f, hw - 0.20f, 0.04f, 0.12f, 0.04f, 0.02f},
                {xRear  - 1.45f, roofBotY - 0.04f, H - 0.02f, hw - 0.20f, 0.04f, 0.12f, 0.04f, 0.00f},
                {xRear  - 0.85f, roofBotY - 0.04f, H - 0.10f, hw - 0.28f, 0.04f, 0.08f, 0.02f, 0.00f},
            };
            CabComponents::AppendSculptedHullX(mesh.get(), eRoof, roofWhite, 3);
            RunningGear::AppendBox(mesh.get(), Vec3(xFront + 1.45f, roofBotY + 0.02f, -hw + 0.19f), Vec3(xRear - 1.35f, H - 0.09f, hw - 0.19f), accent);
            // Glowing e-BRT battery status LED bar + flush aerodynamic rear wheel spats
            const Vec4 evCyan(0.18f, 0.90f, 0.94f, 1.0f);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                RunningGear::AppendBox(mesh.get(), Vec3(xFront + 1.85f, H - 0.09f, s > 0 ? hw - 0.21f : -hw + 0.17f), Vec3(xRear - 1.65f, H - 0.05f, s > 0 ? hw - 0.17f : -hw + 0.21f), evCyan);
                RunningGear::AppendBox(mesh.get(), Vec3(rearAxleX - archHalfLen + 0.04f, archTopY - 0.26f, s > 0 ? hw - 0.01f : -hw - 0.02f), Vec3(rearAxleX + archHalfLen - 0.04f, archTopY + 0.02f, s > 0 ? hw + 0.02f : -hw + 0.01f), bodyPaint);
            }
        } else if (!isDoubleDecker) {
            // Standard 12m transit: rooftop white HVAC pod + curbside route board box near front door
            std::vector<HullStationX> hvacPod = {
                {-1.5f, roofBotY - 0.04f, H + 0.04f, 0.68f, 0.03f, 0.06f, 0.02f, 0.05f},
                {-0.9f, roofBotY - 0.04f, H + 0.08f, 0.76f, 0.03f, 0.08f, 0.02f, 0.00f},
                { 1.1f, roofBotY - 0.04f, H + 0.08f, 0.76f, 0.03f, 0.08f, 0.02f, 0.00f},
                { 1.6f, roofBotY - 0.04f, H + 0.03f, 0.66f, 0.03f, 0.05f, 0.01f, 0.00f},
            };
            CabComponents::AppendSculptedHullX(mesh.get(), hvacPod, roofWhite, 3);
            RunningGear::AppendBox(mesh.get(), Vec3(d1X1 + 0.20f, winBeltBot - 0.22f, hw), Vec3(d1X1 + 0.92f, winBeltBot - 0.06f, hw + 0.02f), glossBlack);
            RunningGear::AppendBox(mesh.get(), Vec3(d1X1 + 0.24f, winBeltBot - 0.19f, hw + 0.01f), Vec3(d1X1 + 0.88f, winBeltBot - 0.09f, hw + 0.025f), destAmber);
        }

        city::AppendCityMirrorsForStyle(mesh.get(), style, xFront, topCutoutY, hw, bodyPaint, darkTrim);
        city::AppendCityRearDesign(mesh.get(), style, xRear, floorY, lowerWinTop, upperWinTop, hw, m_Def.headlights_on, bodyPaint, accent, glossBlack, darkTrim, chromeCol);

        // Glass Pass 2 (DRAWN LAST)
        city::AppendCityCurvedWindshields(
            mesh.get(), style, xFront, xApillar, floorY, noseTopY, lowerWinTop, upperWinBot, upperWinTop, roofTopY, hw, windshieldGlass
        );
        if (isDoubleDecker) {
            AppendClippedBeltRibbon(mesh.get(), xFront + 0.56f, winEndX, winBeltBot, lowerWinTop, hw, 6, sideGlass, glossBlack, true, {{d1X0, d1X1}, {d2X0, d2X1}});
            AppendClippedBeltRibbon(mesh.get(), xFront + 0.50f, winEndX, upperWinBot, upperWinTop, hw, 7, sideGlass, glossBlack, true, {});
        } else {
            CabComponents::AppendSwoopingCoachGlassSide(mesh.get(), xApillar, xSwoopEnd, d2X0, noseTopY, winBeltBot, singleDeckWinTop, hw, 3, sideGlass, glossBlack, true, false, d1X0, d1X1, xSwoopStart);
            AppendClippedBeltRibbon(mesh.get(), d2X0, winEndX, winBeltBot, singleDeckWinTop, hw, 4, sideGlass, glossBlack, true, {{d2X0, d2X1}});
        }
        AppendTransitDoubleLeafDoor(mesh.get(), d1X0, d1X1, floorY + 0.05f, doorTopY, hw, m_Def.door_open_ratio, true, bodyPaint, darkTrim, sideGlass);
        AppendTransitDoubleLeafDoor(mesh.get(), d2X0, d2X1, floorY + 0.05f, doorTopY, hw, m_Def.door_open_ratio, true, bodyPaint, darkTrim, sideGlass);
    }

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

bool CityBus::GenerateTextures(VkDevice, VkQueue, VkCommandPool) {
    m_Generated = true;
    return true;
}

} // namespace sphere::vehicles
