#pragma once

namespace procedural::props {

struct GateProfileSpec {
    float cantileverLeafClearanceMeters = 0.08f;
    float pivotLeafThicknessMeters = 0.045f;
    float louverBladePitchMeters = 0.11f;
    float biFoldArmRadiusMeters = 0.045f;
    float speedstilePedestalLengthMeters = 1.35f;
    float solarMastHeightMeters = 0.85f;
};

} // namespace procedural::props
