#pragma once
#include "RunningGearVariants.h"
#include "scene/Mesh.h"
#include "scene/Transform.h"
#include <cstdint>

namespace sphere::trailers::gear {

/**
 * Shared Procedural Running Gear Engine.
 * Generates axle beams, air suspension bellows or parabolic leaf springs,
 * dual-wheel rubber tires, silver alloy/steel rims, red brake calipers/drums,
 * ABS sensor rings, mudguards, and telescopic landing gear legs.
 */
class RunningGear {
public:
    static void AppendMesh(
        Mesh* mesh,
        const RunningGearConfig& cfg,
        float bogieCenterX,
        float chassisBottomY,
        float landingGearX,
        bool landingGearExtended = false
    );

    static void AppendBox(
        Mesh* mesh,
        const Vec3& minP,
        const Vec3& maxP,
        const Vec4& color
    );

    static void AppendWheelZ(
        Mesh* mesh,
        float cx, float cy, float cz,
        float radius, float width,
        int segments,
        const Vec4& tireColor,
        const Vec4& rimColor,
        const Vec4& brakeColor,
        float steerAngleRad = 0.0f
    );
};

} // namespace sphere::trailers::gear
