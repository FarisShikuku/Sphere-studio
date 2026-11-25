#pragma once

#include "../../shared/math/MathTypes.h"
#include <string>

namespace procedural::buildings {

enum class SocketType {
    Window,
    Door,
    RoofAttachment,
    Balcony,
    WallDecoration,
    Hvac,
    Signage,
    InteriorDoor,
    DamageOrigin
};

struct Socket {
    SocketType type;
    std::string id;
    Vec3 position;
    Vec3 normal;
    Vec3 tangent;
    float width = 1.0f;
    float height = 1.0f;
    bool isOccupied = false;
};

} // namespace procedural::buildings
