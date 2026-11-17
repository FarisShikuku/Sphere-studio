#pragma once

#include "VehicleID.h"
#include "Mesh.h"

namespace sphere::vehicles::city {

enum class CityBusStyle {
    STANDARD_TRANSIT = 0,
    DOUBLE_DECKER,
    ARTICULATED_BRT,
    ELECTRIC_LOW_FLOOR
};

inline CityBusStyle ResolveCityStyle(VehicleID id) {
    switch (id) {
        case VehicleID::BUS_CITY_DOUBLE_DECKER:    return CityBusStyle::DOUBLE_DECKER;
        case VehicleID::BUS_CITY_ARTICULATED_BENDY: return CityBusStyle::ARTICULATED_BRT;
        case VehicleID::BUS_CITY_LOW_FLOOR:        return CityBusStyle::ELECTRIC_LOW_FLOOR;
        case VehicleID::BUS_CITY_STANDARD:
        default:                                   return CityBusStyle::STANDARD_TRANSIT;
    }
}

void AppendCityFrontDesign(
    Mesh* mesh,
    CityBusStyle style,
    float xFront,
    float xApillar,
    float floorY,
    float noseTopY,
    float winSideTop,
    float roofTopY,
    float hw,
    bool headlightsOn,
    const Vec4& bodyPaint,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol,
    const Vec4& destAmber
);

void AppendCityCurvedWindshields(
    Mesh* mesh,
    CityBusStyle style,
    float xFront,
    float xApillar,
    float floorY,
    float noseTopY,
    float lowerWinTop,
    float upperWinBot,
    float upperWinTop,
    float roofTopY,
    float hw,
    const Vec4& glassColor
);

void AppendCityRoofCanopySmooth(
    Mesh* mesh,
    float xFront,
    float xStartTrim,
    float xEndTrim,
    float winSideTop,
    float roofTopY,
    float hw,
    const Vec4& roofColor,
    bool closeFront,
    bool closeRear
);

void AppendCityMirrorsForStyle(
    Mesh* mesh,
    CityBusStyle style,
    float xFront,
    float winTop,
    float hw,
    const Vec4& bodyPaint,
    const Vec4& darkTrim
);

void AppendCityRearDesign(
    Mesh* mesh,
    CityBusStyle style,
    float xRear,
    float floorY,
    float lowerWinTop,
    float upperWinTop,
    float hw,
    bool headlightsOn,
    const Vec4& bodyPaint,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
);

} // namespace sphere::vehicles::city
