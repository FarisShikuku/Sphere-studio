#include "RunningGearVariants.h"
#include <algorithm>

namespace sphere::trailers::gear {

RunningGearConfig RunningGearVariants::FromTrailerDef(const TrailerDef& def) {
    RunningGearConfig cfg;
    cfg.axle_count = def.axle_count;
    cfg.trailer_length_m = def.dimensions.x;
    cfg.hitch_type = def.hitch_type;

    if (def.hitch_type == HitchType::BALL_HITCH_50MM || def.category == "Recreational") {
        cfg.wheel_radius_m = 0.345f; // 205/75R15
        cfg.tire_width_m = 0.215f;
        cfg.axle_spacing_m = 0.88f;
    } else if (def.category == "Car Carriers") {
        cfg.wheel_radius_m = 0.385f; // 235/75R17.5
        cfg.tire_width_m = 0.315f;
        cfg.axle_spacing_m = 1.25f;
    } else if (def.hitch_type == HitchType::DETACHABLE_GOOSENECK || def.category == "Heavy Haulage" || def.deck_height_m <= 1.02f) {
        cfg.wheel_radius_m = 0.395f; // 245/70R17.5 low-profile dual
        cfg.tire_width_m = 0.235f;
        cfg.axle_spacing_m = 1.25f;
        cfg.has_dual_wheels = true;
    } else if (def.deck_height_m < 1.22f) {
        cfg.wheel_radius_m = 0.420f; // 315/45R22.5 mega-liner
        cfg.tire_width_m = 0.315f;
        cfg.axle_spacing_m = 1.36f;
    } else {
        cfg.wheel_radius_m = 0.525f; // 385/65R22.5 highway semi
        cfg.tire_width_m = 0.385f;
        cfg.axle_spacing_m = 1.36f;
    }

    if (def.wheel_radius_m > 0.25f && def.wheel_radius_m != 0.52f) {
        cfg.wheel_radius_m = def.wheel_radius_m;
    }
    if (def.has_dual_wheels) {
        cfg.has_dual_wheels = true;
    }

    float outerTireW = cfg.has_dual_wheels ? (cfg.tire_width_m * 2.0f + 0.06f) : cfg.tire_width_m;
    cfg.track_width_m = std::max(1.20f, def.dimensions.y - outerTireW - 0.04f);
    cfg.has_lift_axle = def.has_lift_axle;
    cfg.lift_axle_raised = def.lift_axle_raised;
    cfg.has_steerable_axle = def.has_steerable_axle;
    cfg.steer_angle_deg = def.steer_angle_deg;
    cfg.suspension_type = def.suspension_type;
    cfg.brake_type = def.brake_type;

    if (def.axle_count == 2) cfg.variant = RunningGearVariant::TANDEM_AXLE;
    else if (def.axle_count >= 3) cfg.variant = RunningGearVariant::TRIDEM_AXLE;
    if (def.has_steerable_axle) cfg.variant = RunningGearVariant::STEERABLE;
    return cfg;
}

} // namespace sphere::trailers::gear
