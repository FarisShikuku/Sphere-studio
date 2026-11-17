#pragma once

#include "VehicleID.h"
#include "Mesh.h"

namespace sphere::vehicles::coach {

enum class CoachDesignStyle {
    UNVI_HIGH_DECKER = 0,
    VOLVO_9900_THEATER,
    VOLVO_BZR_ELECTRIC,
    VOLVO_9700_USCAN,
    VOLVO_9700_DD,
    VOLVO_9800_DD,
    VOLVO_9700_TEAL,
    VOLVO_9800_AERO,
    YUTONG_TOURING,
    AERO_BLADE_VIP,
    CHAMPAGNE_SR2,
    NEOPLAN_TOURLINER,
    TWIN_GLASS_DOME,
    OBSIDIAN_VIP_6X2,
    EXECUTIVE_COACH_I6,
    LUXURY_VIP_4X2
};

inline CoachDesignStyle ResolveCoachStyle(VehicleID id) {
    switch (id) {
        case VehicleID::COACH_HIGH_DECKER:      return CoachDesignStyle::UNVI_HIGH_DECKER;
        case VehicleID::COACH_SLEEPER:          return CoachDesignStyle::VOLVO_9900_THEATER;
        case VehicleID::COACH_TOURING:          return CoachDesignStyle::VOLVO_BZR_ELECTRIC;
        case VehicleID::COACH_LUXURY:           return CoachDesignStyle::VOLVO_9700_USCAN;
        case VehicleID::COACH_VOLVO_9700_DD:    return CoachDesignStyle::VOLVO_9700_DD;
        case VehicleID::COACH_VOLVO_9800_DD:    return CoachDesignStyle::VOLVO_9800_DD;
        case VehicleID::COACH_VOLVO_9700_TEAL:  return CoachDesignStyle::VOLVO_9700_TEAL;
        case VehicleID::COACH_VOLVO_9800_AERO:  return CoachDesignStyle::VOLVO_9800_AERO;
        case VehicleID::COACH_YUTONG_TOURING:   return CoachDesignStyle::YUTONG_TOURING;
        case VehicleID::COACH_AERO_BLADE_VIP:   return CoachDesignStyle::AERO_BLADE_VIP;
        case VehicleID::COACH_CHAMPAGNE_SR2:    return CoachDesignStyle::CHAMPAGNE_SR2;
        case VehicleID::COACH_NEOPLAN_TOURLINER:return CoachDesignStyle::NEOPLAN_TOURLINER;
        case VehicleID::COACH_TWIN_GLASS_DOME:  return CoachDesignStyle::TWIN_GLASS_DOME;
        case VehicleID::COACH_OBSIDIAN_VIP_6X2: return CoachDesignStyle::OBSIDIAN_VIP_6X2;
        case VehicleID::COACH_EXECUTIVE_I6:     return CoachDesignStyle::EXECUTIVE_COACH_I6;
        case VehicleID::COACH_LUXURY_VIP_4X2:   return CoachDesignStyle::LUXURY_VIP_4X2;
        default:                                return CoachDesignStyle::VOLVO_9700_USCAN;
    }
}

void AppendCoachFrontDesign(
    Mesh* mesh,
    CoachDesignStyle style,
    float xFront,
    float xApillar,
    float skirtBotY,
    float noseTopY,
    float windshieldTopY,
    float winSideTop,
    float H,
    float hw,
    bool headlightsOn,
    const Vec4& bodyPaint,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol,
    const Vec4& ledDisplay
);

void AppendCoachCurvedWindshields(
    Mesh* mesh,
    CoachDesignStyle style,
    float xFront,
    float xApillar,
    float skirtBotY,
    float noseTopY,
    float windshieldTopY,
    float winSideTop,
    float H,
    float hw,
    const Vec4& glassColor
);

void AppendCoachRoofCanopySmooth(
    Mesh* mesh,
    float xFront,
    float winEndX,
    float xRear,
    float winSideTop,
    float H,
    float hw,
    const Vec4& roofColor,
    bool isDoubleDeck = false
);

void AppendCoachMirrorsForStyle(
    Mesh* mesh,
    CoachDesignStyle style,
    float xFront,
    float windshieldTopY,
    float H,
    float hw,
    const Vec4& bodyPaint,
    const Vec4& darkTrim
);

void AppendCoachRearDesign(
    Mesh* mesh,
    CoachDesignStyle style,
    float xRear,
    float skirtBotY,
    float deckFloorY,
    float winSideTop,
    float H,
    float hw,
    bool headlightsOn,
    bool isDoubleDeck,
    const Vec4& bodyPaint,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
);

} // namespace sphere::vehicles::coach
