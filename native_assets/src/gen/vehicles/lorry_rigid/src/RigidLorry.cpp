#include "RigidLorry.h"
#include "CabComponents.h"
#include "ChassisComponents.h"
#include "RunningGear.h"
#include <algorithm>
#include <cmath>

namespace sphere::vehicles {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::cab::CabComponents;
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

// Rotated box around Z axis at pivot (px, py) for hydraulic tipper skip
void AppendBoxTiltedZ(
    Mesh* mesh,
    const Vec3& minP, const Vec3& maxP,
    float pivotX, float pivotY,
    float tiltAngleRad,
    const Vec4& col
) {
    float c = std::cos(tiltAngleRad);
    float s = std::sin(tiltAngleRad);
    // Negative tilt angle raises -X front upward when pivoting around rear +X axle
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

// Watertight outward-CCW tapered cement mixer drum inclined upward toward rear
void AppendMixerDrum(
    Mesh* mesh,
    float xFront, float xRear,
    float yFront, float yRear,
    float spinRad,
    int segs,
    const Vec4& colA,
    const Vec4& colB
) {
    // 5 axial rings: pedestal nose -> flare -> barrel -> taper -> discharge cone
    const float tPos[5] = {0.0f, 0.22f, 0.56f, 0.86f, 1.0f};
    const float rads[5] = {0.42f, 0.98f, 1.04f, 0.72f, 0.38f};

    auto ringPt = [&](float fx, float fy, float r, float theta) -> Vec3 {
        // Orthogonal cross section roughly perpendicular to drum axis
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
            // Helical barber-pole stripe pattern rotating with drum spin
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
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(m_Def.dimensions.x, 6.2f, 12.0f);
    const float W = std::clamp(m_Def.dimensions.y, 2.25f, 2.60f);
    const float H = std::clamp(m_Def.dimensions.z, 2.65f, 4.10f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(m_Def.wheel_radius_m, 0.42f, 0.62f);
    const float tireW = std::clamp(m_Def.tire_width_m, 0.26f, 0.40f);
    const float halfTrack = hw - tireW * 0.65f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float frameBotY = std::max(0.68f, wheelR * 2.0f - 0.16f);
    const float frameTopY = frameBotY + 0.24f;

    const Vec4 cabPaint = m_Def.base_color;
    const Vec4 accent = m_Def.accent_color;
    const Vec4 frameDark(0.13f, 0.15f, 0.18f, 1.0f);
    const Vec4 grilleChrome(0.76f, 0.79f, 0.84f, 1.0f);
    const Vec4 grilleInner(0.08f, 0.09f, 0.11f, 1.0f);
    const Vec4 darkTrim(0.11f, 0.12f, 0.14f, 1.0f);
    const Vec4 windowGlass(0.11f, 0.17f, 0.26f, 1.0f);
    const Vec4 alloyTank(0.74f, 0.77f, 0.82f, 1.0f);

    // 1. Frame Rails
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.08f, xRear - 0.06f, frameBotY, frameTopY, 0.43f, frameDark);

    // 2. Cab-Over-Engine (COE) Forward Cab
    const float cabLen = 2.15f;
    const float cabBackX = xFront + cabLen;
    const float cabTopY = std::clamp(m_Def.cab_height_m, 2.55f, 3.25f);
    const float cabBaseY = frameBotY - 0.04f;
    const float cabHW = hw - 0.03f;

    // Lower bumper & wheel arch skirt
    CabComponents::AppendFrontBumperAndLights(mesh.get(), xFront, cabBaseY - 0.18f, cabBaseY + 0.22f, cabHW, m_Def.headlights_on, darkTrim);
    // Main cab body box + slightly sloped upper cab
    RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.02f, cabBaseY + 0.22f, -cabHW), Vec3(cabBackX, cabBaseY + 1.10f, cabHW), cabPaint);
    CabComponents::AppendFrustumX(
        mesh.get(),
        xFront + 0.04f, cabBaseY + 1.10f, cabTopY, cabHW - 0.02f,
        cabBackX,       cabBaseY + 1.10f, cabTopY, cabHW,
        cabPaint
    );
    // Roof aero visor for box truck
    if (m_Def.id == VehicleID::LORRY_RIGID_BOX) {
        CabComponents::AppendFrustumX(
            mesh.get(),
            xFront + 0.65f, cabTopY, cabTopY + 0.05f, cabHW - 0.08f,
            cabBackX,       cabTopY, std::max(cabTopY + 0.12f, H - 0.06f), cabHW - 0.04f,
            cabPaint
        );
    }

    // Windshield, heavy grille, doors, wing mirrors, roof markers
    CabComponents::AppendWindshield(mesh.get(), xFront + 0.04f, xFront + 0.18f, cabBaseY + 1.14f, cabTopY - 0.14f, cabHW - 0.06f, windowGlass, darkTrim);
    CabComponents::AppendHeavyGrille(mesh.get(), xFront + 0.02f, cabBaseY + 0.30f, cabBaseY + 1.00f, cabHW * 0.72f, 4, grilleChrome, grilleInner);
    CabComponents::AppendDoor(mesh.get(), xFront + 0.32f, cabBackX - 0.26f, cabBaseY + 0.26f, cabTopY - 0.18f, -cabHW, false, cabPaint, windowGlass);
    CabComponents::AppendDoor(mesh.get(), xFront + 0.32f, cabBackX - 0.26f, cabBaseY + 0.26f, cabTopY - 0.18f,  cabHW, true,  cabPaint, windowGlass);
    CabComponents::AppendWingMirrors(mesh.get(), xFront + 0.24f, cabBaseY + 1.38f, cabHW, darkTrim);
    CabComponents::AppendAmberRoofMarkers(mesh.get(), xFront + 0.16f, cabTopY, cabHW, m_Def.headlights_on);

    // 3. Axles (Steerable front axle(s) + Single/Tandem rear drive bogie)
    const float frontAxleX = xFront + 1.38f;
    const float wb = std::clamp(m_Def.wheelbase_m, 3.6f, L - 2.2f);
    const float rearCenterX = std::min(xRear - 1.35f, frontAxleX + wb);

    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, halfTrack, false, m_Def.steer_angle_deg);
    if (m_Def.drive_config == VehicleDriveConfig::DRIVE_8X4) {
        ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX + 1.36f, wheelR, tireW, halfTrack, false, m_Def.steer_angle_deg * 0.72f);
    }

    if (m_Def.drive_config == VehicleDriveConfig::DRIVE_4X2) {
        ChassisComponents::AppendVehicleAxle(mesh.get(), rearCenterX, wheelR, tireW, halfTrack, true, 0.0f);
    } else {
        ChassisComponents::AppendVehicleAxle(mesh.get(), rearCenterX - 0.68f, wheelR, tireW, halfTrack, true, 0.0f);
        ChassisComponents::AppendVehicleAxle(mesh.get(), rearCenterX + 0.68f, wheelR, tireW, halfTrack, true, 0.0f);
    }

    // 4. Chassis Ancillaries (Fuel tank, Battery/AdBlue box, Drivetrain)
    ChassisComponents::AppendFuelTank(mesh.get(), cabBackX + 0.35f, std::min(rearCenterX - 1.25f, cabBackX + 1.85f), frameBotY + 0.02f, -(hw - 0.36f), 0.28f, alloyTank);
    ChassisComponents::AppendBatteryAndDefBox(mesh.get(), cabBackX + 0.35f, cabBackX + 1.15f, frameBotY - 0.12f, frameTopY - 0.02f, hw - 0.42f, true);
    ChassisComponents::AppendDrivetrain(mesh.get(), frontAxleX, rearCenterX, std::max(0.24f, m_Def.ground_clearance_m), wheelR);

    // 5. Rear Cargo Body by Variant
    const float bodyFrontX = cabBackX + 0.18f;
    const float bodyRearX = xRear;
    const float deckY = frameTopY + 0.04f;

    switch (m_Def.id) {
        case VehicleID::LORRY_RIGID_BOX: {
            RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX, deckY, -hw), Vec3(bodyRearX, H, hw), accent);
            // Corporate side livery stripe band
            RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX + 0.12f, deckY + (H - deckY) * 0.38f, -hw - 0.01f), Vec3(bodyRearX - 0.12f, deckY + (H - deckY) * 0.56f, -hw + 0.01f), cabPaint);
            RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX + 0.12f, deckY + (H - deckY) * 0.38f,  hw - 0.01f), Vec3(bodyRearX - 0.12f, deckY + (H - deckY) * 0.56f,  hw + 0.01f), cabPaint);
            // Rear roll-up door ribs + latch bars
            RunningGear::AppendBox(mesh.get(), Vec3(bodyRearX - 0.01f, deckY + 0.06f, -hw + 0.10f), Vec3(bodyRearX + 0.02f, H - 0.08f, hw - 0.10f), Vec4(0.78f, 0.80f, 0.84f, 1.0f));
            RunningGear::AppendBox(mesh.get(), Vec3(bodyRearX + 0.01f, deckY + 0.12f, -0.45f), Vec3(bodyRearX + 0.045f, H - 0.14f, -0.40f), grilleChrome);
            RunningGear::AppendBox(mesh.get(), Vec3(bodyRearX + 0.01f, deckY + 0.12f,  0.40f), Vec3(bodyRearX + 0.045f, H - 0.14f,  0.45f), grilleChrome);
            break;
        }
        case VehicleID::LORRY_RIGID_FLATBED: {
            // Hardwood deck + steel edge coaming
            Vec4 deckWood(0.45f, 0.32f, 0.18f, 1.0f);
            RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX, deckY, -hw), Vec3(bodyRearX, deckY + 0.12f, hw), deckWood);
            // Front headboard
            RunningGear::AppendBox(mesh.get(), Vec3(bodyFrontX, deckY + 0.12f, -hw), Vec3(bodyFrontX + 0.09f, H, hw), cabPaint);
            // Dropside hinged panels (folded downward with action_ratio)
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
            float tiltAngle = -std::clamp(m_Def.action_ratio, 0.0f, 1.0f) * 0.62f; // -35.5 deg raises front nose upward
            // Cab protector canopy + main ribbed quarry skip
            AppendBoxTiltedZ(mesh.get(), Vec3(cabBackX - 0.45f, H - 0.12f, -hw), Vec3(bodyFrontX + 0.06f, H + 0.02f, hw), pivotX, pivotY, tiltAngle, cabPaint);
            AppendBoxTiltedZ(mesh.get(), Vec3(bodyFrontX, deckY, -hw), Vec3(bodyRearX, H, hw), pivotX, pivotY, tiltAngle, cabPaint);
            // Structural vertical stiffener ribs
            for (int r = 0; r < 5; ++r) {
                float rx = bodyFrontX + 0.35f + static_cast<float>(r) * ((bodyRearX - bodyFrontX - 0.7f) / 4.0f);
                AppendBoxTiltedZ(mesh.get(), Vec3(rx - 0.06f, deckY, -hw - 0.04f), Vec3(rx + 0.06f, H, -hw), pivotX, pivotY, tiltAngle, accent);
                AppendBoxTiltedZ(mesh.get(), Vec3(rx - 0.06f, deckY,  hw), Vec3(rx + 0.06f, H,  hw + 0.04f), pivotX, pivotY, tiltAngle, accent);
            }
            // Front hydraulic hoist cylinder
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
            // Support A-frames on chassis rails
            RunningGear::AppendBox(mesh.get(), Vec3(drumFrontX - 0.18f, frameTopY, -0.52f), Vec3(drumFrontX + 0.18f, drumFrontY, 0.52f), frameDark);
            RunningGear::AppendBox(mesh.get(), Vec3(drumRearX - 0.55f, frameTopY, -0.54f), Vec3(drumRearX - 0.25f, drumRearY - 0.52f, 0.54f), frameDark);
            float spin = m_Def.action_ratio * 6.2831853f;
            AppendMixerDrum(mesh.get(), drumFrontX, drumRearX, drumFrontY, drumRearY, spin, segs, cabPaint, accent);
            // Rear discharge chute & hopper
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
            // Cradle saddles
            for (float f : {0.15f, 0.50f, 0.85f}) {
                float sx = tankFront + (tankRear - tankFront) * f;
                RunningGear::AppendBox(mesh.get(), Vec3(sx - 0.12f, frameTopY, -0.68f), Vec3(sx + 0.12f, tankCy - tankR * 0.45f, 0.68f), frameDark);
            }
            // Watertight closed cylindrical tank with shallow 0.07R dished end caps (no pointed cones, no interior void)
            ChassisComponents::AppendClosedCylinderX(mesh.get(), tankFront, tankRear, tankCy, 0.0f, tankR, segs, cabPaint, accent, 0.07f);
            // Upper anti-slip walkway + 3 inspection dome hatches
            float topY = tankCy + tankR;
            RunningGear::AppendBox(mesh.get(), Vec3(tankFront + 0.25f, topY - 0.02f, -0.34f), Vec3(tankRear - 0.25f, topY + 0.05f, 0.34f), darkTrim);
            for (float f : {0.25f, 0.50f, 0.75f}) {
                float hx = tankFront + (tankRear - tankFront) * f;
                ChassisComponents::AppendClosedCylinderY(mesh.get(), hx, 0.0f, topY + 0.02f, topY + 0.14f, 0.22f, 14, grilleChrome);
            }
            break;
        }
    }

    // Rear ECE R58 underrun bumper & tail lights
    RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.14f, 0.44f, -hw + 0.08f), Vec3(xRear + 0.04f, 0.56f, hw - 0.08f), grilleChrome);
    RunningGear::AppendBox(mesh.get(), Vec3(xRear + 0.02f, 0.46f, -hw + 0.10f), Vec3(xRear + 0.06f, 0.55f, -hw + 0.34f), Vec4(0.86f, 0.10f, 0.10f, 1.0f));
    RunningGear::AppendBox(mesh.get(), Vec3(xRear + 0.02f, 0.46f,  hw - 0.34f), Vec3(xRear + 0.06f, 0.55f,  hw - 0.10f), Vec4(0.86f, 0.10f, 0.10f, 1.0f));

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
