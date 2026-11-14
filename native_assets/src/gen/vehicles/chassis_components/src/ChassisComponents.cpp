#include "ChassisComponents.h"
#include "RunningGear.h"
#include <algorithm>
#include <cmath>

namespace sphere::vehicles::chassis {

using sphere::trailers::gear::RunningGear;

namespace {
inline void PushTri(Mesh* mesh, const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec4& col) {
    uint32_t idx = static_cast<uint32_t>(mesh->vertices.size());
    Vec4 c(col.x, col.y, col.z, 1.0f);
    Vertex v0; v0.position = p0; v0.color = c; v0.uv[0] = 0.0f; v0.uv[1] = 0.0f;
    Vertex v1; v1.position = p1; v1.color = c; v1.uv[0] = 1.0f; v1.uv[1] = 0.0f;
    Vertex v2; v2.position = p2; v2.color = c; v2.uv[0] = 1.0f; v2.uv[1] = 1.0f;
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
} // namespace

void ChassisComponents::AppendClosedCylinderX(
    Mesh* mesh,
    float xStart, float xEnd,
    float cy, float cz,
    float radius,
    int segments,
    const Vec4& shellColor,
    const Vec4& capColor,
    float dishDepthRatio
) {
    int segs = std::max(16, segments);
    float x0 = std::min(xStart, xEnd);
    float x1 = std::max(xStart, xEnd);
    float dish = std::max(0.0f, radius * dishDepthRatio);
    Vec3 cFront(x0 - dish, cy, cz);
    Vec3 cBack(x1 + dish, cy, cz);

    // Outward winding check (X cylinder): p(theta) = (x, cy + R*sin(t), cz + R*cos(t))
    // Wait: Tangent along +theta is (0, R*cos, -R*sin). Tangent along +X is (1, 0, 0).
    // (1,0,0) x (0, cos, -sin) = (0, sin, cos) -> INWARD!
    // (0, cos, -sin) x (1,0,0) = (0, -sin, -cos) -> wait:
    // +theta cross +X = (0,cos,-sin) x (1,0,0) = (0, -sin, -cos)? No:
    // u = (1, 0, 0) [x0->x1], v = (0, sin1-sin0, cos1-cos0) ~ (0, cos, -sin)*dt
    // u x v = (0*0 - 0*0, 0*0 - 1*(-sin), 1*cos - 0) = (0, +sin, +cos) = OUTWARD RADIAL!
    // Let's verify explicitly!
    // Let a0 = 0: p0 = (x0, cy, cz + R), p1 = (x0, cy + R*sin(d), cz + R*cos(d)) ~ (x0, cy + dR, cz + R)
    // p2 = (x1, cy, cz + R).
    // Tri (a, b, c) where a = p(x0, a0), b = p(x1, a0), c = p(x1, a1):
    // b - a = (+L, 0, 0), c - a = (+L, +dY, 0) => (b-a) x (c-a) = (+L,0,0) x (+L,+dY,0) = (0, 0, +L*dY) -> +Z OUTWARD!
    for (int i = 0; i < segs; ++i) {
        float t0 = (static_cast<float>(i) / static_cast<float>(segs)) * 6.2831853f;
        float t1 = (static_cast<float>(i + 1) / static_cast<float>(segs)) * 6.2831853f;
        float c0 = std::cos(t0), s0 = std::sin(t0);
        float c1 = std::cos(t1), s1 = std::sin(t1);

        Vec3 f0(x0, cy + radius * s0, cz + radius * c0);
        Vec3 f1(x0, cy + radius * s1, cz + radius * c1);
        Vec3 b0(x1, cy + radius * s0, cz + radius * c0);
        Vec3 b1(x1, cy + radius * s1, cz + radius * c1);

        PushQuad(mesh, f0, b0, b1, f1, shellColor);

        // Front cap (-X outward normal): (cFront, f1, f0): (f1-cFront) x (f0-cFront) ~ (0, -dY, 0) -> wait:
        // At a0=0, f0=(x0, cy, cz+R), f1=(x0, cy+dY, cz+R), cFront=(x0-d, cy, cz).
        // f0 - cFront = (+d, 0, +R), f1 - cFront = (+d, +dY, +R).
        // (f0 - cFront) x (f1 - cFront) = ( -R*dY, +d*dY, +d*dY ) -> x component is NEGATIVE (-R*dY) = -X OUTWARD!
        PushTri(mesh, cFront, f0, f1, capColor);

        // Back cap (+X outward normal): reverse order (cBack, b1, b0) -> +X OUTWARD!
        PushTri(mesh, cBack, b1, b0, capColor);
    }
}

void ChassisComponents::AppendClosedCylinderY(
    Mesh* mesh,
    float cx, float cz,
    float yBot, float yTop,
    float radius,
    int segments,
    const Vec4& color
) {
    int segs = std::max(12, segments);
    Vec3 cBot(cx, yBot, cz);
    Vec3 cTop(cx, yTop, cz);

    // p(theta) = (cx + R*cos(t), y, cz + R*sin(t)).
    // At t0=0: b0=(cx+R, yBot, cz), t0=(cx+R, yTop, cz), t1=(cx+R, yTop, cz+dZ), b1=(cx+R, yBot, cz+dZ).
    // Consider (b0, t0, t1, b1): t0-b0 = (0, +H, 0), t1-b0 = (0, +H, +dZ).
    // (t0-b0) x (t1-b0) = (0,+H,0) x (0,+H,+dZ) = (+H*dZ, 0, 0) -> +X OUTWARD!
    for (int i = 0; i < segs; ++i) {
        float t0 = (static_cast<float>(i) / static_cast<float>(segs)) * 6.2831853f;
        float t1 = (static_cast<float>(i + 1) / static_cast<float>(segs)) * 6.2831853f;
        float c0 = std::cos(t0), s0 = std::sin(t0);
        float c1 = std::cos(t1), s1 = std::sin(t1);

        Vec3 b0(cx + radius * c0, yBot, cz + radius * s0);
        Vec3 b1(cx + radius * c1, yBot, cz + radius * s1);
        Vec3 u0(cx + radius * c0, yTop, cz + radius * s0);
        Vec3 u1(cx + radius * c1, yTop, cz + radius * s1);

        PushQuad(mesh, b0, u0, u1, b1, color);

        // Top cap (+Y): at t0=0 -> b0=(+R,0,0), b1=(+R,0,+dZ)
        // (u1 - cTop) x (u0 - cTop) = (+R,0,+dZ) x (+R,0,0) = (0, +R*dZ, 0) -> +Y OUTWARD!
        PushTri(mesh, cTop, u1, u0, color);
        // Bottom cap (-Y): (cBot, b0, b1) -> -Y OUTWARD!
        PushTri(mesh, cBot, b0, b1, color);
    }
}

void ChassisComponents::AppendFrameRails(
    Mesh* mesh,
    float frontX, float rearX,
    float botY, float topY,
    float railOffsetZ,
    const Vec4& frameColor
) {
    float beamW = 0.085f;
    RunningGear::AppendBox(mesh, Vec3(frontX, botY, -railOffsetZ - beamW), Vec3(rearX, topY, -railOffsetZ), frameColor);
    RunningGear::AppendBox(mesh, Vec3(frontX, botY,  railOffsetZ), Vec3(rearX, topY,  railOffsetZ + beamW), frameColor);

    float span = rearX - frontX;
    int crossCount = std::max(3, static_cast<int>(span / 1.45f));
    for (int i = 0; i <= crossCount; ++i) {
        float cx = frontX + (span * static_cast<float>(i)) / static_cast<float>(crossCount);
        RunningGear::AppendBox(
            mesh,
            Vec3(cx - 0.06f, botY + 0.03f, -railOffsetZ),
            Vec3(cx + 0.06f, topY - 0.02f,  railOffsetZ),
            frameColor
        );
    }
}

void ChassisComponents::AppendFuelTank(
    Mesh* mesh,
    float xStart, float xEnd,
    float cy, float cz,
    float radius,
    const Vec4& alloyColor
) {
    Vec4 strapCol(0.11f, 0.12f, 0.14f, 1.0f);
    Vec4 capCol(
        std::max(0.0f, alloyColor.x * 0.86f),
        std::max(0.0f, alloyColor.y * 0.86f),
        std::max(0.0f, alloyColor.z * 0.86f),
        1.0f
    );
    AppendClosedCylinderX(mesh, xStart, xEnd, cy, cz, radius, 20, alloyColor, capCol, 0.07f);
    // Two steel retention straps around tank
    float span = xEnd - xStart;
    for (float f : {0.26f, 0.74f}) {
        float sx = xStart + span * f;
        AppendClosedCylinderX(mesh, sx - 0.025f, sx + 0.025f, cy, cz, radius + 0.012f, 20, strapCol, strapCol, 0.0f);
    }
}

void ChassisComponents::AppendExhaustStacks(
    Mesh* mesh,
    float stackX,
    float botY, float topY,
    float halfSpanZ,
    const Vec4& chromeColor
) {
    Vec4 shieldCol(0.32f, 0.35f, 0.39f, 1.0f);
    for (int side : {-1, 1}) {
        float sz = static_cast<float>(side) * halfSpanZ;
        // Main riser pipe
        AppendClosedCylinderY(mesh, stackX, sz, botY, topY, 0.065f, 14, chromeColor);
        // Perforated heat-shield sleeve
        AppendClosedCylinderY(mesh, stackX, sz, botY + 0.35f, topY - 0.55f, 0.092f, 14, shieldCol);
        // Elbow connector back into frame
        RunningGear::AppendBox(
            mesh,
            Vec3(stackX - 0.14f, botY, std::min(0.0f, sz)),
            Vec3(stackX + 0.06f, botY + 0.12f, std::max(0.0f, sz)),
            shieldCol
        );
    }
}

void ChassisComponents::AppendBatteryAndDefBox(
    Mesh* mesh,
    float xStart, float xEnd,
    float botY, float topY,
    float sideZ, bool isRightSide
) {
    Vec4 boxSteel(0.15f, 0.17f, 0.20f, 1.0f);
    Vec4 defBlue(0.12f, 0.44f, 0.84f, 1.0f);
    float zMin = isRightSide ? sideZ : sideZ - 0.38f;
    float zMax = isRightSide ? sideZ + 0.38f : sideZ;
    RunningGear::AppendBox(mesh, Vec3(xStart, botY, zMin), Vec3(xEnd, topY, zMax), boxSteel);
    // Blue DEF/AdBlue filler cap protruding slightly outboard
    float capZ0 = isRightSide ? zMax - 0.02f : zMin - 0.03f;
    float capZ1 = isRightSide ? zMax + 0.03f : zMin + 0.02f;
    RunningGear::AppendBox(mesh, Vec3(xStart + 0.06f, topY - 0.11f, capZ0), Vec3(xStart + 0.18f, topY - 0.03f, capZ1), defBlue);
}

void ChassisComponents::AppendDrivetrain(
    Mesh* mesh,
    float engineX, float rearAxleX,
    float groundClearanceY,
    float axleCenterY
) {
    Vec4 ironDark(0.16f, 0.18f, 0.21f, 1.0f);
    Vec4 sumpSteel(0.24f, 0.26f, 0.30f, 1.0f);
    // Engine block & oil pan sump (strictly above ground_clearance_m)
    RunningGear::AppendBox(
        mesh,
        Vec3(engineX - 0.48f, groundClearanceY, -0.34f),
        Vec3(engineX + 0.48f, groundClearanceY + 0.58f, 0.34f),
        sumpSteel
    );
    // Propeller shaft
    if (rearAxleX > engineX + 0.6f) {
        AppendClosedCylinderX(
            mesh, engineX + 0.45f, rearAxleX, axleCenterY + 0.03f, 0.0f,
            0.055f, 12, ironDark, ironDark, 0.0f
        );
    }
    // Rear differential pumpkin
    RunningGear::AppendBox(
        mesh,
        Vec3(rearAxleX - 0.22f, axleCenterY - 0.18f, -0.22f),
        Vec3(rearAxleX + 0.22f, axleCenterY + 0.18f,  0.22f),
        ironDark
    );
}

void ChassisComponents::AppendFifthWheelSaddle(
    Mesh* mesh,
    float centerX,
    float frameTopY
) {
    Vec4 castIron(0.18f, 0.20f, 0.24f, 1.0f);
    Vec4 greaseCol(0.08f, 0.08f, 0.09f, 1.0f);
    Vec4 handleSilver(0.75f, 0.78f, 0.82f, 1.0f);

    // Mounting slider plate
    RunningGear::AppendBox(mesh, Vec3(centerX - 0.62f, frameTopY, -0.46f), Vec3(centerX + 0.62f, frameTopY + 0.08f, 0.46f), castIron);
    // Twin saddle pedestals
    RunningGear::AppendBox(mesh, Vec3(centerX - 0.16f, frameTopY + 0.08f, -0.38f), Vec3(centerX + 0.16f, frameTopY + 0.21f, -0.24f), castIron);
    RunningGear::AppendBox(mesh, Vec3(centerX - 0.16f, frameTopY + 0.08f,  0.24f), Vec3(centerX + 0.16f, frameTopY + 0.21f,  0.38f), castIron);
    // Heavy Jost/Fontaine Top Coupling Horseshoe Plate (top at frameTopY + 0.27m ~ 1.18-1.24m)
    RunningGear::AppendBox(mesh, Vec3(centerX - 0.42f, frameTopY + 0.21f, -0.44f), Vec3(centerX + 0.42f, frameTopY + 0.27f, 0.44f), greaseCol);
    // Release lever bar
    RunningGear::AppendBox(mesh, Vec3(centerX - 0.03f, frameTopY + 0.16f, 0.40f), Vec3(centerX + 0.03f, frameTopY + 0.21f, 0.68f), handleSilver);
}

void ChassisComponents::AppendVehicleAxle(
    Mesh* mesh,
    float axleX,
    float wheelRadius,
    float tireWidth,
    float halfTrack,
    bool dualWheels,
    float steerAngleDeg,
    bool discBrakes
) {
    const Vec4 axleSteel(0.14f, 0.16f, 0.19f, 1.0f);
    const Vec4 tireRubber(0.08f, 0.08f, 0.09f, 1.0f);
    const Vec4 silverRim(0.78f, 0.81f, 0.86f, 1.0f);
    const Vec4 brakeCol = discBrakes
        ? Vec4(0.72f, 0.12f, 0.11f, 1.0f)
        : Vec4(0.28f, 0.29f, 0.31f, 1.0f);

    float steerRad = steerAngleDeg * (3.14159265f / 180.0f);
    float outerOff = dualWheels ? (tireWidth * 0.56f) : 0.0f;

    // Solid axle beam spanning between tires (y = wheelRadius so tire bottoms touch Y=0.0f cleanly)
    RunningGear::AppendBox(
        mesh,
        Vec3(axleX - 0.07f, wheelRadius - 0.07f, -halfTrack - outerOff * 0.4f),
        Vec3(axleX + 0.07f, wheelRadius + 0.07f,  halfTrack + outerOff * 0.4f),
        axleSteel
    );

    if (dualWheels) {
        RunningGear::AppendWheelZ(mesh, axleX, wheelRadius, -halfTrack + outerOff, wheelRadius, tireWidth, 28, tireRubber, silverRim, brakeCol, steerRad);
        RunningGear::AppendWheelZ(mesh, axleX, wheelRadius, -halfTrack - outerOff, wheelRadius, tireWidth, 28, tireRubber, silverRim, brakeCol, steerRad);
        RunningGear::AppendWheelZ(mesh, axleX, wheelRadius,  halfTrack - outerOff, wheelRadius, tireWidth, 28, tireRubber, silverRim, brakeCol, steerRad);
        RunningGear::AppendWheelZ(mesh, axleX, wheelRadius,  halfTrack + outerOff, wheelRadius, tireWidth, 28, tireRubber, silverRim, brakeCol, steerRad);
    } else {
        RunningGear::AppendWheelZ(mesh, axleX, wheelRadius, -halfTrack, wheelRadius, tireWidth, 28, tireRubber, silverRim, brakeCol, steerRad);
        RunningGear::AppendWheelZ(mesh, axleX, wheelRadius,  halfTrack, wheelRadius, tireWidth, 28, tireRubber, silverRim, brakeCol, steerRad);
    }
}

} // namespace sphere::vehicles::chassis
