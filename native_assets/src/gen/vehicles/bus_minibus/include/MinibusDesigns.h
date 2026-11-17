#pragma once

#include "Mesh.h"
#include "VehicleID.h"

namespace sphere::vehicles::minibus {

enum class MinibusStyle {
    MATATU_NGANYA = 0,
    SPRINTER_COMMERCIAL,
    AIRPORT_SHUTTLE,
    SCHOOL_BUS_VAN
};

inline MinibusStyle ResolveMinibusStyle(VehicleID id) {
    switch (id) {
        case VehicleID::SPRINTER_VAN:  return MinibusStyle::SPRINTER_COMMERCIAL;
        case VehicleID::SHUTTLE_BUS:   return MinibusStyle::AIRPORT_SHUTTLE;
        case VehicleID::SCHOOL_VAN:    return MinibusStyle::SCHOOL_BUS_VAN;
        case VehicleID::MATATU_14_SEATER:
        default:                       return MinibusStyle::MATATU_NGANYA;
    }
}

void AppendMinibusFrontAndRoof(
    Mesh* mesh,
    MinibusStyle style,
    float xFront,
    float xCowl,
    float xRoofJoin,
    float xRear,
    float floorY,
    float hoodTopY,
    float winBotY,
    float winTopY,
    float roofTopY,
    float hw,
    bool headlightsOn,
    float doorOpenRatio,
    const Vec4& bodyPaint,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
);

void AppendMinibusCurvedWindshield(
    Mesh* mesh,
    MinibusStyle style,
    float xFront,
    float xCowl,
    float xRoofJoin,
    float floorY,
    float hoodTopY,
    float winTopY,
    float roofTopY,
    float hw,
    const Vec4& glassColor
);

void AppendMinibusMirrors(
    Mesh* mesh,
    MinibusStyle style,
    float xCowl,
    float xRoofJoin,
    float hoodTopY,
    float winTopY,
    float hw,
    const Vec4& bodyPaint,
    const Vec4& darkTrim,
    const Vec4& chromeCol
);

void AppendMinibusRearDesign(
    Mesh* mesh,
    MinibusStyle style,
    float xSideEnd,
    float xRear,
    float floorY,
    float winBotY,
    float winTopY,
    float roofTopY,
    float hw,
    bool headlightsOn,
    const Vec4& bodyPaint,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
);

void AppendMinibusRearGlass(
    Mesh* mesh,
    MinibusStyle style,
    float xRear,
    float winBotY,
    float winTopY,
    float hw,
    const Vec4& rearGlassColor
);

} // namespace sphere::vehicles::minibus
