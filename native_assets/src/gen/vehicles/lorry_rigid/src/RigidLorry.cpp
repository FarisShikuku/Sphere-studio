#include "RigidLorry.h"
#include "BespokeRigidLorries.h"
#include "CabComponents.h"
#include "ChassisComponents.h"
#include "RunningGear.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace sphere::vehicles {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::cab::CabComponents;
using sphere::vehicles::cab::HullStationX;
using sphere::vehicles::chassis::ChassisComponents;

namespace {
inline void PushTri(Mesh* mesh, const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec4& col) {
    uint32_t idx = static_cast<uint32_t>(mesh->vertices.size());
    Vec4 c(col.x, col.y, col.z, 1.0f);
    Vertex v0; v0.position = p0; v0.color = c;
    Vertex v1; v1.position = p1; v1.color = c;
    Vertex v2; v2.position = p2; v2.color = c;
    mesh->vertices.push_back(v0);
    mesh->vertices.push_back(v1);
    mesh->vertices.push_back(v2);
    mesh->indices.push_back(idx);
    mesh->indices.push_back(idx + 1);
    mesh->indices.push_back(idx + 2);
}

inline void PushQuad(Mesh* mesh, const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const Vec4& col) {
    PushTri(mesh, a, b, c, col);
    PushTri(mesh, a, c, d, col);
}

void AppendBoxTiltedZ(
    Mesh* mesh,
    const Vec3& minP, const Vec3& maxP,
    float pivotX, float pivotY,
    float tiltAngleRad,
    const Vec4& col
) {
    float c = std::cos(tiltAngleRad);
    float s = std::sin(tiltAngleRad);
    auto xf = [&](float x, float y, float z) -> Vec3 {
        float dx = x - pivotX;
        float dy = y - pivotY;
        return Vec3(pivotX + dx * c - dy * s, pivotY + dx * s + dy * c, z);
    };
    Vec3 corners[8] = {
        xf(minP.x, minP.y, maxP.z), xf(maxP.x, minP.y, maxP.z),
        xf(maxP.x, maxP.y, maxP.z), xf(minP.x, maxP.y, maxP.z),
        xf(minP.x, minP.y, minP.z), xf(maxP.x, minP.y, minP.z),
        xf(maxP.x, maxP.y, minP.z), xf(minP.x, maxP.y, minP.z)
    };
    const int faces[6][4] = {
        {0, 1, 2, 3}, {5, 4, 7, 6}, {3, 2, 6, 7},
        {4, 5, 1, 0}, {1, 5, 6, 2}, {4, 0, 3, 7}
    };
    for (int f = 0; f < 6; ++f) {
        PushQuad(mesh, corners[faces[f][0]], corners[faces[f][1]], corners[faces[f][2]], corners[faces[f][3]], col);
    }
}

void AppendMixerDrum(
    Mesh* mesh,
    float xFront, float xRear,
    float yFront, float yRear,
    float spinRad,
    int segs,
    const Vec4& colA,
    const Vec4& colB
) {
    const float tPos[5] = {0.0f, 0.22f, 0.56f, 0.86f, 1.0f};
    const float rads[5] = {0.42f, 0.98f, 1.04f, 0.72f, 0.38f};

    auto ringPt = [&](float fx, float fy, float r, float theta) -> Vec3 {
        return Vec3(fx, fy + r * std::sin(theta), r * std::cos(theta));
    };

    Vec3 cFront(xFront - 0.08f, yFront, 0.0f);
    Vec3 cRear(xRear + 0.06f, yRear, 0.0f);

    for (int s = 0; s < 4; ++s) {
        float x0 = xFront + (xRear - xFront) * tPos[s];
        float x1 = xFront + (xRear - xFront) * tPos[s + 1];
        float y0 = yFront + (yRear - yFront) * tPos[s];
        float y1 = yFront + (yRear - yFront) * tPos[s + 1];
        float r0 = rads[s];
        float r1 = rads[s + 1];

        for (int i = 0; i < segs; ++i) {
            float a0 = spinRad + (static_cast<float>(i) / static_cast<float>(segs)) * 6.2831853f;
            float a1 = spinRad + (static_cast<float>(i + 1) / static_cast<float>(segs)) * 6.2831853f;
            bool stripe = ((i + s * 2) % 6) < 2;
            const Vec4& segCol = stripe ? colB : colA;

            Vec3 p00 = ringPt(x0, y0, r0, a0);
            Vec3 p01 = ringPt(x0, y0, r0, a1);
            Vec3 p10 = ringPt(x1, y1, r1, a0);
            Vec3 p11 = ringPt(x1, y1, r1, a1);

            PushQuad(mesh, p00, p10, p11, p01, segCol);

            if (s == 0) {
                PushTri(mesh, cFront, p00, p01, colA);
            }
            if (s == 3) {
                PushTri(mesh, cRear, p11, p10, colB);
            }
        }
    }
}
} // namespace

