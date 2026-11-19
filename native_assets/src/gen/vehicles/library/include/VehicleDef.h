#pragma once
#include "VehicleID.h"
#include "scene/Transform.h"
#include "scene/AABB.h"
#include <vulkan/vulkan.h>
#include <cstdint>
#include <string>

namespace sphere::vehicles {

enum class LiveryID : uint32_t {
    FLEET_CORPORATE = 0,
    NAIROBI_NGANYA,
    TRANSIT_MUNICIPAL,
    HIGHWAY_HAULAGE,
    INDUSTRIAL_WORKHORSE,
    LUXURY_METALLIC
};

enum class VehicleDriveConfig : uint32_t {
    DRIVE_4X2 = 0,
    DRIVE_6X4,
    DRIVE_8X4,
    DRIVE_6X2_TAG
};

struct VehicleDef {
    VehicleID id = VehicleID::LORRY_RIGID_BOX;
    std::string name;
    std::string category;
    VehicleDriveConfig drive_config = VehicleDriveConfig::DRIVE_4X2;
    LiveryID livery = LiveryID::FLEET_CORPORATE;

    // Real-World Metric Chassis & Powertrain Specs
    float kerb_weight_kg = 7200.0f;
    float max_gvw_kg = 18000.0f;
    float wheelbase_m = 4.5f;
    float fifth_wheel_offset_m = 0.0f; // >0 for articulated tractors (+X from vehicle center)
    float cab_height_m = 2.85f;
    float engine_torque_nm = 1150.0f;
    float engine_power_hp = 320.0f;
    uint32_t passenger_capacity = 2;

    Vec3 dimensions{8.20f, 2.50f, 3.35f}; // length (X), width (Z), height (Y) in meters
    float ground_clearance_m = 0.32f;
    float wheel_radius_m = 0.50f;
    float tire_width_m = 0.315f;

    // Vulkan PBR Texture Block Formats
    VkFormat albedo_format = VK_FORMAT_BC7_SRGB_BLOCK;
    VkFormat normal_format = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormat orm_format = VK_FORMAT_BC7_UNORM_BLOCK;
    uint32_t texture_width = 1024;
    uint32_t texture_height = 1024;

    // Material, Livery & Surface Properties (Alpha strictly 1.0)
    float roughness = 0.38f;
    float metallic = 0.42f;
    float glass_tint_opacity = 0.88f; // Used by shader metadata; rasterizer keeps geometry 100% opaque
    Vec4 base_color{0.16f, 0.34f, 0.62f, 1.0f};
    Vec4 accent_color{0.95f, 0.58f, 0.08f, 1.0f};
    std::string label_text = "SPHERE TRANSIT";

    uint32_t seed = 0x5A1C0001u;
    float wear_and_tear = 0.08f;

    // Runtime Interactive Mechanics
    float action_ratio = 0.0f;      // Tipper lift / mixer drum spin / luggage bays / bendy flex
    float door_open_ratio = 0.0f;   // Passenger / cab door open ratio [0.0..1.0]
    float steer_angle_deg = 0.0f;   // Front steer axle turn angle [-28..+28 deg]
    bool headlights_on = true;
    bool has_roof_rack = false;
    bool has_exhaust_stacks = false;
    bool is_rhd = false;            // false = LHD (Left-Hand Drive), true = RHD (Right-Hand Drive)
};

} // namespace sphere::vehicles
