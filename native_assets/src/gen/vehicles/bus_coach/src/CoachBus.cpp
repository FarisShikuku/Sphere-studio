#include "CoachBus.h"
#include "CoachFrontDesigns.h"
#include "ExecutiveCoachGeometry.h"
#include "VolvoDoubleDeckerGeometry.h"
#include "LuxuryCoachGeometry.h"
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
using sphere::vehicles::coach::CoachDesignStyle;

CoachBus::CoachBus(VehicleID id) : m_Def(CreateCoachBusDefaultDef(id)) {}

std::shared_ptr<Mesh> CoachBus::GenerateMesh(uint32_t lod) {
    (void)lod;
    if (m_Def.id == VehicleID::COACH_EXECUTIVE_I6) {
        auto mesh = coach::GenerateExecutiveCoachMesh(m_Def);
        m_Bounds = mesh->bounds;
        m_CachedMesh = mesh;
        return mesh;
    }
    if (m_Def.id == VehicleID::COACH_VOLVO_9800_DD || m_Def.id == VehicleID::COACH_VOLVO_9700_DD) {
        auto mesh = coach::GenerateVolvoDoubleDeckerMesh(m_Def);
        m_Bounds = mesh->bounds;
        m_CachedMesh = mesh;
        return mesh;
    }
    if (m_Def.id == VehicleID::COACH_LUXURY_VIP_4X2) {
        auto mesh = coach::GenerateLuxuryCoachMesh(m_Def);
        m_Bounds = mesh->bounds;
        m_CachedMesh = mesh;
        return mesh;
    }
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(m_Def.dimensions.x, 10.5f, 15.0f);
    const float W = std::clamp(m_Def.dimensions.y, 2.40f, 2.60f);
    const float H = std::clamp(m_Def.dimensions.z, 3.25f, 4.10f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(m_Def.wheel_radius_m, 0.45f, 0.56f);
    const float tireW = std::clamp(m_Def.tire_width_m, 0.27f, 0.35f);

    // Accurate track math so outer steer tire and outer dual drive tire sit cleanly inside wheel arches
    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = hw - tireW * 1.06f - 0.025f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float skirtBotY = std::clamp(m_Def.ground_clearance_m, 0.22f, 0.38f);
    const float deckFloorY = std::max(skirtBotY + 1.12f, wheelR * 2.0f + 0.34f);
    const float archTopY = wheelR * 2.0f + 0.14f;
    const float archHalfLen = wheelR + 0.13f;

    const Vec4 bodyPaint = m_Def.base_color;
    const Vec4 accent = m_Def.accent_color;
    const Vec4 glossBlack(0.06f, 0.07f, 0.09f, 1.0f);
    const Vec4 darkTrim(0.10f, 0.11f, 0.13f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 interiorSeat(0.18f, 0.24f, 0.34f, 1.0f);
    const Vec4 interiorFloor(0.16f, 0.17f, 0.20f, 1.0f);
    const Vec4 chromeCol(0.84f, 0.87f, 0.92f, 1.0f);
    const Vec4 copperStripe(0.84f, 0.44f, 0.18f, 1.0f);
    const Vec4 ledDisplay(0.92f, 0.96f, 1.00f, 1.0f);
    const Vec4 amberMarker(1.00f, 0.58f, 0.08f, 1.0f);
    const Vec4 bayDoorCol(
        std::min(1.0f, bodyPaint.x * 0.93f + 0.02f),
        std::min(1.0f, bodyPaint.y * 0.93f + 0.02f),
        std::min(1.0f, bodyPaint.z * 0.93f + 0.02f),
        1.0f
    );
    // Translucent automotive glass rendered last over opaque interior & cockpit
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);

    const CoachDesignStyle style = coach::ResolveCoachStyle(m_Def.id);
    const bool isUnvi       = (style == CoachDesignStyle::UNVI_HIGH_DECKER);
    const bool is9900       = (style == CoachDesignStyle::VOLVO_9900_THEATER);
    const bool isBzr        = (style == CoachDesignStyle::VOLVO_BZR_ELECTRIC);
    const bool is9700       = (style == CoachDesignStyle::VOLVO_9700_USCAN);
    const bool is9700DD     = (style == CoachDesignStyle::VOLVO_9700_DD);
    const bool is9800DD     = (style == CoachDesignStyle::VOLVO_9800_DD);
    const bool isDoubleDeck = (is9700DD || is9800DD);
    const bool is9700Teal   = (style == CoachDesignStyle::VOLVO_9700_TEAL);
    const bool is9800Aero   = (style == CoachDesignStyle::VOLVO_9800_AERO);
    const bool isYutong     = (style == CoachDesignStyle::YUTONG_TOURING);
    const bool isAeroBlade  = (style == CoachDesignStyle::AERO_BLADE_VIP);
    const bool isChampagne  = (style == CoachDesignStyle::CHAMPAGNE_SR2);
    const bool isNeoplan    = (style == CoachDesignStyle::NEOPLAN_TOURLINER);
    const bool isTwinDome   = (style == CoachDesignStyle::TWIN_GLASS_DOME);
    const bool isObsidian   = (style == CoachDesignStyle::OBSIDIAN_VIP_6X2);
    const bool isExecutiveI6= (style == CoachDesignStyle::EXECUTIVE_COACH_I6);

    // 1. Structural frame rails & axle geometry
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.35f, xRear - 0.22f, skirtBotY + 0.06f, skirtBotY + 0.24f, 0.45f, darkTrim);

    const float frontAxleX = xFront + 2.48f;
    const float driveAxleX = std::min(xRear - 2.35f, frontAxleX + m_Def.wheelbase_m);
    const bool hasTagAxle = (m_Def.drive_config == VehicleDriveConfig::DRIVE_6X2_TAG);
    const float tagAxleX = driveAxleX + 1.38f;

    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, frontHalfTrack, false, m_Def.steer_angle_deg);
    ChassisComponents::AppendVehicleAxle(mesh.get(), driveAxleX, wheelR, tireW, rearHalfTrack, true, 0.0f);
    if (hasTagAxle) {
        ChassisComponents::AppendVehicleAxle(mesh.get(), tagAxleX, wheelR, tireW, frontHalfTrack, false, -m_Def.steer_angle_deg * 0.35f);
    }

    // Recessed semicircular wheel-well pockets behind tires, painted semicircular arch spandrels & outer flared arch eyebrows
    auto emitCoachWheelBay = [&](float ax, float innerZ) {
        const float cutR = wheelR + 0.085f;
        CabComponents::AppendWheelWellLiner(mesh.get(), ax, archHalfLen, skirtBotY, archTopY, hw, innerZ, linerDark);
        CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutR, archHalfLen, skirtBotY, archTopY, -hw, false, 0.07f, bodyPaint, linerDark);
        CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutR, archHalfLen, skirtBotY, archTopY,  hw, true,  0.07f, bodyPaint, linerDark);
        CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutR, -hw, false, 0.036f, darkTrim);
        CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutR,  hw, true,  0.036f, darkTrim);
    };
    emitCoachWheelBay(frontAxleX, hw - tireW - 0.16f);
    emitCoachWheelBay(driveAxleX, hw - tireW * 2.15f - 0.14f);
    if (hasTagAxle) {
        emitCoachWheelBay(tagAxleX, hw - tireW - 0.16f);
    }

    // Key body vertical landmarks calibrated per exact coach model (real-world ~1.0m side glass height ratio)
    const float entryFrontX = xFront + 0.72f;
    const float entryBackX  = xFront + 1.54f;
    const float winEndX     = xRear - 0.58f;
    const float noseTopY    = std::max(archTopY + 0.14f, skirtBotY + (isDoubleDeck ? 0.94f : 1.16f));
    const float xApillar    = xFront + 0.62f;
    const float xSwoopStart = std::max(entryBackX + 0.10f, frontAxleX - 0.12f);
    const float xSwoopEnd   = std::min(winEndX - 0.80f, frontAxleX + 1.35f);

    const float lowerDeckFloorY = isDoubleDeck ? (skirtBotY + 0.18f) : deckFloorY;
    const float winSideTop      = isDoubleDeck ? (H - 0.30f) : (H - 0.42f);
    const float lowerWinTop     = isDoubleDeck ? (skirtBotY + 1.94f) : winSideTop;
    const float upperDeckFloorY = isDoubleDeck ? (lowerWinTop + 0.28f) : deckFloorY;
    const float upperWinBot     = isDoubleDeck ? (upperDeckFloorY + 0.36f) : 0.0f;

    const float winMainBot = isDoubleDeck
        ? std::max(archTopY + 0.18f, lowerDeckFloorY + 0.96f)
        : (is9900
            ? std::max(deckFloorY + 0.78f, skirtBotY + (H - skirtBotY) * 0.55f)
            : std::max(deckFloorY + 0.90f, skirtBotY + (H - skirtBotY) * 0.59f));
    const float winRearBot = is9900 ? (winMainBot + 0.24f) : -1.0f;
    const float frontWinLowBot = (isUnvi || is9700Teal || is9800Aero || isExecutiveI6)
        ? std::max(archTopY + 0.06f, noseTopY - (isExecutiveI6 ? 0.18f : 0.14f))
        : (noseTopY - 0.02f);
    const float windshieldTopY = isDoubleDeck ? (H - 0.28f) : (winSideTop + 0.02f);

    // 2. Full 3D Interior (Driver cockpit, stairwell, stanchions, 2+2 seats; dual decks for 9700DD / 9800DD)
    // Safely inset behind curved A-pillars and roof dome corners so interior geometry never protrudes
    CabComponents::AppendBusInterior(
        mesh.get(),
        xFront + 0.80f, winEndX - 0.04f,
        lowerDeckFloorY, isDoubleDeck ? (lowerWinTop - 0.02f) : (winSideTop - 0.08f),
        hw - 0.04f,
        entryFrontX, entryBackX,
        skirtBotY + 0.10f,
        hasTagAxle ? 12 : 11,
        false, 0.0f, 0.0f,
        interiorSeat, interiorFloor, darkTrim
    );

    if (isDoubleDeck) {
        // Upper passenger deck floor, aisle, 12 seat rows visible through upper windshield/ribbon, and inset ceiling
        float inHW = hw - 0.10f;
        float ceilHW = hw - 0.18f;
        Vec4 upCeiling(0.86f, 0.88f, 0.92f, 1.0f);
        Vec4 headrestCol(0.84f, 0.86f, 0.90f, 1.0f);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.92f, upperDeckFloorY - 0.06f, -inHW), Vec3(winEndX - 0.04f, upperDeckFloorY, inHW), interiorFloor);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 1.00f, upperDeckFloorY, -0.22f), Vec3(winEndX - 0.10f, upperDeckFloorY + 0.015f, 0.22f), Vec4(0.46f, 0.50f, 0.56f, 1.0f));
        int upRows = 12;
        float upStart = xFront + 1.12f;
        float upEnd = winEndX - 0.42f;
        float pitch = (upEnd - upStart) / static_cast<float>(upRows);
        for (int r = 0; r < upRows; ++r) {
            float rx = upStart + (static_cast<float>(r) + 0.5f) * pitch;
            CabComponents::AppendSeat(mesh.get(), Vec3(rx, upperDeckFloorY, -inHW + 0.24f), -1.0f, 0.40f, 0.86f, interiorSeat, headrestCol, darkTrim);
            CabComponents::AppendSeat(mesh.get(), Vec3(rx, upperDeckFloorY, -inHW + 0.68f), -1.0f, 0.40f, 0.86f, interiorSeat, headrestCol, darkTrim);
            CabComponents::AppendSeat(mesh.get(), Vec3(rx, upperDeckFloorY,  inHW - 0.68f), -1.0f, 0.40f, 0.86f, interiorSeat, headrestCol, darkTrim);
            CabComponents::AppendSeat(mesh.get(), Vec3(rx, upperDeckFloorY,  inHW - 0.24f), -1.0f, 0.40f, 0.86f, interiorSeat, headrestCol, darkTrim);
        }
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 1.30f, H - 0.22f, -ceilHW), Vec3(winEndX - 0.04f, H - 0.17f, ceilHW), upCeiling);
        RunningGear::AppendBox(mesh.get(), Vec3(winEndX - 0.08f, upperDeckFloorY, -inHW), Vec3(winEndX - 0.02f, H - 0.22f, inHW), upCeiling);
    }

    // Underfloor luggage compartment interior enclosure (single-deck coaches)
    float bayStartX = frontAxleX + archHalfLen + 0.12f;
    float bayEndX   = driveAxleX - archHalfLen - 0.12f;
    if (!isDoubleDeck && bayEndX > bayStartX + 0.5f) {
        RunningGear::AppendBox(
            mesh.get(),
            Vec3(bayStartX, skirtBotY + 0.10f, -hw + 0.14f),
            Vec3(bayEndX,   deckFloorY - 0.06f, hw - 0.14f),
            linerDark
        );
    }

    // 3. Exterior Compound-Curved Front Fascia, Lamps, Grille, Emblem, Roof Dome & Corner A-Pillars (Pass 1)
    coach::AppendCoachFrontDesign(
        mesh.get(),
        style,
        xFront, xApillar,
        skirtBotY, noseTopY, windshieldTopY, winSideTop, H, hw,
        m_Def.headlights_on,
        bodyPaint, accent, glossBlack, darkTrim, chromeCol, ledDisplay
    );

    // 3B. Smooth corner drop bridging curved front nose cheek into dropped side glass / curb quarter window
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        if (isUnvi || is9700Teal || is9800Aero || is9700DD || is9800DD || isExecutiveI6) {
            float zWrap = s * (hw + 0.006f);
            float lowDrop = isDoubleDeck ? (noseTopY - 0.14f) : frontWinLowBot;
            if (side > 0) {
                CabComponents::AppendQuad(mesh.get(), Vec3(xApillar - 0.04f, noseTopY - 0.01f, zWrap), Vec3(xApillar + 0.06f, lowDrop, zWrap), Vec3(xApillar + 0.06f, noseTopY + 0.02f, zWrap), Vec3(xApillar - 0.04f, noseTopY + 0.02f, zWrap), glossBlack);
            } else {
                CabComponents::AppendQuad(mesh.get(), Vec3(xApillar + 0.06f, lowDrop, zWrap), Vec3(xApillar - 0.04f, noseTopY - 0.01f, zWrap), Vec3(xApillar - 0.04f, noseTopY + 0.02f, zWrap), Vec3(xApillar + 0.06f, noseTopY + 0.02f, zWrap), glossBlack);
            }
        }
    }

    // 3C. Smooth Arched Roof Canopy (exact seam with front roof dome) & Exact Coach Roof Pod / Battery Fairing
    const Vec4 roofColor = is9700 ? accent
        : (is9900 ? darkTrim
        : (isObsidian ? glossBlack : bodyPaint));
    coach::AppendCoachRoofCanopySmooth(mesh.get(), xFront, winEndX, xRear, winSideTop, H, hw, roofColor, isDoubleDeck);

    if (isBzr || is9800Aero) {
        // Sculpted full-length rooftop aero fairing in body paint
        std::vector<HullStationX> evRoofFairing = {
            {xFront + 1.75f, H - 0.06f, H + 0.05f, hw - 0.24f, 0.04f, 0.12f, 0.04f, 0.12f},
            {xFront + 2.65f, H - 0.06f, H + 0.18f, hw - 0.16f, 0.04f, 0.14f, 0.05f, 0.04f},
            {xRear - 1.75f,  H - 0.06f, H + 0.17f, hw - 0.16f, 0.04f, 0.14f, 0.05f, 0.00f},
            {xRear - 0.95f,  H - 0.06f, H + 0.04f, hw - 0.26f, 0.04f, 0.10f, 0.03f, 0.00f},
        };
        CabComponents::AppendSculptedHullX(mesh.get(), evRoofFairing, bodyPaint, 4);
        if (isBzr) {
            // BZR Electric glowing cyan/emerald battery charge status bars along upper fairing
            const Vec4 evChargeCol(0.16f, 0.86f, 0.82f, 1.0f);
            RunningGear::AppendBox(mesh.get(), Vec3(xFront + 2.75f, H + 0.06f, -hw + 0.15f), Vec3(xRear - 1.85f, H + 0.11f, -hw + 0.18f), evChargeCol);
            RunningGear::AppendBox(mesh.get(), Vec3(xFront + 2.75f, H + 0.06f,  hw - 0.18f), Vec3(xRear - 1.85f, H + 0.11f,  hw - 0.15f), evChargeCol);
        } else {
            // 9800 Aero twin black shark-fin telematics antennas along roof spine
            for (float antX : {xFront + 2.15f, xRear - 1.35f}) {
                RunningGear::AppendBox(mesh.get(), Vec3(antX - 0.12f, H + 0.02f, -0.025f), Vec3(antX + 0.10f, H + 0.18f, 0.025f), glossBlack);
            }
        }
    } else if (isExecutiveI6) {
        // Irizar i6 signature tall aerodynamic full-roof contoured HVAC shell/dome blending forward into brow
        std::vector<HullStationX> execRoofPod = {
            {xFront + 1.08f, H - 0.08f, H + 0.08f, hw - 0.28f, 0.04f, 0.14f, 0.05f, 0.10f},
            {xFront + 2.20f, H - 0.06f, H + 0.20f, hw - 0.18f, 0.04f, 0.18f, 0.06f, 0.04f},
            {xRear - 2.00f,  H - 0.06f, H + 0.18f, hw - 0.18f, 0.04f, 0.16f, 0.05f, 0.00f},
            {xRear - 1.10f,  H - 0.08f, H + 0.05f, hw - 0.28f, 0.04f, 0.12f, 0.03f, 0.00f},
        };
        CabComponents::AppendSculptedHullX(mesh.get(), execRoofPod, bodyPaint, 4);
    } else if (!isDoubleDeck) {
        std::vector<HullStationX> acPod = {
            {xRear - 5.2f, H - 0.04f, H + 0.11f, 0.72f, 0.04f, 0.08f, 0.02f, 0.08f},
            {xRear - 4.4f, H - 0.04f, H + 0.16f, 0.84f, 0.04f, 0.09f, 0.03f, 0.02f},
            {xRear - 2.1f, H - 0.04f, H + 0.15f, 0.82f, 0.04f, 0.09f, 0.03f, 0.00f},
            {xRear - 1.4f, H - 0.04f, H + 0.06f, 0.68f, 0.04f, 0.06f, 0.01f, 0.00f},
        };
        const Vec4 podColor = is9700 ? accent
            : (is9900 || is9700Teal || isObsidian ? darkTrim
            : (isAeroBlade ? accent : bodyPaint));
        CabComponents::AppendSculptedHullX(mesh.get(), acPod, podColor, 3);
        if (isAeroBlade) {
            // Concept amber illuminated side aero blades running along both roof rails
            RunningGear::AppendBox(mesh.get(), Vec3(xFront + 1.80f, H - 0.06f, -hw + 0.12f), Vec3(xRear - 0.70f, H + 0.01f, -hw + 0.18f), accent);
            RunningGear::AppendBox(mesh.get(), Vec3(xFront + 1.80f, H - 0.06f,  hw - 0.18f), Vec3(xRear - 0.70f, H + 0.01f,  hw - 0.12f), accent);
        } else if (is9700) {
            // Twin North American emergency roof escape hatches
            for (float hx : {xFront + 3.60f, xRear - 3.60f}) {
                RunningGear::AppendBox(mesh.get(), Vec3(hx - 0.42f, H - 0.02f, -0.44f), Vec3(hx + 0.42f, H + 0.06f, 0.44f), accent);
            }
        }
    } else if (is9800DD) {
        // 9800DD upper deck twin panoramic roof hatches
        for (float hx : {xFront + 3.80f, xRear - 3.40f}) {
            RunningGear::AppendBox(mesh.get(), Vec3(hx - 0.40f, H - 0.02f, -0.42f), Vec3(hx + 0.40f, H + 0.05f, 0.42f), bodyPaint);
        }
    }

    // 3D. Sculpted Rear D-Pillar Engine Tail Enclosure [winEndX .. xRear]
    std::vector<HullStationX> rearTail = {
        {winEndX, skirtBotY + 0.02f, winSideTop + 0.02f, hw,         0.10f, 0.12f, 0.02f, 0.0f},
        {xRear,   skirtBotY + 0.12f, winSideTop + 0.02f, hw - 0.05f, 0.12f, 0.12f, 0.02f, 0.0f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), rearTail, bodyPaint, 4);

    // 3E. Lower Side Skirts & Waist Beltline Walls (with real wheel-well and +Z door cutouts)
    std::vector<std::pair<float, float>> leftSkipSkirt = {
        {frontAxleX - archHalfLen, frontAxleX + archHalfLen},
        {driveAxleX - archHalfLen, driveAxleX + archHalfLen},
    };
    std::vector<std::pair<float, float>> rightSkipSkirt = {
        {entryFrontX, entryBackX},
        {frontAxleX - archHalfLen, frontAxleX + archHalfLen},
        {driveAxleX - archHalfLen, driveAxleX + archHalfLen},
    };
    if (hasTagAxle) {
        leftSkipSkirt.push_back({tagAxleX - archHalfLen, tagAxleX + archHalfLen});
        rightSkipSkirt.push_back({tagAxleX - archHalfLen, tagAxleX + archHalfLen});
    }

    CabComponents::AppendSkirtWithCutouts(mesh.get(), xApillar, winEndX, skirtBotY, archTopY,  hw, true,  0.07f, rightSkipSkirt, bodyPaint);
    CabComponents::AppendSkirtWithCutouts(mesh.get(), xApillar, winEndX, skirtBotY, archTopY, -hw, false, 0.07f, leftSkipSkirt, bodyPaint);

    // Narrow door jamb ahead of front entry door
    RunningGear::AppendBox(mesh.get(), Vec3(xApillar, frontWinLowBot - 0.04f, hw - 0.07f), Vec3(entryFrontX, lowerWinTop, hw), glossBlack);

    {
        // --- SINGLE-DECK OVER-12M WAIST WALLS, SWOOPS & MODEL-SPECIFIC SIDE SIGNATURES ---
        CabComponents::AppendSkirtWithCutouts(mesh.get(), xApillar, xSwoopStart, archTopY - 0.01f, frontWinLowBot, -hw, false, 0.07f, {}, bodyPaint);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), xApillar, entryFrontX, archTopY - 0.01f, frontWinLowBot,  hw, true,  0.07f, {}, bodyPaint);
        CabComponents::AppendSkirtWithCutouts(mesh.get(), entryBackX, xSwoopStart, archTopY - 0.01f, frontWinLowBot, hw, true,  0.07f, {}, bodyPaint);

        const Vec4 swoopTrimCol = is9900 ? accent
            : (is9700 || is9700Teal || is9800Aero ? darkTrim
            : (isNeoplan ? glossBlack : chromeCol));
        CabComponents::AppendSwoopWaistWallSide(
            mesh.get(),
            xSwoopStart, xSwoopEnd,
            archTopY - 0.01f, frontWinLowBot, winMainBot,
            hw, 0.07f, bodyPaint, swoopTrimCol, false
        );

        if (is9900) {
            // Ascending theater-floor sloped side wall rising from winMainBot to winRearBot
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                float zOut = s * hw;
                float zIn  = s * (hw - 0.07f);
                float yA = archTopY - 0.01f;
                if (side > 0) {
                    CabComponents::AppendQuad(mesh.get(), Vec3(xSwoopEnd, yA, zOut), Vec3(winEndX, yA, zOut), Vec3(winEndX, winRearBot, zOut), Vec3(xSwoopEnd, winMainBot, zOut), bodyPaint);
                    CabComponents::AppendQuad(mesh.get(), Vec3(winEndX, yA, zIn), Vec3(xSwoopEnd, yA, zIn), Vec3(xSwoopEnd, winMainBot, zIn), Vec3(winEndX, winRearBot, zIn), darkTrim);
                } else {
                    CabComponents::AppendQuad(mesh.get(), Vec3(winEndX, yA, zOut), Vec3(xSwoopEnd, yA, zOut), Vec3(xSwoopEnd, winMainBot, zOut), Vec3(winEndX, winRearBot, zOut), bodyPaint);
                    CabComponents::AppendQuad(mesh.get(), Vec3(xSwoopEnd, yA, zIn), Vec3(winEndX, yA, zIn), Vec3(winEndX, winRearBot, zIn), Vec3(xSwoopEnd, winMainBot, zIn), darkTrim);
                }
                float zWave = s * (hw + 0.012f);
                if (side > 0) {
                    CabComponents::AppendQuad(mesh.get(), Vec3(xSwoopEnd, winMainBot - 0.06f, zWave), Vec3(winEndX + 0.02f, winRearBot - 0.06f, zWave), Vec3(winEndX + 0.02f, winRearBot + 0.01f, zWave), Vec3(xSwoopEnd, winMainBot + 0.01f, zWave), accent);
                } else {
                    CabComponents::AppendQuad(mesh.get(), Vec3(winEndX + 0.02f, winRearBot - 0.06f, zWave), Vec3(xSwoopEnd, winMainBot - 0.06f, zWave), Vec3(xSwoopEnd, winMainBot + 0.01f, zWave), Vec3(winEndX + 0.02f, winRearBot + 0.01f, zWave), accent);
                }
                float zPin = s * (hw + 0.009f);
                float pinY = archTopY + 0.32f;
                RunningGear::AppendBox(mesh.get(), Vec3(xSwoopEnd, pinY, std::min(zOut, zPin)), Vec3(winEndX, pinY + 0.028f, std::max(zOut, zPin)), copperStripe);
            }
        } else {
            RunningGear::AppendBox(mesh.get(), Vec3(xSwoopEnd, archTopY - 0.01f, -hw), Vec3(winEndX, winMainBot, -hw + 0.07f), bodyPaint);
            RunningGear::AppendBox(mesh.get(), Vec3(xSwoopEnd, archTopY - 0.01f, hw - 0.07f), Vec3(winEndX, winMainBot, hw), bodyPaint);
        }

        // Exact signature side pillar sashes & trim features per coach model
        for (int side : {-1, 1}) {
            float s = static_cast<float>(side);
            float zTri = s * (hw + 0.014f);

            if (isNeoplan) {
                // Neoplan Tourliner signature sharp forward-angled Pearl White "shark-fin" B-pillar biting into glass
                float finBotX0 = xSwoopStart + 0.15f;
                float finBotX1 = xSwoopEnd + 0.15f;
                float finTopX0 = xApillar + 0.12f;
                float finTopX1 = xApillar + 0.52f;
                if (side > 0) {
                    CabComponents::AppendQuad(mesh.get(), Vec3(finBotX0, winMainBot - 0.18f, zTri), Vec3(finBotX1, winMainBot, zTri), Vec3(finTopX1, winSideTop + 0.02f, zTri), Vec3(finTopX0, winSideTop + 0.02f, zTri), bodyPaint);
                } else {
                    CabComponents::AppendQuad(mesh.get(), Vec3(finBotX1, winMainBot, zTri), Vec3(finBotX0, winMainBot - 0.18f, zTri), Vec3(finTopX0, winSideTop + 0.02f, zTri), Vec3(finTopX1, winSideTop + 0.02f, zTri), bodyPaint);
                }
                RunningGear::AppendBox(mesh.get(), Vec3(finBotX0 - 0.04f, winMainBot - 0.02f, s > 0 ? hw : -hw - 0.016f), Vec3(finBotX1 - 0.10f, winMainBot + 0.06f, s > 0 ? hw + 0.016f : -hw), chromeCol);
            } else if (isYutong || isTwinDome) {
                // Signature sweeping curved silver/white hockey-stick C-pillar sash rising behind cab into roof rail
                float hBotX0 = frontAxleX - 0.55f;
                float hBotX1 = frontAxleX - 0.22f;
                float hTopX0 = xApillar + 0.24f;
                float hTopX1 = xApillar + 0.62f;
                const Vec4 sashCol = isYutong ? chromeCol : bodyPaint;
                if (side > 0) {
                    CabComponents::AppendQuad(mesh.get(), Vec3(hBotX0, frontWinLowBot, zTri), Vec3(hBotX1, winMainBot - 0.08f, zTri), Vec3(hTopX1, winSideTop + 0.03f, zTri), Vec3(hTopX0, winSideTop + 0.03f, zTri), sashCol);
                } else {
                    CabComponents::AppendQuad(mesh.get(), Vec3(hBotX1, winMainBot - 0.08f, zTri), Vec3(hBotX0, frontWinLowBot, zTri), Vec3(hTopX0, winSideTop + 0.03f, zTri), Vec3(hTopX1, winSideTop + 0.03f, zTri), sashCol);
                }
                if (isTwinDome) {
                    // Golden orange front corner wing badge & white upper roof edge strip
                    RunningGear::AppendBox(mesh.get(), Vec3(xSwoopStart - 0.25f, archTopY + 0.04f, s > 0 ? hw : -hw - 0.015f), Vec3(xSwoopStart + 0.15f, frontWinLowBot, s > 0 ? hw + 0.015f : -hw), accent);
                }
            } else if (isChampagne) {
                // Flowing chrome spear behind cab door branching into twin thin pinstripes
                RunningGear::AppendBox(mesh.get(), Vec3(xSwoopStart + 0.05f, frontWinLowBot + 0.08f, s > 0 ? hw : -hw - 0.015f), Vec3(xSwoopEnd + 0.15f, winMainBot - 0.14f, s > 0 ? hw + 0.015f : -hw), chromeCol);
                RunningGear::AppendBox(mesh.get(), Vec3(xSwoopEnd, winMainBot - 0.18f, s > 0 ? hw : -hw - 0.012f), Vec3(winEndX, winMainBot - 0.155f, s > 0 ? hw + 0.012f : -hw), darkTrim);
                RunningGear::AppendBox(mesh.get(), Vec3(xSwoopEnd, winMainBot - 0.26f, s > 0 ? hw : -hw - 0.012f), Vec3(winEndX, winMainBot - 0.235f, s > 0 ? hw + 0.012f : -hw), darkTrim);
            } else if (isObsidian) {
                // Continuous bright mirror-chrome eyebrow arch curving over wheel well & down body beltline
                RunningGear::AppendBox(mesh.get(), Vec3(xSwoopStart, winMainBot - 0.08f, s > 0 ? hw : -hw - 0.014f), Vec3(winEndX + 0.02f, winMainBot - 0.04f, s > 0 ? hw + 0.014f : -hw), chromeCol);
                RunningGear::AppendBox(mesh.get(), Vec3(frontAxleX - 0.72f, archTopY + 0.06f, s > 0 ? hw : -hw - 0.012f), Vec3(frontAxleX - 0.62f, archTopY + 0.12f, s > 0 ? hw + 0.012f : -hw), amberMarker);
            } else if (isExecutiveI6) {
                // Continuous chrome beltline spear sweeping behind the diagonal curved B-pillar & amber side clearance markers
                RunningGear::AppendBox(mesh.get(), Vec3(xSwoopEnd - 0.04f, winMainBot - 0.045f, s > 0 ? hw : -hw - 0.014f), Vec3(winEndX + 0.02f, winMainBot + 0.012f, s > 0 ? hw + 0.014f : -hw), chromeCol);
                // Chrome front quarter lower trim strip connecting front cheek to swoop
                if (side < 0) {
                    RunningGear::AppendBox(mesh.get(), Vec3(xApillar, frontWinLowBot - 0.045f, -hw - 0.014f), Vec3(xSwoopStart + 0.04f, frontWinLowBot + 0.008f, -hw), chromeCol);
                } else {
                    RunningGear::AppendBox(mesh.get(), Vec3(entryBackX, frontWinLowBot - 0.045f, hw), Vec3(xSwoopStart + 0.04f, frontWinLowBot + 0.008f, hw + 0.014f), chromeCol);
                }
                for (float mx : {xFront + 1.85f, frontAxleX + 1.45f, (frontAxleX + driveAxleX) * 0.60f, xRear - 0.75f}) {
                    RunningGear::AppendBox(mesh.get(), Vec3(mx - 0.055f, skirtBotY + 0.15f, s > 0 ? hw : -hw - 0.013f), Vec3(mx + 0.055f, skirtBotY + 0.20f, s > 0 ? hw + 0.013f : -hw), amberMarker);
                }
            }

            if (is9700 || is9800Aero || is9700Teal || isChampagne) {
                RunningGear::AppendBox(mesh.get(), Vec3(frontAxleX - 0.72f, archTopY + 0.06f, s > 0 ? hw : -hw - 0.012f), Vec3(frontAxleX - 0.62f, archTopY + 0.12f, s > 0 ? hw + 0.012f : -hw), amberMarker);
            }
            if (isNeoplan || isYutong || isTwinDome || isObsidian || is9800Aero || isAeroBlade || isExecutiveI6) {
                // Rear quarter vertical/horizontal engine intake louver grille
                for (int lv = 0; lv < 5; ++lv) {
                    float ly = skirtBotY + 0.22f + static_cast<float>(lv) * 0.11f;
                    const Vec4 lvCol = isAeroBlade ? chromeCol : darkTrim;
                    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.95f, ly, s > 0 ? hw : -hw - 0.012f), Vec3(xRear - 0.26f, ly + 0.045f, s > 0 ? hw + 0.012f : -hw), lvCol);
                }
            }
            if (isBzr) {
                // BZR Electric flush aerodynamic upper drive-wheel & tag-wheel fairings
                RunningGear::AppendBox(mesh.get(), Vec3(driveAxleX - archHalfLen + 0.04f, archTopY - 0.28f, s > 0 ? hw - 0.01f : -hw - 0.022f), Vec3(tagAxleX + archHalfLen - 0.04f, archTopY + 0.02f, s > 0 ? hw + 0.022f : -hw + 0.01f), bodyPaint);
                RunningGear::AppendBox(mesh.get(), Vec3(driveAxleX - archHalfLen + 0.04f, archTopY - 0.30f, s > 0 ? hw : -hw - 0.026f), Vec3(tagAxleX + archHalfLen - 0.04f, archTopY - 0.27f, s > 0 ? hw + 0.026f : -hw), Vec4(0.16f, 0.84f, 0.82f, 1.0f));
            } else if (is9700) {
                // US/CAN fluted corrugated stainless belt ribs along side panels
                for (int r = 0; r < 3; ++r) {
                    float ry = archTopY + 0.10f + static_cast<float>(r) * 0.12f;
                    RunningGear::AppendBox(mesh.get(), Vec3(xSwoopEnd, ry, s > 0 ? hw : -hw - 0.012f), Vec3(winEndX, ry + 0.03f, s > 0 ? hw + 0.012f : -hw), chromeCol);
                }
            }
        }

        if (isUnvi || isBzr) {
            const Vec4 beltCol = isUnvi ? accent : chromeCol;
            RunningGear::AppendBox(mesh.get(), Vec3(xApillar,  frontWinLowBot - 0.045f, -hw - 0.011f), Vec3(xSwoopStart, frontWinLowBot + 0.008f, -hw + 0.01f), beltCol);
            RunningGear::AppendBox(mesh.get(), Vec3(entryBackX, frontWinLowBot - 0.045f, hw - 0.01f), Vec3(xSwoopStart, frontWinLowBot + 0.008f,  hw + 0.011f), beltCol);
            RunningGear::AppendBox(mesh.get(), Vec3(xSwoopEnd, winMainBot - 0.05f, -hw - 0.011f), Vec3(winEndX + 0.02f, winMainBot + 0.01f, -hw + 0.01f), beltCol);
            RunningGear::AppendBox(mesh.get(), Vec3(xSwoopEnd, winMainBot - 0.05f,  hw - 0.01f), Vec3(winEndX + 0.02f, winMainBot + 0.01f,  hw + 0.011f), beltCol);
        } else if (is9900) {
            RunningGear::AppendBox(mesh.get(), Vec3(xApillar,  frontWinLowBot - 0.045f, -hw - 0.011f), Vec3(xSwoopStart, frontWinLowBot + 0.008f, -hw + 0.01f), accent);
            RunningGear::AppendBox(mesh.get(), Vec3(entryBackX, frontWinLowBot - 0.045f, hw - 0.01f), Vec3(xSwoopStart, frontWinLowBot + 0.008f,  hw + 0.011f), accent);
        }
        CabComponents::AppendSkirtWithCutouts(mesh.get(), xFront + 0.76f, winEndX, skirtBotY + 0.05f, skirtBotY + 0.12f, -hw - 0.008f, false, 0.02f, leftSkipSkirt, is9700 ? darkTrim : (isExecutiveI6 ? bodyPaint : accent));
        CabComponents::AppendSkirtWithCutouts(mesh.get(), xFront + 0.76f, winEndX, skirtBotY + 0.05f, skirtBotY + 0.12f,  hw + 0.008f, true,  0.02f, rightSkipSkirt, is9700 ? darkTrim : (isExecutiveI6 ? bodyPaint : accent));
    }

    // 4. Flush Underfloor Luggage Compartment Bay Doors (pantograph lift when action_ratio > 0)
    float bayLift = std::clamp(m_Def.action_ratio, 0.0f, 1.0f) * 0.45f;
    int bayCount = 3;
    float baySpan = bayEndX - bayStartX;
    for (int b = 0; b < bayCount; ++b) {
        float bx0 = bayStartX + (baySpan * static_cast<float>(b)) / static_cast<float>(bayCount) + 0.035f;
        float bx1 = bayStartX + (baySpan * static_cast<float>(b + 1)) / static_cast<float>(bayCount) - 0.035f;
        float zOut = hw + 0.018f + bayLift * 0.28f;
        float yTop = deckFloorY - 0.12f;
        float yBot = skirtBotY + 0.13f + bayLift * 0.48f;
        if (yTop > yBot + 0.15f) {
            RunningGear::AppendBox(mesh.get(), Vec3(bx0, yBot, -zOut), Vec3(bx1, yTop, -hw + 0.01f), bayDoorCol);
            RunningGear::AppendBox(mesh.get(), Vec3(bx0, yBot,  hw - 0.01f), Vec3(bx1, yTop,  zOut), bayDoorCol);
            float midB = (bx0 + bx1) * 0.5f;
            const Vec4 latchCol = (is9900 || isYutong || isNeoplan) ? darkTrim : chromeCol;
            RunningGear::AppendBox(mesh.get(), Vec3(midB - 0.07f, yBot + 0.09f, -zOut - 0.012f), Vec3(midB + 0.07f, yBot + 0.14f, -zOut), latchCol);
            RunningGear::AppendBox(mesh.get(), Vec3(midB - 0.07f, yBot + 0.09f,  zOut), Vec3(midB + 0.07f, yBot + 0.14f,  zOut + 0.012f), latchCol);
        }
    }

    // 5. Opaque Side Window Mullions, Openable Curb Door Frame & Style-Specific Mirrors (Pass 1)
    CabComponents::AppendSwoopingCoachGlassSide(
        mesh.get(),
        xApillar, xSwoopEnd, winEndX,
        frontWinLowBot, winMainBot, winSideTop,
        hw, 7, sideGlass, glossBlack, false, true, entryFrontX, entryBackX, xSwoopStart, winRearBot
    );

    CabComponents::AppendAnimatedDoor(
        mesh.get(),
        entryFrontX, entryBackX,
        skirtBotY + 0.12f, winSideTop - 0.04f,
        hw, true,
        m_Def.door_open_ratio, false, false,
        bayDoorCol, glossBlack, sideGlass
    );

    coach::AppendCoachMirrorsForStyle(mesh.get(), style, xFront, windshieldTopY, H, hw, bodyPaint, darkTrim);

    // 6. Coach-Specific Curved Rear Fascia, Custom Taillights, Diffusers & Exhausts
    coach::AppendCoachRearDesign(
        mesh.get(),
        style,
        xRear,
        skirtBotY, deckFloorY, winSideTop, H, hw,
        m_Def.headlights_on, false,
        bodyPaint, accent, glossBlack, darkTrim, chromeCol
    );

    // 7. Translucent Automotive Glass Pass (DRAWN LAST so 3D interior & steps shine clearly through!)
    coach::AppendCoachCurvedWindshields(
        mesh.get(),
        style,
        xFront, xApillar,
        skirtBotY, noseTopY, windshieldTopY, winSideTop, H, hw,
        windshieldGlass
    );
    CabComponents::AppendSwoopingCoachGlassSide(
        mesh.get(),
        xApillar, xSwoopEnd, winEndX,
        frontWinLowBot, winMainBot, winSideTop,
        hw, 7, sideGlass, glossBlack, true, false, entryFrontX, entryBackX, xSwoopStart, winRearBot
    );
    CabComponents::AppendAnimatedDoor(
        mesh.get(),
        entryFrontX, entryBackX,
        skirtBotY + 0.12f, winSideTop - 0.04f,
        hw, true,
        m_Def.door_open_ratio, false, true,
        bayDoorCol, glossBlack, sideGlass
    );
    if (isTwinDome || is9900) {
        // Panoramic tinted glass scenic roof skylights on shoulder edges
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 1.85f, H - 0.08f, -hw + 0.30f), Vec3(xRear - 1.65f, H + 0.01f, -hw + 0.62f), sideGlass);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 1.85f, H - 0.08f,  hw - 0.62f), Vec3(xRear - 1.65f, H + 0.01f,  hw - 0.30f), sideGlass);
    }

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

bool CoachBus::GenerateTextures(VkDevice, VkQueue, VkCommandPool) {
    m_Generated = true;
    return true;
}

} // namespace sphere::vehicles