RigidLorry::RigidLorry(VehicleID id) : m_Def(CreateRigidLorryDefaultDef(id)) {}

std::shared_ptr<Mesh> RigidLorry::GenerateMesh(uint32_t lod) {
    (void)lod;
    if (m_Def.id == VehicleID::LORRY_UD_QUESTER_TIPPER) {
        auto mesh = trucks::GenerateUdQuesterTipperMesh(m_Def);
        m_Bounds = mesh->bounds;
        m_CachedMesh = mesh;
        return mesh;
    }
    if (m_Def.id == VehicleID::LORRY_MAN_TGX_BOX) {
        auto mesh = trucks::GenerateManTgxBoxLorryMesh(m_Def);
        m_Bounds = mesh->bounds;
        m_CachedMesh = mesh;
        return mesh;
    }
    if (m_Def.id == VehicleID::LORRY_SCANIA_P320_CURTAINSIDER) {
        auto mesh = trucks::GenerateScaniaP320CurtainsiderMesh(m_Def);
        m_Bounds = mesh->bounds;
        m_CachedMesh = mesh;
        return mesh;
    }
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(m_Def.dimensions.x, 6.2f, 12.0f);
    const float W = std::clamp(m_Def.dimensions.y, 2.25f, 2.60f);
    const float H = std::clamp(m_Def.dimensions.z, 2.65f, 4.10f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(m_Def.wheel_radius_m, 0.42f, 0.62f);
    const float tireW = std::clamp(m_Def.tire_width_m, 0.26f, 0.40f);

    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = hw - tireW * 1.06f - 0.025f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float frameBotY = std::max(0.68f, wheelR * 2.0f - 0.14f);
    const float frameTopY = frameBotY + 0.24f;
    const float archTopY = wheelR * 2.0f + 0.13f;
    const float archHalfLen = wheelR + 0.12f;

    const Vec4 cabPaint = m_Def.base_color;
    const Vec4 accent = m_Def.accent_color;
    const Vec4 frameDark(0.13f, 0.15f, 0.18f, 1.0f);
    const Vec4 grilleChrome(0.82f, 0.85f, 0.90f, 1.0f);
    const Vec4 glossBlack(0.06f, 0.07f, 0.09f, 1.0f);
    const Vec4 darkTrim(0.10f, 0.11f, 0.13f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 interiorSeat(0.18f, 0.20f, 0.24f, 1.0f);
    const Vec4 dashDark(0.11f, 0.12f, 0.15f, 1.0f);
    const Vec4 alloyTank(0.76f, 0.79f, 0.84f, 1.0f);
    const Vec4 windshieldGlass(0.32f, 0.54f, 0.74f, 0.34f);
    const Vec4 sideGlass(0.20f, 0.38f, 0.56f, 0.42f);

    // 1. Frame Rails & Axles
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.12f, xRear - 0.06f, frameBotY, frameTopY, 0.43f, frameDark);

    const float cabLen = 2.22f;
    const float cabBackX = xFront + cabLen;
    const float cabTopY = std::clamp(m_Def.cab_height_m, 2.55f, 3.25f);
    const float cabBaseY = frameBotY - 0.04f;
    const float cabClearY = std::max(cabBaseY + 0.22f, archTopY + 0.02f);
    const float cabHW = hw - 0.03f;

    const float winFrontX = xFront + 0.12f;
    const float winBrowX  = xFront + 0.38f;
    const float winBotY   = std::max(cabBaseY + 1.12f, cabClearY + 0.88f);
    const float winTopY   = cabTopY - 0.18f;
    const float doorX0    = xFront + 0.40f;
    const float doorX1    = cabBackX - 0.24f;
    const float doorBotY  = cabClearY + 0.02f;
    const float doorTopY  = cabTopY - 0.22f;

    // 2. Full 3D Forward COE Truck Cab Interior (Dash, gauges, steering wheel, twin seats, ceiling)
    CabComponents::AppendTruckCabInterior(
        mesh.get(),
        xFront + 0.30f, cabBackX - 0.06f,
        cabClearY + 0.05f, cabTopY - 0.14f,
        cabHW, false, dashDark, interiorSeat, accent,
        m_Def.is_rhd, winBotY, m_Def.steer_angle_deg
    );

    // 3. Modern Sculpted Euro/Urban COE Forward Cab Shell (built around real windshield & door cutouts)
    CabComponents::AppendModernLedFascia(
        mesh.get(), xFront + 0.10f, cabBaseY - 0.18f, cabBaseY + 0.22f,
        cabHW - 0.02f, m_Def.headlights_on,
        m_Def.id == VehicleID::LORRY_RIGID_BOX ? cabPaint : darkTrim, glossBlack
    );

    // Lower nose below windshield (cleanly above front tire clearance)
    std::vector<HullStationX> coeNose = {
        {xFront + 0.14f, cabClearY, cabBaseY + 1.06f, cabHW - 0.05f, 0.09f, 0.12f, 0.02f, 0.07f},
        {xFront + 0.42f, cabClearY, winBotY,          cabHW,         0.08f, 0.10f, 0.02f, 0.07f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), coeNose, cabPaint, 4);

    // Roof canopy above windshield & doors
    std::vector<HullStationX> coeRoof = {
        {xFront + 0.36f, winTopY, cabTopY - 0.10f, cabHW - 0.02f, 0.03f, 0.19f, 0.06f, 0.07f},
        {xFront + 1.05f, winTopY, cabTopY - 0.04f, cabHW,         0.03f, 0.19f, 0.07f, 0.02f},
        {cabBackX,       winTopY, cabTopY - 0.04f, cabHW,         0.03f, 0.19f, 0.07f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), coeRoof, cabPaint, 4);

    // Rear cab B-pillar & bulkhead wall behind door latch jamb [cabBackX - 0.24f .. cabBackX]
    std::vector<HullStationX> cabRearWall = {
        {cabBackX - 0.24f, cabClearY, winTopY + 0.02f, cabHW, 0.08f, 0.08f, 0.0f, 0.0f},
        {cabBackX,         cabClearY, winTopY + 0.02f, cabHW, 0.08f, 0.08f, 0.0f, 0.0f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), cabRearWall, cabPaint, 4);

    // Slanted A-pillars flanking windshield
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.26f, winBotY - 0.04f, s > 0 ? cabHW - 0.08f : -cabHW), Vec3(xFront + 0.44f, winTopY + 0.03f, s > 0 ? cabHW : -cabHW + 0.08f), glossBlack);
    }

    if (m_Def.id == VehicleID::LORRY_RIGID_BOX) {
        std::vector<HullStationX> aeroPod = {
            {xFront + 0.72f, cabTopY - 0.06f, cabTopY + 0.02f, cabHW - 0.10f, 0.04f, 0.10f, 0.02f, 0.07f},
            {cabBackX,       cabTopY - 0.06f, std::max(cabTopY + 0.14f, H - 0.04f), cabHW - 0.04f, 0.04f, 0.14f, 0.04f, 0.00f},
        };
        CabComponents::AppendSculptedHullX(mesh.get(), aeroPod, cabPaint, 3);
    }

    CabComponents::AppendCurvedBlackNoseMask(
        mesh.get(), winFrontX, winBrowX,
        cabClearY + 0.66f, winBotY, winTopY,
        cabHW - 0.04f, 0.10f, glossBlack, glossBlack, false, true
    );
    CabComponents::AppendHeavyGrille(mesh.get(), xFront + 0.04f, cabClearY + 0.04f, cabClearY + 0.64f, cabHW * 0.72f, 4, grilleChrome, glossBlack);
    CabComponents::AppendAnimatedDoor(mesh.get(), doorX0, doorX1, doorBotY, doorTopY, -cabHW, false, m_Def.door_open_ratio, false, false, cabPaint, glossBlack, sideGlass);
    CabComponents::AppendAnimatedDoor(mesh.get(), doorX0, doorX1, doorBotY, doorTopY,  cabHW, true,  m_Def.door_open_ratio, false, false, cabPaint, glossBlack, sideGlass);
    CabComponents::AppendCantileverCoachMirrors(mesh.get(), xFront + 0.44f, cabTopY - 0.24f, cabHW, m_Def.id == VehicleID::LORRY_RIGID_BOX ? cabPaint : darkTrim);
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.42f, cabTopY - 0.08f, cabHW * 0.85f, m_Def.headlights_on);

    // 4. Axles (accurate track widths!), Wheel-Well Liners & Fenders
    const float frontAxleX = xFront + 1.38f;
    const float wb = std::clamp(m_Def.wheelbase_m, 3.6f, L - 2.2f);
    const float rearCenterX = std::min(xRear - 1.35f, frontAxleX + wb);

    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, frontHalfTrack, false, m_Def.steer_angle_deg);
    CabComponents::AppendWheelWellLiner(mesh.get(), frontAxleX, archHalfLen, frameBotY - 0.12f, archTopY, cabHW, cabHW - tireW - 0.14f, linerDark);
    CabComponents::AppendWheelArchFender(mesh.get(), frontAxleX, wheelR, wheelR + 0.085f, -cabHW, false, 0.036f, darkTrim);
    CabComponents::AppendWheelArchFender(mesh.get(), frontAxleX, wheelR, wheelR + 0.085f,  cabHW, true,  0.036f, darkTrim);

    if (m_Def.drive_config == VehicleDriveConfig::DRIVE_8X4) {
        float frontAxle2X = frontAxleX + 1.36f;
        ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxle2X, wheelR, tireW, frontHalfTrack, false, m_Def.steer_angle_deg * 0.72f);
        CabComponents::AppendWheelArchFender(mesh.get(), frontAxle2X, wheelR, wheelR + 0.085f, -cabHW, false, 0.036f, darkTrim);
        CabComponents::AppendWheelArchFender(mesh.get(), frontAxle2X, wheelR, wheelR + 0.085f,  cabHW, true,  0.036f, darkTrim);
    }

    if (m_Def.drive_config == VehicleDriveConfig::DRIVE_4X2) {
        ChassisComponents::AppendVehicleAxle(mesh.get(), rearCenterX, wheelR, tireW, rearHalfTrack, true, 0.0f);
        CabComponents::AppendWheelArchFender(mesh.get(), rearCenterX, wheelR, wheelR + 0.08f, -hw, false, tireW * 1.15f, darkTrim);
        CabComponents::AppendWheelArchFender(mesh.get(), rearCenterX, wheelR, wheelR + 0.08f,  hw, true,  tireW * 1.15f, darkTrim);
    } else {
        float r0 = rearCenterX - 0.68f;
        float r1 = rearCenterX + 0.68f;
        ChassisComponents::AppendVehicleAxle(mesh.get(), r0, wheelR, tireW, rearHalfTrack, true, 0.0f);
        ChassisComponents::AppendVehicleAxle(mesh.get(), r1, wheelR, tireW, rearHalfTrack, true, 0.0f);
        for (float rx : {r0, r1}) {
            CabComponents::AppendWheelArchFender(mesh.get(), rx, wheelR, wheelR + 0.08f, -hw, false, tireW * 1.15f, darkTrim);
            CabComponents::AppendWheelArchFender(mesh.get(), rx, wheelR, wheelR + 0.08f,  hw, true,  tireW * 1.15f, darkTrim);
        }
    }

    // 5. Chassis Ancillaries (Fuel tank, Battery/AdBlue box, Drivetrain)
    ChassisComponents::AppendFuelTank(mesh.get(), cabBackX + 0.35f, std::min(rearCenterX - 1.25f, cabBackX + 1.85f), frameBotY + 0.02f, -(hw - 0.36f), 0.28f, alloyTank);
    ChassisComponents::AppendBatteryAndDefBox(mesh.get(), cabBackX + 0.35f, cabBackX + 1.15f, frameBotY - 0.12f, frameTopY - 0.02f, hw - 0.42f, true);
    ChassisComponents::AppendDrivetrain(mesh.get(), frontAxleX, rearCenterX, std::max(0.24f, m_Def.ground_clearance_m), wheelR);

    // 6. Rear Cargo Body by Variant
    const float bodyFrontX = cabBackX + 0.18f;
    const float bodyRearX = xRear;
    const float deckY = frameTopY + 0.04f;

    switch (m_Def.id) {
        case VehicleID::LORRY_RIGID_BOX: {
            std::vector<HullStationX> boxStations = {
                {bodyFrontX, deckY, H - 0.02f, hw, 0.04f, 0.10f, 0.02f, 0.03f},
                {bodyRearX,  deckY, H - 0.02f, hw, 0.04f, 0.10f, 0.02f, 0.00f},
            };
            CabComponents::AppendSculptedHullX(mesh.get(), boxStations, accent, 3);
            float skirtX0 = cabBackX + 0.25f;
            float skirtX1 = rearCenterX - archHalfLen - 0.20f;
            if (skirtX1 > skirtX0 + 0.4f) {
                RunningGear::AppendBox(mesh.get(), Vec3(skirtX0, frameBotY - 0.22f, -hw + 0.02f), Vec3(skirtX1, deckY, -hw + 0.08f), cabPaint);
                RunningGear::AppendBox(mesh.get(), Vec3(skirtX0, frameBotY - 0.22f,  hw - 0.08f), Vec3(skirtX1, deckY,  hw - 0.02f), cabPaint);
            }
            RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX + 0.12f, deckY + (H - deckY) * 0.38f, -hw - 0.01f), Vec3(bodyRearX - 0.12f, deckY + (H - deckY) * 0.56f, -hw + 0.01f), cabPaint);
            RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX + 0.12f, deckY + (H - deckY) * 0.38f,  hw - 0.01f), Vec3(bodyRearX - 0.12f, deckY + (H - deckY) * 0.56f,  hw + 0.01f), cabPaint);
            RunningGear::AppendBox(mesh.get(), Vec3(bodyRearX - 0.01f, deckY + 0.06f, -hw + 0.12f), Vec3(bodyRearX + 0.02f, H - 0.10f, hw - 0.12f), Vec4(0.80f, 0.82f, 0.86f, 1.0f));
            RunningGear::AppendBox(mesh.get(), Vec3(bodyRearX + 0.01f, deckY + 0.12f, -0.45f), Vec3(bodyRearX + 0.045f, H - 0.16f, -0.40f), grilleChrome);
            RunningGear::AppendBox(mesh.get(), Vec3(bodyRearX + 0.01f, deckY + 0.12f,  0.40f), Vec3(bodyRearX + 0.045f, H - 0.16f,  0.45f), grilleChrome);
            break;
        }
        case VehicleID::LORRY_RIGID_FLATBED: {
            Vec4 deckWood(0.45f, 0.32f, 0.18f, 1.0f);
            RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX, deckY, -hw), Vec3(bodyRearX, deckY + 0.12f, hw), deckWood);
            RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX, deckY + 0.12f, -hw), Vec3(bodyFrontX + 0.09f, H, hw), cabPaint);
            float dropDrop = std::clamp(m_Def.action_ratio, 0.0f, 1.0f) * 0.45f;
            float sideTop = deckY + 0.72f - dropDrop;
            float sideBot = deckY + 0.12f - dropDrop;
            RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX + 0.12f, sideBot, -hw), Vec3(bodyRearX - 0.04f, sideTop, -hw + 0.06f), accent);
            RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX + 0.12f, sideBot,  hw - 0.06f), Vec3(bodyRearX - 0.04f, sideTop,  hw), accent);
            RunningGear::AppendBox(mesh.get(), Vec3(bodyRearX - 0.07f, deckY + 0.12f, -hw), Vec3(bodyRearX, deckY + 0.72f, hw), accent);
            break;
        }
        case VehicleID::LORRY_RIGID_TIPPER: {
            float pivotX = rearCenterX + 0.55f;
            float pivotY = frameTopY + 0.08f;
            float tiltAngle = -std::clamp(m_Def.action_ratio, 0.0f, 1.0f) * 0.62f;
            AppendBoxTiltedZ(mesh.get(), Vec3(cabBackX - 0.45f, H - 0.12f, -hw), Vec3(bodyFrontX + 0.06f, H + 0.02f, hw), pivotX, pivotY, tiltAngle, cabPaint);
            AppendBoxTiltedZ(mesh.get(), Vec3(bodyFrontX, deckY, -hw), Vec3(bodyRearX, H, hw), pivotX, pivotY, tiltAngle, cabPaint);
            for (int r = 0; r < 5; ++r) {
                float rx = bodyFrontX + 0.35f + static_cast<float>(r) * ((bodyRearX - bodyFrontX - 0.7f) / 4.0f);
                AppendBoxTiltedZ(mesh.get(), Vec3(rx - 0.06f, deckY, -hw - 0.04f), Vec3(rx + 0.06f, H, -hw), pivotX, pivotY, tiltAngle, accent);
                AppendBoxTiltedZ(mesh.get(), Vec3(rx - 0.06f, deckY,  hw), Vec3(rx + 0.06f, H,  hw + 0.04f), pivotX, pivotY, tiltAngle, accent);
            }
            float ramTopY = deckY + std::clamp(m_Def.action_ratio, 0.0f, 1.0f) * 1.95f + 0.45f;
            ChassisComponents::AppendClosedCylinderY(mesh.get(), bodyFrontX + 0.25f, 0.0f, frameBotY + 0.1f, ramTopY, 0.11f, 14, grilleChrome);
            break;
        }
        case VehicleID::LORRY_RIGID_CEMENT_MIXER: {
            int segs = (lod == 0) ? 24 : 16;
            float drumFrontX = bodyFrontX + 0.32f;
            float drumRearX = bodyRearX - 0.45f;
            float drumFrontY = frameTopY + 1.05f;
            float drumRearY = frameTopY + 1.92f;
            RunningGear::AppendBox(mesh.get(), Vec3(drumFrontX - 0.18f, frameTopY, -0.52f), Vec3(drumFrontX + 0.18f, drumFrontY, 0.52f), frameDark);
            RunningGear::AppendBox(mesh.get(), Vec3(drumRearX - 0.55f, frameTopY, -0.54f), Vec3(drumRearX - 0.25f, drumRearY - 0.52f, 0.54f), frameDark);
            float spin = m_Def.action_ratio * 6.2831853f;
            AppendMixerDrum(mesh.get(), drumFrontX, drumRearX, drumFrontY, drumRearY, spin, segs, cabPaint, accent);
            RunningGear::AppendBox(mesh.get(), Vec3(drumRearX - 0.12f, drumRearY - 0.25f, -0.28f), Vec3(bodyRearX + 0.12f, drumRearY + 0.22f, 0.28f), accent);
            break;
        }
        case VehicleID::LORRY_RIGID_TANKER:
        default: {
            int segs = (lod == 0) ? 28 : 18;
            float tankR = 0.96f;
            float tankCy = frameTopY + tankR + 0.08f;
            float tankFront = bodyFrontX + 0.12f;
            float tankRear = bodyRearX - 0.12f;
            for (float f : {0.15f, 0.50f, 0.85f}) {
                float sx = tankFront + (tankRear - tankFront) * f;
                RunningGear::AppendBox(mesh.get(), Vec3(sx - 0.12f, frameTopY, -0.68f), Vec3(sx + 0.12f, tankCy - tankR * 0.45f, 0.68f), frameDark);
            }
            ChassisComponents::AppendClosedCylinderX(mesh.get(), tankFront, tankRear, tankCy, 0.0f, tankR, segs, cabPaint, accent, 0.07f);
            float topY = tankCy + tankR;
            RunningGear::AppendBox(mesh.get(), Vec3(tankFront + 0.25f, topY - 0.02f, -0.34f), Vec3(tankRear - 0.25f, topY + 0.05f, 0.34f), darkTrim);
            for (float f : {0.25f, 0.50f, 0.75f}) {
                float hx = tankFront + (tankRear - tankFront) * f;
                ChassisComponents::AppendClosedCylinderY(mesh.get(), hx, 0.0f, topY + 0.02f, topY + 0.14f, 0.22f, 14, grilleChrome);
            }
            break;
        }
    }

    // Rear ECE R58 underrun bumper & modern LED tail light clusters
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.14f, 0.44f, -hw + 0.08f), Vec3(xRear + 0.04f, 0.56f, hw - 0.08f), grilleChrome);
    RunningGear::AppendBox(mesh.get(), Vec3(xRear + 0.02f, 0.46f, -hw + 0.10f), Vec3(xRear + 0.06f, 0.55f, -hw + 0.36f), Vec4(0.90f, 0.10f, 0.12f, 1.0f));
    RunningGear::AppendBox(mesh.get(), Vec3(xRear + 0.02f, 0.46f,  hw - 0.36f), Vec3(xRear + 0.06f, 0.55f,  hw - 0.10f), Vec4(0.90f, 0.10f, 0.12f, 1.0f));

    // 7. Translucent Cab Glass Pass (DRAWN LAST over truck cab interior)
    CabComponents::AppendCurvedWindshieldGlass(mesh.get(), winFrontX, winBrowX, winBotY, winTopY, cabHW - 0.04f, 0.10f, windshieldGlass);
    CabComponents::AppendAnimatedDoor(mesh.get(), doorX0, doorX1, doorBotY, doorTopY, -cabHW, false, m_Def.door_open_ratio, false, true, cabPaint, glossBlack, sideGlass);
    CabComponents::AppendAnimatedDoor(mesh.get(), doorX0, doorX1, doorBotY, doorTopY,  cabHW, true,  m_Def.door_open_ratio, false, true, cabPaint, glossBlack, sideGlass);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

bool RigidLorry::GenerateTextures(VkDevice, VkQueue, VkCommandPool) {
    m_Generated = true;
    return true;
}

} // namespace sphere::vehicles
