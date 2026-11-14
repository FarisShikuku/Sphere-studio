#pragma once
#include "TrailerID.h"
#include "scene/Transform.h"
#include "scene/AABB.h"
#include <vulkan/vulkan.h>
#include <string>

namespace sphere::trailers {

enum class HitchType : uint32_t {
    FIFTH_WHEEL_KINGPIN = 0,
    PINTLE_RING_DRAWBAR,
    BALL_HITCH_50MM,
    DETACHABLE_GOOSENECK
};

enum class SuspensionType : uint32_t {
    AIR_SUSPENSION = 0,
    LEAF_SPRING,
    HYDRAULIC_MODULAR
};

enum class BrakeType : uint32_t {
    DISC_ABS = 0,
    DRUM_SCAM
};

struct TrailerDef {
    TrailerID id = TrailerID::FLATBED_STD;
    std::string name;
    std::string category;
    HitchType hitch_type = HitchType::FIFTH_WHEEL_KINGPIN;

    // Physical Mechanics & Chassis Specs
    float empty_weight_kg = 6500.0f;
    float max_payload_kg = 32000.0f;
    float volume_m3 = 82.0f;
    Vec3 dimensions{13.6f, 2.55f, 1.55f}; // length, width, height (meters)
    float deck_height_m = 1.35f;
    float kingpin_offset_m = 1.65f;

    // Running Gear Configuration
    uint32_t axle_count = 3;
    bool has_lift_axle = false;
    bool has_steerable_axle = false;
    SuspensionType suspension_type = SuspensionType::AIR_SUSPENSION;
    BrakeType brake_type = BrakeType::DISC_ABS;
    float wheel_radius_m = 0.52f;
    float tire_width_m = 0.385f;

    // Vulkan PBR Texture Block Formats
    VkFormat albedo_format = VK_FORMAT_BC7_SRGB_BLOCK;
    VkFormat normal_format = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormat orm_format = VK_FORMAT_BC7_UNORM_BLOCK;
    uint32_t texture_width = 1024;
    uint32_t texture_height = 1024;

    // Material & Stencil Properties
    float opacity_bias = 1.0f; // <1.0 for translucent curtainsiders
    float roughness = 0.55f;
    float metallic = 0.45f;
    Vec4 base_color{0.25f, 0.30f, 0.36f, 1.0f};
    Vec4 accent_color{0.88f, 0.25f, 0.12f, 1.0f};
    std::string label_text = "SPHERE LOGISTICS";

    uint32_t seed = 0x7A110001;
    float wear_and_tear = 0.12f;

    // Runtime Interactive Mechanics & Visual State
    bool lift_axle_raised = false;
    float steer_angle_deg = 0.0f;
    bool landing_gear_extended = false;
    float curtain_open_ratio = 0.0f;
    float hydraulic_lift_ratio = 0.0f;
    bool has_dual_wheels = false;
};

} // namespace sphere::trailers
