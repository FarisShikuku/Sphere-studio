#pragma once
#include "ChassisComponentsVariants.h"
#include "scene/Mesh.h"
#include "scene/Transform.h"

namespace sphere::vehicles::chassis {

/**
 * Shared Procedural Chassis Components Engine.
 * Builds C-channel ladder frame rails, solid closed cylindrical aluminum fuel tanks,
 * vertical chrome exhaust stacks, AdBlue/battery boxes, engine blocks, drivetrains,
 * SAE fifth-wheel coupling saddles, and closed cylindrical drums/tanks.
 */
class ChassisComponents {
public:
    // Closed, watertight outward-CCW cylinder along X axis (optional shallow dished end caps)
    static void AppendClosedCylinderX(
        Mesh* mesh,
        float xStart, float xEnd,
        float cy, float cz,
        float radius,
        int segments,
        const Vec4& shellColor,
        const Vec4& capColor,
        float dishDepthRatio = 0.08f
    );

    // Closed, watertight outward-CCW vertical cylinder along Y axis (for exhaust stacks / stanchions)
    static void AppendClosedCylinderY(
        Mesh* mesh,
        float cx, float cz,
        float yBot, float yTop,
        float radius,
        int segments,
        const Vec4& color
    );

    // Main twin C-channel frame rails + crossmembers
    static void AppendFrameRails(
        Mesh* mesh,
        float frontX, float rearX,
        float botY, float topY,
        float railOffsetZ,
        const Vec4& frameColor
    );

    // Closed cylindrical fuel tank (horizontal or twin-strap mounted)
    static void AppendFuelTank(
        Mesh* mesh,
        float xStart, float xEnd,
        float cy, float cz,
        float radius,
        const Vec4& alloyColor
    );

    // Vertical twin chrome exhaust stacks + heat shields behind cab
    static void AppendExhaustStacks(
        Mesh* mesh,
        float stackX,
        float botY, float topY,
        float halfSpanZ,
        const Vec4& chromeColor
    );

    // Sealed AdBlue/DEF + Battery Box
    static void AppendBatteryAndDefBox(
        Mesh* mesh,
        float xStart, float xEnd,
        float botY, float topY,
        float sideZ, bool isRightSide
    );

    // Lower Engine sump + driveshaft + differential housings
    static void AppendDrivetrain(
        Mesh* mesh,
        float engineX, float rearAxleX,
        float groundClearanceY,
        float axleCenterY
    );

    // SAE 2-inch Fifth-Wheel Coupling Saddle
    static void AppendFifthWheelSaddle(
        Mesh* mesh,
        float centerX,
        float frameTopY
    );

    // Steerable front + single/tandem/tridem rear axles using RunningGear::AppendWheelZ
    static void AppendVehicleAxle(
        Mesh* mesh,
        float axleX,
        float wheelRadius,
        float tireWidth,
        float halfTrack,
        bool dualWheels,
        float steerAngleDeg,
        bool discBrakes = true
    );
};

} // namespace sphere::vehicles::chassis
