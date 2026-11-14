#pragma once
#include "../../library/include/TrailerDef.h"
#include <cstdint>

namespace sphere::trailers::gear {

enum class RunningGearVariant : uint32_t {
    TANDEM_AXLE = 0,
    TRIDEM_AXLE,
    LIFT_AXLE,
    STEERABLE,
    AIR_SUSPENSION,
    LEAF_SPRING
};

struct RunningGearConfig {
    RunningGearVariant variant = RunningGearVariant::TRIDEM_AXLE;
    uint32_t axle_count = 3;
    float axle_spacing_m = 1.31f;
    float track_width_m = 2.04f;
    float wheel_radius_m = 0.52f;
    float tire_width_m = 0.385f;
    float suspension_travel_m = 0.18f;
    bool has_lift_axle = false;
    bool lift_axle_raised = false;
    bool has_steerable_axle = false;
    float steer_angle_deg = 0.0f;
    SuspensionType suspension_type = SuspensionType::AIR_SUSPENSION;
    BrakeType brake_type = BrakeType::DISC_ABS;
    bool has_dual_wheels = false;
    float trailer_length_m = 13.6f;
    HitchType hitch_type = HitchType::FIFTH_WHEEL_KINGPIN;
};

class RunningGearVariants {
public:
    static RunningGearConfig FromTrailerDef(const TrailerDef& def);
};

} // namespace sphere::trailers::gear
