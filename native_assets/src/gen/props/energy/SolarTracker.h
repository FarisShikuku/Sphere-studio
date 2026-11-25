#pragma once

#include "../PropMeshHelper.h"

namespace procedural::props {

class SolarTracker {
public:
    static void appendGimbalAndTorqueTube(
        GeneratedMeshPayload& payload,
        const Vec3& center,
        float tubeLength,
        float tiltAngleDeg,
        uint32_t color = 0xFF505662
    );
};

} // namespace procedural::props
