#pragma once
#include "CargoID.h"
#include "scene/Transform.h"
#include "scene/AABB.h"
#include <vulkan/vulkan.h>
#include <string>

namespace sphere::cargo {

enum class CargoPackingType : uint32_t {
    BAG = 0,
    CRATE,
    DRUM,
    RAW_STACK,
    BOTTLE,
    PALLETIZED,
    PERISHABLE
};

/**
 * Physical and Vulkan definition of a cargo commodity item.
 * Adheres to physical mechanics (mass, volume, stacking threshold)
 * and Sascha Willems Vulkan PBR texture block formats.
 */
struct CargoDef {
    CargoID id = CargoID::SUGAR_BAG_50KG;
    std::string name;
    std::string category;
    CargoPackingType packingType = CargoPackingType::BAG;

    // Physical Mechanics (Read by physics/vehicle chassis)
    float weight_kg = 50.0f;
    float volume_m3 = 0.08f;
    Vec3 dimensions{0.8f, 0.5f, 0.2f}; // length, width, height in meters
    float density_kg_m3 = 625.0f;
    float max_stack_load_kg = 1500.0f;
    bool is_liquid_or_bulk = false;
    bool is_perishable = false;

    // Vulkan Formats & Pipeline Configuration (Sascha Willems PBR texturing)
    VkFormat albedo_format = VK_FORMAT_BC7_SRGB_BLOCK;
    VkFormat normal_format = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormat orm_format = VK_FORMAT_BC7_UNORM_BLOCK;
    uint32_t texture_width = 1024;
    uint32_t texture_height = 1024;

    // Material Optical Properties
    float opacity_bias = 1.0f; // 1.0 = opaque, 0.1-0.3 = glass/translucent
    float roughness = 0.7f;
    float metallic = 0.0f;
    Vec4 base_color{0.95f, 0.95f, 0.95f, 1.0f};

    // Stencil, Label & Typography integration (imported from shared/text/TextDef.h)
    std::string label_text = "50 KG NET";
    std::string brand_id = "GENERIC_STENCIL";
    Vec4 label_color{0.08f, 0.18f, 0.55f, 1.0f};

    // Procedural Noise & Geometry Dispatches
    uint32_t seed = 0xCA860001;
    float wear_and_tear = 0.05f;       // Surface abrasion, rust, dust [0.0 - 1.0]
    float wrinkle_or_corrugation = 0.35f; // Fold/rib amplitude
    uint32_t mesh_lod_subdivisions = 16;
};

} // namespace sphere::cargo
